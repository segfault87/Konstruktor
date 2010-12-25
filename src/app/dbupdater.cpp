// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2008 Park "segfault" J. K. <mastermind@planetmono.org>

#include <iostream>

#include <QDir>
#include <QDirIterator>
#include <QFSFileEngine>
#include <QGLWidget>
#include <QSet>
#include <QStringList>

#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <kstandarddirs.h>

#include <libldr/color.h>
#include <libldr/metrics.h>
#include <libldr/model.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <libldr/utils.h>

#include "config.h"
#include "dbmanager.h"
#include "dbupdater.h"
#include "pixmaprenderer.h"

#include "dbupdater.h"

KonstruktorDBUpdater::KonstruktorDBUpdater(QObject *parent)
	: QObject(parent)
{
	config_ = 0L;
	reader_ = 0L;
	library_ = 0L;
	status_ = false;
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (!args->count()) {
		std::cerr << "No LDraw path given." << std::endl;
		return;
	}

	if (args->isSet("rescan"))
		forceRescan_ = true;
	else
		forceRescan_ = false;

	try {
		library_ = new ldraw::part_library(args->arg(0).toLocal8Bit().data());
	} catch (const ldraw::exception &e) {
		std::cerr << e.what() << std::endl;
		return;
	}

	manager_ = new KonstruktorDBManager(this);
	manager_->initialize(saveLocation("") + "parts.db");
	if (!manager_->isInitialized()) {
		std::cerr << "could not open database file" << std::endl;
		delete manager_;
		return;
	}
	
	status_ = true;

	library_->set_unlink_policy(ldraw::part_library::parts);
	ldraw::color::init();
	reader_ = new ldraw::reader(library_->ldrawpath(ldraw::part_library::ldraw_parts_path));
	
	config_ = new KonstruktorConfig;
	
	QSize pixsize = config_->thumbnailSize();
	renderer_ = new KonstruktorPixmapRenderer(pixsize.width(), pixsize.height());

	dropOutdatedTables();
	constructTables();
}

KonstruktorDBUpdater::~KonstruktorDBUpdater()
{
	if (config_) delete config_;
	if (reader_) delete reader_;
	if (library_) delete library_;
}

void KonstruktorDBUpdater::dropOutdatedTables()
{
	if (config_->databaseRevision() != DB_REVISION_NUMBER) {
		manager_->query("DROP TABLE parts");
		manager_->query("DROP TABLE categories");
		manager_->query("DROP TABLE part_categories");
		manager_->query("DROP TABLE part_keywords");
		config_->setPartCount(-1);
		
		deletePartImages();
	}
}

void KonstruktorDBUpdater::constructTables()
{
	if (!checkTable("parts")) {
		manager_->query(
			"CREATE TABLE parts ("
			"    id INTEGER PRIMARY KEY,"
			"    partid TEXT,"
			"    desc TEXT,"
			"    filename TEXT,"
			"    xsize REAL,"
			"    ysize REAL,"
			"    zsize REAL,"
			"    minx REAL,"
			"    maxx REAL,"
			"    miny REAL,"
			"    maxy REAL,"
			"    minz REAL,"
			"    maxz REAL,"
			"    unofficial INTEGER DEFAULT 0,"
			"    size INTEGER,"
			"    magic INTEGER,"
			"    count INTEGER DEFAULT 0"
			");"
		);
	}
	
	if (!checkTable("categories")) {
		manager_->query(
			"CREATE TABLE categories ("
			"    id INTEGER PRIMARY KEY,"
			"    category TEXT,"
			"    visibility INTEGER DEFAULT 0"
			");"
		);
	}
	
	if (!checkTable("part_categories")) {
		manager_->query(
			"CREATE TABLE part_categories ("
			"    partid INTEGER,"
			"    catid INTEGER"
			");"
		);
	}
	
	if (!checkTable("part_keywords")) {
		manager_->query(
			"CREATE TABLE part_keywords ("
			"    partid INTEGER,"
			"    keyword TEXT"
			");"
		);
	}
	
	if (!checkTable("favorites")) {
		manager_->query(
			"CREATE TABLE favorites ("
			"    partid TEXT,"
			"    group TEXT"
			");"
		);
	}
}

void KonstruktorDBUpdater::deleteAll()
{
	manager_->query("DELETE FROM parts");
	manager_->query("DELETE FROM categories");
	manager_->query("DELETE FROM part_categories");
	manager_->query("DELETE FROM part_keywords");

	deletePartImages();

	config_->setPartCount(-1);
	config_->writeConfig();
}

