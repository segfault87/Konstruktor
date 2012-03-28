// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QFileDialog>
#include <QIcon>
#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QTemporaryFile>
#include <QVBoxLayout>

#include <libldr/model.h>

#include "application.h"
#include "povrayexporter.h"
#include "povrayrenderparameters.h"
#include "scanlinewidget.h"

#include "povrayrenderwidget.h"

namespace Konstruktor
{

POVRayRenderWidget::POVRayRenderWidget(const POVRayRenderParameters &params, const ldraw::model *model, QWidget *parent)
    : QDialog(parent)
{
  process_ = 0L;
  
  width_ = params.width();
  height_ = params.height();
  antialiasing_ = params.antialiasing();
  
  rcvHeader_ = true;
  rcvHeaderBytes_ = 0;
  rcvPixels_ = 0;
  progress_ = 0;
  numRestBytes_ = 0;
  line_ = 0;
  column_ = 0;
  skipBytes_ = 0;
  
  image_ = QImage(width_, height_, QImage::Format_ARGB32);
  image_.fill(qRgba(0, 0, 0, 0));
  
  // Generate POV file
  tempFile_ = new QTemporaryFile;
  
  if (!tempFile_->open()) {
    QMessageBox::critical(this, tr("Error"), tr("Error writing to temporary file."));
    close();
  }
  
  exporter_ = new POVRayExporter(model, &params, this);
  exporter_->start();
  
  tempFile_->write(exporter_->output());
  tempFile_->flush();
  
  // GUI setup
  setWindowTitle("Render Window");
  
  QVBoxLayout *vboxLayout = new QVBoxLayout(this);
  
  scrollArea_ = new ScanlineWidgetContainer(image_, this);
  
  ScanlineWidget *scanlineWidget = scrollArea_->scanlineWidget();
  
  status_ = new QLabel(tr("Idle"), this);
  status_->setAlignment(Qt::AlignHCenter);
  progressBar_ = new QProgressBar(this);
  progressBar_->setMaximum(100);
  
  vboxLayout->addWidget(scrollArea_);
  vboxLayout->addWidget(status_);
  vboxLayout->addWidget(progressBar_);
  
  connect(this, SIGNAL(lineFinished(int)), scanlineWidget, SLOT(updateLine(int)));
  connect(this, SIGNAL(percent(int)), progressBar_, SLOT(setValue(int)));
  
  setLayout(vboxLayout);
}

POVRayRenderWidget::~POVRayRenderWidget()
{
  if (process_)
    cancelRender();
  
  tempFile_->close();
  delete tempFile_;
}

void POVRayRenderWidget::start()
{
  //enableButton(QDialog::Close, false);
  //enableButton(QDialog::User1, true);
  
  error_ = false;
  terminated_ = false;
  
  process_ = new QProcess(this);

  QStringList args;
  args << QString("+I")+tempFile_->fileName() << "+O-" << "+FT" << "-D" << QString("-w%1").arg(width_) << QString("-h%1").arg(height_) << "+UA";
  if (antialiasing_)
    args << "+a0.3";
  
  connect(process_, SIGNAL(readyReadStandardError()), this, SLOT(povrayMessage()));
  connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(povrayImage()));
  connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(povrayFinished(int, QProcess::ExitStatus)));
  
  process_->start(Application::self()->config()->povRayExecutablePath(), args);
  
  status_->setText(tr("Rendering..."));
}

void POVRayRenderWidget::cancelRender()
{
  if (process_) {
    terminated_ = true;
    process_->terminate();
  }
}

void POVRayRenderWidget::saveImage()
{
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), QString(), tr("Portable Network Graphics (*.png)"));
  
  if (!filename.endsWith(".png", Qt::CaseInsensitive))
    filename += ".png";
  
  if (filename.isEmpty())
    return;
  
  image_.save(filename, "PNG");
}

void POVRayRenderWidget::povrayMessage()
{
  process_->setReadChannel(QProcess::StandardError);
  
  QString output = process_->readAll();
  
  // Workaround
  // It's impossible to determine the process state from the return code
  if (output.toLower().contains("parse error"))
    error_ = true;
  
  povrayOutput_ += output;
}

