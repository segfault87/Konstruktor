// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <QByteArray>
#include <QFile>
#include <QRegExp>
#include <QStringList>

#include <klocale.h>
#include <kmessagebox.h>
#include <kprogressdialog.h>

#include <sqlite3.h>

#include <libldr/metrics.h>
#include <libldr/model.h>
#include <libldr/part_library.h>
#include <libldr/reader.h>
#include <libldr/utils.h>

#include "dbmanager.h"

// usleep() is not portable
#ifdef WIN32
#define _SLEEP(ms) ::SleepEx(ms < 500 ? 1 : (ms + 500) / 1000, TRUE)
#else
#define _SLEEP(ms) ::usleep(ms)
#endif

KonstruktorDBManager::KonstruktorDBManager(QObject *parent) : QObject(parent)
{
	isLoaded_ = false;
}

KonstruktorDBManager::~KonstruktorDBManager()
{
	if (isLoaded_)
		sqlite3_close(db_);
}

void KonstruktorDBManager::initialize(const QString &path)
{
	const QByteArray epath = QFile::encodeName(path);
	
	QFile file(epath);
	if (file.open(QIODevice::ReadOnly)) {
		char format[50];
		file.readLine(format, 50);
		if (!QByteArray(format).startsWith("SQLite format 3")) {
			file.close();
			QFile::remove(path);
		} else {
			file.close();
		}
	}
	
	if (sqlite3_open(epath, &db_) != SQLITE_OK) {
		KMessageBox::error(0L, i18n("Could not open the part database."));
		return;
	}
	
	isLoaded_ = true;
	
	query("PRAGMA default_synchronous = OFF;");
}

QStringList KonstruktorDBManager::query(const QString &statement)
{
	if (!isLoaded_) // There is no DB connection
		return QStringList();
	
	QStringList values;
	int error, rc = 0;
	const char *tail;
	sqlite3_stmt *stmt;
	int busyCnt = 0;
	int retryCnt = 0;
	
	do {
		do {
			if (busyCnt) {
				_SLEEP(100000);
				// BUSY COUNTER
			}
			error = sqlite3_prepare(db_, statement.toUtf8(), -1, &stmt, &tail);
		} while (error == SQLITE_BUSY && busyCnt++ < 120);
		
		if (error != SQLITE_OK) {
			if (error == SQLITE_BUSY)
				KMessageBox::sorry(0L, i18n("Sorry, Database is locked right now. Please try again later."));
			values = QStringList();
		} else {
			busyCnt = 0;
			int number = sqlite3_column_count(stmt);
			
			while (1) {
				error = sqlite3_step(stmt);
				
				if (error == SQLITE_BUSY) {
					if (busyCnt++ > 120) {
						KMessageBox::sorry(0L, i18n("Sorry, Database is locked right now. Please try again later."));
						break;
					}
					_SLEEP(100000);
					continue;
				}
				if (error == SQLITE_DONE || error == SQLITE_ERROR)
					break;
				
				for (int i = 0; i < number; i++)
					values << QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i)));
			}
			
			rc = sqlite3_finalize(stmt);
			
			if (error != SQLITE_DONE && rc != SQLITE_SCHEMA) {
				values = QStringList();
			}
			if (rc == SQLITE_SCHEMA) {
				retryCnt++;
				if (retryCnt >= 10) {
					KMessageBox::error(0L, i18n("Retry count has reached maximum"));
					values = QStringList();
				}
			}
		}
	} while (rc == SQLITE_SCHEMA && retryCnt < 10);
	
	return values;
}

int KonstruktorDBManager::insert(const QString &statement)
{
	if (!isLoaded_) // There is no DB connection
		return 0;
	
	QStringList values;
	int error, rc = 0;
	const char *tail;
	sqlite3_stmt *stmt;
	int busyCnt = 0;
	int retryCnt = 0;
	
	do {
		do {
			if (busyCnt) {
				_SLEEP(100000);
				// BUSY COUNTER
			}
			error = sqlite3_prepare(db_, statement.toUtf8(), -1, &stmt, &tail);
		} while (error == SQLITE_BUSY && busyCnt++ < 120);
		
		if (error != SQLITE_OK) {
			if (error == SQLITE_BUSY)
				KMessageBox::sorry(0L, i18n("Sorry, Database is locked right now. Please try again later."));
			values = QStringList();
		} else {
			busyCnt = 0;
			
			while (1) {
				error = sqlite3_step(stmt);
				
				if (error == SQLITE_BUSY) {
					if (busyCnt++ > 120) {
						KMessageBox::sorry(0L, i18n("Sorry, Database is locked right now. Please try again later."));
						break;
					}
					_SLEEP(100000);
					continue;
				}
				if (error == SQLITE_DONE || error == SQLITE_ERROR)
					break;
			}
			
			rc = sqlite3_finalize(stmt);
			
			if (error != SQLITE_DONE && rc != SQLITE_SCHEMA) {
				values = QStringList();
			}
			if (rc == SQLITE_SCHEMA) {
				retryCnt++;
				if (retryCnt >= 10) {
					KMessageBox::error(0L, i18n("Retry count has reached maximum"));
					values = QStringList();
				}
			}
		}
	} while (rc == SQLITE_SCHEMA && retryCnt < 10);
	
	return sqlite3_last_insert_rowid(db_);
}

