// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _OBJECTLIST_H_
#define _OBJECTLIST_H_

#include <string>

#include <QList>

namespace ldraw
{
	class writer;
	class model;
	class element_base;
}

class QMimeData;

namespace Konstruktor
{

class ObjectList
{
  public:
	static const char mimeType[];

	ObjectList();
	ObjectList(const ObjectList &rhs);
	ObjectList(const QSet<int> objectSet, const ldraw::model *m);
	ObjectList(const ldraw::model *m);

	void clear();
	void addObject(const ldraw::element_base *elem);

	int length() const;
	QList<ldraw::element_base*> elements() const;

	ObjectList& operator= (const ObjectList &rhs);

	QMimeData* mimeData() const;

	QByteArray serialize() const;
	static ObjectList deserialize(const QByteArray &data);

  private:
	void addObject(const std::string &s);
	
	QList<std::string> list_;
};

}

Q_DECLARE_METATYPE(Konstruktor::ObjectList);

#endif
