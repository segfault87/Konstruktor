// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <sstream>

#include <QMimeData>
#include <QSet>
#include <QTextStream>

#include <libldr/elements.h>
#include <libldr/model.h>
#include <libldr/reader.h>
#include <libldr/writer.h>

#include "objectlist.h"

namespace Konstruktor
{

const char ObjectList::mimeType[] = "application/x-konstruktor-objectlist";

ObjectList::ObjectList()
{

}

ObjectList::ObjectList(const ObjectList &rhs)
{
	list_ = rhs.list_;
}

ObjectList::ObjectList(const QSet<int> objectSet, const ldraw::model *m)
{
	const std::vector<ldraw::element_base *> &ov = m->elements();
	
	foreach (int id, objectSet) {
		std::ostringstream os;
		ldraw::writer wr(os);

		wr.write(ov[id]);
		os.flush();

		list_.append(os.str());
	}
}

ObjectList::ObjectList(const ldraw::model *m)
{
	const std::vector<ldraw::element_base *> &ov = m->elements();

	for (ldraw::model::const_iterator it = ov.begin(); it != ov.end(); ++it) {
		std::ostringstream os;
		ldraw::writer wr(os);

		wr.write(*it);
		os.flush();

		list_.append(os.str());
	}
}

void ObjectList::clear()
{
	list_.clear();
}

void ObjectList::addObject(const ldraw::element_base *elem)
{
	std::ostringstream os;
	ldraw::writer wr(os);

	wr.write(elem);
	os.flush();


	list_.append(os.str());
}

int ObjectList::length() const
{
	return list_.size();
}

QList<ldraw::element_base *> ObjectList::elements() const
{
	QList<ldraw::element_base *> nlist;

	foreach (const std::string &line, list_) {
		ldraw::element_base *elem = ldraw::reader::parse_line(line);

		if (!elem)
			elem = new ldraw::element_comment("");
		
		nlist.append(elem);
	}

	return nlist;
}

ObjectList& ObjectList::operator= (const ObjectList &rhs)
{
	list_ = rhs.list_;
	
	return *this;
}

QMimeData* ObjectList::mimeData() const
{
	QMimeData *mimeData = new QMimeData();

	QString textoutput;
	QTextStream tos(&textoutput);
	QByteArray output = serialize();

	foreach (const std::string &str, list_)
		tos << str.c_str();

	tos.flush();
	
	mimeData->setText(textoutput);
	mimeData->setData(mimeType, output);

	return mimeData;
}

QByteArray ObjectList::serialize() const
{
	QByteArray ba;
	QDataStream stream(&ba, QIODevice::WriteOnly);

	stream << list_.count();

	foreach (const std::string &str, list_)
		stream << str.c_str();

	return ba;
}

ObjectList ObjectList::deserialize(const QByteArray &data)
{
	ObjectList ol;
	int length;
	QDataStream stream(const_cast<QByteArray *>(&data), QIODevice::ReadOnly); /* WTF! */
	
	stream >> length;

	for (int i = 0; i < length; ++i) {
		char *s = 0L;

		stream >> s;

		ol.addObject(std::string(s));

		delete s;
	}

	return ol;
}

void ObjectList::addObject(const std::string &s)
{
	list_.append(s);
}

}
