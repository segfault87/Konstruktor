// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_

#include <QObject>
#include <QThread>

struct sqlite3;

class QString;
class QStringList;
class KProgressDialog;
class KonstruktorPixmapRenderer;

class KonstruktorDBManager : public QObject
{
	Q_OBJECT;
	
  public:
	KonstruktorDBManager(QObject *parent = 0L);
	~KonstruktorDBManager();
	
	void initialize(const QString &path);
	bool isInitialized() const { return isLoaded_; }
	
	QStringList query(const QString &statement);
	int insert(const QString &statement);
	
  private:
	KProgressDialog *rescanDialog_;
	
	sqlite3 *db_;
	bool isLoaded_;
	KonstruktorPixmapRenderer *renderer_;
	KProgressDialog *dialog_;
};

#if 0

class DBUpdater : public QThread
{
	Q_OBJECT;
	
  public:
	DBUpdater(KonstruktorDBManager *parent = 0L);
	
	void run();
	
  signals:
	void updateMaximum(int maximum);
	void updateValue(int value);
	void finished();
	
  private:
	void determineSize(const QString &str, float &xs, float &ys, float &zs);
	float floatify(const QString &str);
	
	KonstruktorDBManager *parent_;
};

#endif

#endif
