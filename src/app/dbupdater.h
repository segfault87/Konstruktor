// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _DBUPDATER_H_
#define _DBUPDATER_H_

#include <string>

#include <QDir>
#include <QHash>
#include <QThread>

#define DB_REVISION_NUMBER 3

namespace ldraw
{
  class part_library;
  class reader;
}

namespace Konstruktor
{

class DBManager;
class PixmapRenderer;
class Config;

class DBUpdater : public QThread
{
  Q_OBJECT;

 public:
  DBUpdater(const std::string &path, bool forceRescan = false, QObject *parent = 0L);
  ~DBUpdater();
  
  void dropOutdatedTables();
  void constructTables();
  void deleteAll();

  void setup();
	void finalize();

  virtual void run();
	void runSingleThreaded();

signals:
	void nextStep();
  void progress(int current, int total, const std::string &name, const std::string &desc);
	void scanFinished();

 private slots:
	void step();
  
 private:
	void increment();

  bool checkTable(const QString &name);
  QString saveLocation(const QString &path);
  
  static QString escape(const QString &string);
  static QString unescape(const QString &string);
  
  void determineSize(const QString &str, float &xs, float &ys, float &zs);
  float floatify(const QString &str);
  void deletePartImages();

	bool runningInThread_;
  
  DBManager *manager_;
  PixmapRenderer *renderer_;
  Config *config_;
  ldraw::part_library *library_;
  ldraw::reader *reader_;
  
  std::string path_;
  bool forceRescan_;
  int round_;
  std::map<std::string, std::string>::const_iterator iterator_, end_;
  bool inTransaction_;
	int totalSize_;
	QHash<QString, int> categories_;
	QDir directory_;
	QString imagePath_;
};

}

#endif

