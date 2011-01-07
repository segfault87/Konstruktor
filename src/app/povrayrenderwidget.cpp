// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include <kfiledialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktemporaryfile.h>
#include <kprocess.h>

#include <libldr/model.h>

#include "application.h"
#include "povrayexporter.h"
#include "povrayrenderparameters.h"
#include "scanlinewidget.h"

#include "povrayrenderwidget.h"

KonstruktorPOVRayRenderWidget::KonstruktorPOVRayRenderWidget(const KonstruktorPOVRayRenderParameters &params, const ldraw::model *model, QWidget *parent)
	: KDialog(parent)
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
	tempFile_ = new KTemporaryFile;
	tempFile_->setSuffix(".pov");

	if (!tempFile_->open()) {
		KMessageBox::error(this, i18n("Error writing to temporary file."));
		close();
	}
	
	exporter_ = new KonstruktorPOVRayExporter(model, &params, this);
	exporter_->start();
	
	tempFile_->write(exporter_->output());
	tempFile_->flush();

	// GUI setup
	setCaption("Render Window");
	setButtons(KDialog::Close | KDialog::User1 | KDialog::User2);
	setButtonText(KDialog::User2, i18n("&Save"));
	setButtonIcon(KDialog::User2, KIcon("document-save"));
	setButtonText(KDialog::User1, i18n("&Cancel"));
	setButtonIcon(KDialog::User1, KIcon("dialog-cancel"));
	enableButton(KDialog::User1, false);
	enableButton(KDialog::User2, false);
	connect(this, SIGNAL(user1Clicked()), SLOT(cancelRender()));
	connect(this, SIGNAL(user2Clicked()), SLOT(saveImage()));
	
	QWidget *mainWidget = new QWidget;
	QVBoxLayout *vboxLayout = new QVBoxLayout(mainWidget);

	scrollArea_ = new KonstruktorScanlineWidgetContainer(image_, this);

	KonstruktorScanlineWidget *scanlineWidget = scrollArea_->scanlineWidget();
	
	status_ = new QLabel(i18n("Idle"), this);
	status_->setAlignment(Qt::AlignHCenter);
	progressBar_ = new QProgressBar(mainWidget);
	progressBar_->setMaximum(100);
	
	vboxLayout->addWidget(scrollArea_);
	vboxLayout->addWidget(status_);
	vboxLayout->addWidget(progressBar_);

	connect(this, SIGNAL(lineFinished(int)), scanlineWidget, SLOT(updateLine(int)));
	connect(this, SIGNAL(percent(int)), progressBar_, SLOT(setValue(int)));

	setMainWidget(mainWidget);
}

KonstruktorPOVRayRenderWidget::~KonstruktorPOVRayRenderWidget()
{
	if (process_)
		cancelRender();
	
	tempFile_->close();
	delete tempFile_;
}

void KonstruktorPOVRayRenderWidget::start()
{
	enableButton(KDialog::Close, false);
	enableButton(KDialog::User1, true);
	
	error_ = false;
	terminated_ = false;
	
	process_ = new KProcess(this);

	process_->setOutputChannelMode(KProcess::SeparateChannels);
	*process_ << KonstruktorApplication::self()->config()->povRayExecutablePath();
	*process_ << QString("+I")+tempFile_->fileName() << "+O-" << "+FT" << "-D" << QString("-w%1").arg(width_) << QString("-h%1").arg(height_) << "+UA";
	if (antialiasing_)
		*process_ << "+a0.3";

	connect(process_, SIGNAL(readyReadStandardError()), this, SLOT(povrayMessage()));
	connect(process_, SIGNAL(readyReadStandardOutput()), this, SLOT(povrayImage()));
	connect(process_, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(povrayFinished(int, QProcess::ExitStatus)));

	process_->start();

	status_->setText(i18n("Rendering..."));
}

void KonstruktorPOVRayRenderWidget::cancelRender()
{
	if (process_) {
		terminated_ = true;
		process_->terminate();
	}
}

void KonstruktorPOVRayRenderWidget::saveImage()
{
	QString filename = KFileDialog::getSaveFileName(KUrl(), i18n("*.png|Portable Network Graphics"), this, i18n("Save Image File"));

	if (!filename.endsWith(".png", Qt::CaseInsensitive))
		filename += ".png";
	
	if (filename.isEmpty())
		return;

	image_.save(filename, "PNG");
}

void KonstruktorPOVRayRenderWidget::povrayMessage()
{
	process_->setReadChannel(KProcess::StandardError);

	QString output = process_->readAll();

	// Workaround
	// It's impossible to determine the process state from return code
	if (output.toLower().contains("parse error"))
		error_ = true;
	
	povrayOutput_ += output;
}

/* Parse Targa stream and display */
void KonstruktorPOVRayRenderWidget::povrayImage()
{
	process_->setReadChannel(KProcess::StandardOutput);

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

void KonstruktorPOVRayRenderWidget::povrayFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
	enableButton(KDialog::Close, true);
	enableButton(KDialog::User1, false);

	if (terminated_) {
		status_->setText(i18n("Aborted."));
	} else if (exitStatus == QProcess::CrashExit || exitCode || error_) {
		KMessageBox::detailedError(this, i18n("Rendering failed. Please refer to the POV-Ray output message below."), povrayOutput_);
		status_->setText(i18n("Rendering failed."));
	} else {
		status_->setText(i18n("Finished."));
		enableButton(KDialog::User2, true);
	}

	delete process_;
	process_ = 0L;
}

void KonstruktorPOVRayRenderWidget::setPixel(int x, int y, uint c)
{
	if (x >= 0 && x < image_.width() && y >= 0 && y < image_.height())
		image_.setPixel(x, y, c);
}

