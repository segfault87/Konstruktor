#include <iostream>

#include <QApplication>
#include <QFileDialog>
#include <QGLWidget>
#include <QPaintEvent>
#include <QTime>
#include <QTimer>

#include "modelviewer.h"

class ModelViewerWidget : public QGLWidget
{
  public:
	ModelViewerWidget(QWidget *parent);
	~ModelViewerWidget();

  private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent *event);

	int frames_;
	int fps_;
	int timebase_;
	QTime time_;
};

ModelViewerWidget::ModelViewerWidget(QWidget *parent)
	: QGLWidget(parent)
{
	QTimer *timer = new QTimer(this);

	makeCurrent();
	initDisplay();
	doneCurrent();
	
	connect(timer, SIGNAL(timeout()), this, SLOT(update()));
	timer->start(33);

	time_.start();
}

ModelViewerWidget::~ModelViewerWidget()
{

}

void ModelViewerWidget::initializeGL()
{
	
}

void ModelViewerWidget::resizeGL(int width, int height)
{
	//::resize(width, height);
}

void ModelViewerWidget::paintEvent(QPaintEvent *event)
{
	int start, end;
	
	makeCurrent();

	QPainter p;
	p.begin(this);

	start = time_.elapsed();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	renderer_->setup();
	::resize(width(), height());
	::render(start);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	end = time_.elapsed();

	p.drawText(10, 25, QString("%1 ms").arg(end - start));
	p.end();
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	if (!initializeLdraw())
		return -1;

	ModelViewerWidget mvw(0L);

	if (argc > 1) {
		if (std::strcmp(argv[1], "-immediate") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_immediate;
		else if (std::strcmp(argv[1], "-varray") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_varray;
		else if (std::strcmp(argv[1], "-vbo") == 0)
			mode_ = ldraw_renderer::renderer_opengl_factory::mode_vbo;
		else {
			std::cerr << "invalid option." << std::endl;
			return -3;
		}
	}

	QString fn = QFileDialog::getOpenFileName(0L, "Select a file to load", QString(), "LDraw Model Files (*.dat *.ldr *.mpd)");

	if (fn.isNull())
		return -1;
	
	if (!initializeModel(fn.toLocal8Bit().data()))
		return -1;

	mvw.show();

	return app.exec();
};

