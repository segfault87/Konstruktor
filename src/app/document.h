// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _DOCUMENT_H_
#define _DOCUMENT_H_

#include <map>
#include <set>
#include <string>
#include <utility>

#include <QByteArray>
#include <QList>
#include <QObject>

#include <KUrl>

#include <libldr/math.h>
#include <libldr/model.h>
#include <renderer/mouse_rotation.h>

#include "viewport.h"

extern const ldraw::matrix isometricProjectionMatrix;

class QGLWidget;
class QUndoStack;
class KonstruktorPixmapRenderer;
class KonstruktorSubmodelModel;

class KonstruktorDocument : public QObject
{
	Q_OBJECT;
	
  public:
	KonstruktorDocument(const QString &name, const QString &desc, const QString &author, QGLWidget *glBase, QObject *parent = 0L);
	KonstruktorDocument(const QString &path, const KUrl &url, QGLWidget *glBase, QObject *parent = 0L);
	~KonstruktorDocument();

	void sendSignals();
	
	QByteArray save(bool forceMultipart = true) const;
	
	ldraw::model_multipart* contents() { return modelBase_; }
	ldraw::model* getActiveModel() { return activeModel_; }
	bool setActiveModel(ldraw::model *m);
	bool setActiveModel(const std::string &key);
	
	const KUrl& location() const { return location_; }
	bool canSave() const { return canSave_; }
	
	void setLocation(const KUrl &u) { location_ = u; }
	void setSaveable(bool s);

	KonstruktorSubmodelModel* model() { return model_; }
	
	// Manipulation
	ldraw::model* newSubmodel(const std::string &name, const std::string &desc, const std::string &author);
	void renameSubmodel(ldraw::model *model, const std::string &newname, const std::string &newdesc);
	void deleteSubmodel(ldraw::model *model);
	
	void updatePixmap();
	bool updatePixmap(ldraw::model *model);

	QUndoStack* activeUndoStack();
	QList<QUndoStack *> undoStacks();

	// Viewport-related stuff
	KonstruktorViewport& getViewport(int i) { return viewport_[i]; }
	ldraw_renderer::mouse_rotation& getMouseRotation() { return rotation_;}
	const ldraw::vector& getCenter() const { return center_; }
	const ldraw::vector& getLength() const { return length_; }
	
	void recalibrateScreenDimension();
	void resetPerspective();

  signals:
	void undoStackAdded(QUndoStack *editor);
	void undoStackChanged(QUndoStack *editor);
	
  private:
	ldraw::model_multipart *modelBase_;

	ldraw::model *activeModel_;
	QUndoStack *activeUndoStack_;
	
	KonstruktorViewport viewport_[7];
	ldraw_renderer::mouse_rotation rotation_;
	ldraw::vector center_, length_;
	
	KUrl location_;
	
	bool canSave_;
	
	KonstruktorPixmapRenderer *renderer_;
	KonstruktorSubmodelModel *model_;
	
	//friend class KonstruktorPixmapUpdater;
};

#endif