int KonstruktorDBUpdater::start()
{
	if (!status_)
		return 1;
	
	QHash<QString, int> categories;

	QDir dir(library_->ldrawpath(ldraw::part_library::ldraw_parts_path).c_str());
	QString path = saveLocation("partimgs/");

	const std::map<std::string, std::string> &partlist = library_->part_list();
	int totalSize = partlist.size();

	if (forceRescan_) {
		deleteAll();
	} else if (config_->partCount() == totalSize) {
		return 0;
	} else {
		// To rescan after interrupted
		config_->setPartCount(-1);
		config_->writeConfig();
	}

	std::cout << "0 " << (totalSize - 1) << " Starting" << std::endl;
	
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = partlist.begin(); it != partlist.end(); ++it, ++i) {
		// Omit subparts
		std::string fn = ldraw::utils::translate_string((*it).second);
		if (fn[0] == 's' && fn[1] == DIRECTORY_SEPARATOR[0])
			continue;

		QString qFilename = QString((*it).second.c_str());
		int fsize = (int)QFileInfo(dir, qFilename).size();
		bool insert = true;
		int idx = 0;

		// Skip if unchanged
		QStringList fsizeresult = manager_->query(QString("SELECT id, size FROM parts WHERE filename='%1'").arg(escape(qFilename)));
		if (fsizeresult.size()) {
			idx = fsizeresult[0].toInt();
			if (fsizeresult[1].toInt() == fsize) {
				manager_->query(QString("UPDATE parts SET magic=%1 WHERE id=%2").arg(config_->magic()).arg(idx));
				continue;
			} else {
				insert = false;
			}
		}
		
		// load the model
		ldraw::model_multipart *m;
		try {
			m = reader_->load_from_file((*it).second);
		} catch (const ldraw::exception &e) {
			std::cerr << e.what() << std::endl;
			continue;
		}
		
		// If current part is a link to other one, skip it.
		if (ldraw::utils::translate_string(m->main_model()->desc()).find("moved to") != std::string::npos) {
			delete m;
			continue;
		}

		std::cout << i << " " << totalSize - 1 << " " << m->main_model()->name() << " (" << m->main_model()->desc() << ")" << std::endl;

		library_->link(m);
		
		// Render and save
		ldraw::utils::validate_bowtie_quads(m->main_model());
		renderer_->renderToPixmap(m->main_model(), true).save(path+(*it).second.c_str()+".png", "PNG");
		
		// Metrics
		if (!m->main_model()->custom_data<ldraw::metrics>())
			m->main_model()->update_custom_data<ldraw::metrics>();
		const ldraw::metrics *metrics = m->main_model()->custom_data<ldraw::metrics>();
		const ldraw::vector &min = metrics->min();
		const ldraw::vector &max = metrics->max();
		
		QString qPartno = escape(qFilename.section('.', 0, 0));
		QString qDesc = escape(m->main_model()->desc().c_str());

		// Check whether this part is official or unofficial
		int unofficial = 0;
		std::list<std::string> ldraworgheader = m->main_model()->header("LDRAW_ORG");
		if (ldraworgheader.size() > 0 && ldraw::utils::translate_string(*ldraworgheader.begin()).find("unofficial") != std::string::npos)
			unofficial = 1;

		// Regular expression
		float xs, ys, zs;
		determineSize(qDesc, xs, ys, zs);

		// Insert this into db
		if (insert) {
			QString query =
				QString("INSERT INTO parts(partid, desc, filename, xsize, ysize, ") +
				QString("zsize, minx, maxx, miny, maxy, minz, maxz, size, magic, unofficial) ") +
				QString("VALUES('%1', '%2', '%3', %4, %5, %6, ").arg(qPartno, qDesc, escape(qFilename)).arg(xs).arg(ys).arg(zs) +
				QString("%1, %2, %3, %4, %5, %6, ").arg(min.x()).arg(max.x()).arg(min.y()).arg(max.y()).arg(min.z()).arg(max.z()) +
				QString("%1, %2, %3)").arg(fsize).arg(config_->magic()).arg(unofficial);
			idx = manager_->insert(query);
		} else {
			manager_->query(QString("DELETE FROM part_categories WHERE partid=%1").arg(idx));
			manager_->query(QString("DELETE FROM part_keywords WHERE partid=%1").arg(idx));

			QString query =
				QString("UPDATE parts SET partid='%1', desc='%2', filename='%3', ").arg(qPartno, qDesc, escape(qFilename)) +
				QString("xsize=%1, ysize=%2, zsize=%3, minx=%4, maxx=%5, ").arg(xs).arg(ys).arg(zs).arg(min.x()).arg(max.x()) +
				QString("miny=%1, maxy=%2, minz=%3, maxz=%4, size=%5, ").arg(min.y()).arg(max.y()).arg(min.z()).arg(max.z()).arg(fsize) +
				QString("magic=%1, unofficial=%2 WHERE id=%3").arg(config_->magic()).arg(unofficial).arg(idx);
			manager_->insert(query);
		}

		// Categories
		QSet<QString> setCats;
		std::list<std::string> catheader = m->main_model()->header("CATEGORY");

		QString qCategory = qDesc.section(' ', 0, 0);
		if (qCategory[0] == '_' || qCategory[0] == '~') // Colored parts
			qCategory = qCategory.right(qCategory.length()-1);
		setCats.insert(qCategory);
		for (std::list<std::string>::iterator it = catheader.begin(); it != catheader.end(); ++it)
			setCats.insert(QString((*it).c_str()).trimmed());

		for (QSet<QString>::Iterator it = setCats.begin(); it != setCats.end(); ++it) {
			if (!categories.contains(*it)) {
				QStringList lc = manager_->query(QString("SELECT id FROM categories WHERE category='%1'").arg(*it));
				if (lc.isEmpty())
					categories[*it] = manager_->insert(QString("INSERT INTO categories(category) VALUES('%1')").arg(*it));
				else
					categories[*it] = lc[0].toInt();
			}
			manager_->insert(QString("INSERT INTO part_categories(partid, catid) VALUES(%1, %2)").arg(idx).arg(categories[*it]));
		}

		// Keywords
		QSet<QString> setKeywords;
		std::list<std::string> keywordheader = m->main_model()->header("KEYWORDS");
		for (std::list<std::string>::iterator it = keywordheader.begin(); it != keywordheader.end(); ++it) {
			QStringList ls = QString((*it).c_str()).split(',');
			for (int i = 0; i < ls.size(); ++i)
				setKeywords.insert(ls[i].trimmed());
		}

		for (QSet<QString>::Iterator it = setKeywords.begin(); it != setKeywords.end(); ++it)
			manager_->insert(QString("INSERT INTO part_keywords(partid, keyword) VALUES(%1, '%2')").arg(idx).arg(escape(*it)));
		
		delete m;
	}

	// Delete remainings
	QStringList remainings = manager_->query(QString("SELECT id, partid, filename FROM parts WHERE magic != %1").arg(config_->magic()));
	for (QStringList::Iterator it = remainings.begin(); it != remainings.end(); ++it) {
		int pcnt = (*it++).toInt();
		
		manager_->query(QString("DELETE FROM parts WHERE id=%1").arg(pcnt));
		manager_->query(QString("DELETE FROM part_categories WHERE partid=%1").arg(pcnt));
		manager_->query(QString("DELETE FROM part_keywords WHERE partid=%1").arg(pcnt));
		manager_->query(QString("DELETE FROM favorites WHERE partid='%1'").arg(*it++));

		QFSFileEngine fs(path + (*it) + ".png");
		fs.remove();
	}

	std::cout << (totalSize - 1) << " " << (totalSize - 1) << " Finished" << std::endl;

	config_->setDatabaseRevision(DB_REVISION_NUMBER);
	config_->setPartCount(totalSize);
	config_->setMagic(config_->magic() + 1);
	config_->writeConfig();
	
	return 0;
}