/* Parse Targa stream and display */
void POVRayRenderWidget::povrayImage()
{
  process_->setReadChannel(QProcess::StandardOutput);
  
  QByteArray buffer = process_->readAll();
  int buflen = buffer.size();
  
  int index = 0;
  int i;
  int oldline = line_;
  
  if (rcvHeader_) {
    // Targa header
    while ((rcvHeaderBytes_ < 18) && (index < buflen)) {
      header_[rcvHeaderBytes_] = (unsigned char)buffer[index];
      ++rcvHeaderBytes_;
      ++index;
    }
    
    if (rcvHeaderBytes_ == 18) {
      // Header received
      rcvHeader_ = false;
      skipBytes_ = header_[0];
      bytespp_ = header_[16] / 8;
    }
  }
  
  if (skipBytes_ > 0) {
    int skip = buflen - index;
    if (skip > skipBytes_)
      skip = skipBytes_;
    skipBytes_ -= skip;
    index += skip;
  }
  
  if ((numRestBytes_ > 0) && (index < buflen)) {
    while ((numRestBytes_ < bytespp_) && (index < buflen)) {
      restBytes_[numRestBytes_] = (unsigned char)buffer[index];
      ++index;
      ++numRestBytes_;
    }
    if (numRestBytes_ == bytespp_) {
      numRestBytes_ = 0;
      if (bytespp_ == 4)
        setPixel(column_, line_, qRgba(restBytes_[2], restBytes_[1], restBytes_[0], restBytes_[3]));
      else
        setPixel(column_, line_, qRgb(restBytes_[2], restBytes_[1], restBytes_[0]));
      ++column_;
      ++rcvPixels_;
      if (column_ == width_) {
        column_ = 0;
        ++line_;
      }
    }
  }
  
  if (index < buflen) {
    int num = (buflen - index) / bytespp_;
    for (i = 0; i < num; ++i) {
      if (bytespp_ == 4)
        setPixel(column_, line_, qRgba(buffer[index+2], buffer[index+1], buffer[index], buffer[index+3]));
      else
        setPixel(column_, line_, qRgb(buffer[index+2], buffer[index+1], buffer[index]));
      
      index += bytespp_;
      
      ++column_;
      ++rcvPixels_;
      if (column_ == width_) {
        column_ = 0;
        ++line_;
      }
    }
    
    numRestBytes_ = buflen - index;
    for (i = 0; i < numRestBytes_; ++i) {
      restBytes_[i] = buffer[index];
      ++index;
    }
  }
  
  if (line_ != oldline)
    emit lineFinished(line_ - 1);
  
  int oldProgress = progress_;
  int numPixels = 0;
  numPixels = width_ * height_;
  
  progress_ = rcvPixels_ * 100 / numPixels;
  
  if (progress_ != oldProgress)
    emit percent(progress_);
}

void POVRayRenderWidget::povrayFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
  //enableButton(QDialog::Close, true);
  //enableButton(QDialog::User1, false);
  
  if (terminated_) {
    status_->setText(tr("Aborted."));
  } else if (exitStatus == QProcess::CrashExit || exitCode || error_) {
    QMessageBox *mb = new QMessageBox();
    mb->addButton(QMessageBox::Ok);
    mb->setDefaultButton(QMessageBox::Ok);
    mb->setText(tr("Rendering failed. Please refer to the POV-Ray output message below."));
    mb->setDetailedText(povrayOutput_);
    mb->exec();
    status_->setText(tr("Rendering failed."));
  } else {
    status_->setText(tr("Finished."));
    //enableButton(QDialog::User2, true);
  }
  
  delete process_;
  process_ = 0L;
}

void POVRayRenderWidget::setPixel(int x, int y, uint c)
{
  if (x >= 0 && x < image_.width() && y >= 0 && y < image_.height())
    image_.setPixel(x, y, c);
}

}
