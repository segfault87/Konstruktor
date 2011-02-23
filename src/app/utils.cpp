// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <QStringList>

#include "utils.h"

namespace Konstruktor
{

int Utils::maximum(const QSet<int> &set)
{
	int max = -1;

	foreach (int i, set) {
		if (i > max)
			max = i;
	}

	return max;
}

QString Utils::urlFileName(const QUrl &url)
{
	QStringList list;

	QString path = url.path();

	if (path.endsWith('/') || path.endsWith('\\'))
		return QString();

	list = url.path().split('/');
	if (list.size() < 1)
		list = url.path().split('\\');
	
	if (list.size() == 0)
		return QString();

	return list.last();
}

}