bool KonstruktorDBUpdater::checkTable(const QString &name)
{
	if (manager_->query(QString("SELECT name FROM SQLITE_MASTER WHERE name='%1'").arg(escape(name))).isEmpty())
		return false;
	else
		return true;
}

QString KonstruktorDBUpdater::saveLocation(const QString &path)
{
	globalMutex_.lock();
	QString result = KGlobal::dirs()->saveLocation("data", QString("konstruktor/") + path, true);
	globalMutex_.unlock();


	return result;
}

QString KonstruktorDBUpdater::escape(const QString &string)
{
	QString n = string;
	n.replace('\'', "''");

	return n;
}

QString KonstruktorDBUpdater::unescape(const QString &string)
{
	QString r = string;
	r.replace("''", "'");
	return r;
}

void KonstruktorDBUpdater::determineSize(const QString &str, float &xs, float &ys, float &zs)
{
	static QRegExp triplet("([./\\d]+) *x *([./\\d]+) *x *([./\\d]+)");
	static QRegExp pair("([./\\d]+) *x *([./\\d]+)");
	static QRegExp single("([./\\d]+)");

	if (triplet.indexIn(str) > -1) {
		xs = floatify(triplet.cap(1));
		ys = floatify(triplet.cap(2));
		zs = floatify(triplet.cap(3));
	} else if (pair.indexIn(str) > -1) {
		xs = floatify(pair.cap(1));
		ys = floatify(pair.cap(2));
		zs = 0.0f;
	} else {
		int pos = 0;
		xs = 0.0f;
		while ((pos = single.indexIn(str, pos)) != 1) {
			xs = floatify(single.cap(1));
			if (xs < 32.0f) // empirical
				break;
			pos += single.matchedLength();
		}
		ys = 0.0f;
		zs = 0.0f;
	}
}

float KonstruktorDBUpdater::floatify(const QString &str)
{
	if (str.contains('/'))
		return str.section('/', 0, 0).toFloat() / str.section('/', 1, 1).toFloat();
	else
		return str.toFloat();
}

void KonstruktorDBUpdater::deletePartImages()
{
	QDir dir(saveLocation("partimgs/"));
	QDirIterator it(dir);

	while (it.hasNext()) {
		QFSFileEngine fs(it.next());
		fs.remove();
	}
}
