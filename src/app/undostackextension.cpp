// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include "undostackextension.h"

KonstruktorUndoStackExtension::KonstruktorUndoStackExtension(ldraw::model *m, void *arg)
	: ldraw::extension(m, arg), QUndoStack(reinterpret_cast<QObject *>(arg))
{

}



