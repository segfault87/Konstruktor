// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QProcess>

#include <kdialog.h>

namespace ldraw { class model; }

class QLabel;
class QProgressBar;

class KTemporaryFile;
class KProcess;

class KonstruktorPOVRayRenderParameters;
class KonstruktorPOVRayExporter;
class KonstruktorScanlineWidgetContainer;

class KonstruktorPOVRayRenderWidget : public KDialog
{
	Q_OBJECT;
	
  public:
	KonstruktorPOVRayRenderWidget(const KonstruktorPOVRayRenderParameters &params, const ldraw::model *model, QWidget *parent = 0L);
	~KonstruktorPOVRayRenderWidget();
	
	QString povrayOutput() const { return povrayOutput_; }
	const QImage& image() const { return image_; }
	
  signals:
	void lineFinished(int line);
	void percent(int percent);
	void povrayMessage(const QString &msg);
										  
  public slots:
	void start();
	void cancelRender();
	void saveImage();
	
  private slots:
	void povrayMessage();
	void povrayImage();
	void povrayFinished(int exitCode, QProcess::ExitStatus exitStatus);
	
  private:
	void setPixel(int x, int y, uint c);

	KonstruktorScanlineWidgetContainer *scrollArea_;
	QLabel *status_;
	QProgressBar *progressBar_;
	
	KonstruktorPOVRayExporter *exporter_;
	
	KTemporaryFile *tempFile_;
	KProcess *process_;
	QString povrayOutput_;
	QImage image_;
	bool error_;
	bool terminated_;
	int width_;
	int height_;
	bool antialiasing_;

	// targa reader
	bool rcvHeader_;
	unsigned char header_[18];
	int rcvHeaderBytes_;
	int skipBytes_;
	int bytespp_;
	int rcvPixels_;
	int progress_;
	unsigned char restBytes_[4];
	int numRestBytes_;
	int line_;
	int column_;
};
