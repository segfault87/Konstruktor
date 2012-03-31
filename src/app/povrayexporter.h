// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#ifndef _POVRAYEXPORTER_H_
#define _POVRAYEXPORTER_H_

#include <set>
#include <string>

#include <QByteArray>
#include <QObject>
#include <QTextStream>

namespace ldraw {
  class metrics;
  class model;
}

class QIODevice;

namespace Konstruktor
{

class POVRayRenderParameters;

class POVRayExporter : public QObject
{
 public:
  POVRayExporter(const ldraw::model *m, const POVRayRenderParameters *p, QObject *parent = 0L);
  
  void start();
  const QByteArray& output() const { return output_; }
  
 private:
  QString convertNameFormat(const std::string &s);
  
  void fillHeader();
  void fillColors();
  void fillColorsRecursive(const ldraw::model *m, std::set<int> &cset);
  void fillParts();
  void fillPartsRecursive(const ldraw::model_multipart *main, const ldraw::model *m, std::set<const ldraw::model *> &cset, std::set<const ldraw::model *> &bl);
  void fillFooter();
  
  // Finishes
  void setPlane(const ldraw::metrics &metrics);
  void setBackground();
  void setCamera(const ldraw::metrics &metrics);
  void setLights(const ldraw::metrics &metrics);
  
  // math object serializer
  QString serialize(const ldraw::vector &v);
  QString serialize(const ldraw::matrix &m);
  
  // primitive substitutes for native pov-ray primitive objects
  bool substitute(const QString &name);
  void substituteStud();
  void substituteStud2();
  void substituteStud3();
  void substituteStud4();
  void substitutePeghole();
  void substitute2Dash4Cyli();
  void substitute2Dash4Ndis();
  void substitute2Dash4Disc();
  void substitute4Dash4Cyli();
  void substitute4Dash4Ndis();
  void substitute8Dash8Sphe();
  
  bool colorAmbiguityTest(const ldraw::model *m);
  
 private:
  QByteArray output_;
  QTextStream stream_;
  const ldraw::model *model_;
  const POVRayRenderParameters *params_;
};

}

#endif
