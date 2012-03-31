// Konstruktor - An interactive LDraw modeler for KDE
// Copyright (c)2006-2011 Park "segfault" J. K. <mastermind@planetmono.org>

#include <cmath>

#include <QIODevice>
#include <QString>

#include <libldr/color.h>
#include <libldr/math.h>
#include <libldr/metrics.h>
#include <libldr/model.h>
#include <libldr/utils.h>

#include "povrayrenderparameters.h"

#include "povrayexporter.h"

#define RADIAN(n) (n / 180.0f * M_PI)

namespace Konstruktor
{

/* POV-Ray exporter
 * Exports a LDraw model to POV-Ray format.
 * Large amount of serialization scheme has been borrowed from L3P.
 * L3P by Lars C. Hassing. (http://www.hassings.dk/l3/l3p.html) 
 */

POVRayExporter::POVRayExporter(const ldraw::model *m, const POVRayRenderParameters *p, QObject *parent)
    : QObject(parent), stream_(&output_, QIODevice::WriteOnly)
{
  model_ = m;
  params_ = p;
}

void POVRayExporter::start()
{
  output_.clear();
  
  fillHeader();
  fillColors();
  fillParts();
  fillFooter();
  
  stream_.flush();
}

QString POVRayExporter::convertNameFormat(const std::string &s)
{
  QString cs(s.c_str());
  
  cs = cs.toLower();
  cs = cs.insert(0, '_');
  cs = cs.replace('.', "_dot_");
  cs = cs.replace('-', "_dash_");
  cs = cs.replace('/', "_slash_");
  cs = cs.replace('\\', "_slash_");
  cs = cs.replace('#', "_sharp_");
  cs = cs.replace(' ', "_");
  cs = cs.replace('(', "");
  cs = cs.replace(')', "");
  cs = cs.replace('~', "_tilde_");
  
  return cs;
}

void POVRayExporter::fillHeader()
{
  // LEGO Logo
  
  stream_ << "#declare L3Logo = union {\n";
  stream_ << "\tsphere { <-59, 0, -96>, 6}\n";
  stream_ << "\tcylinder { <-59, 0, -96>, <59, 0, -122>, 6 open}\n";
  stream_ << "\tsphere { <59, 0, -122>, 6 }\n";
  stream_ << "\tcylinder { <59, 0, -122>, <59, 0, -84>, 6 open}\n";
  stream_ << "\tsphere { <59 ,0 ,-84>, 6 }\n\n";
  
  stream_ << "\tsphere { <-59, 0, -36>, 6 }\n";
  stream_ << "\tcylinder { <-59, 0, -36>, <-59, 0, 1>, 6 open}\n";
  stream_ << "\tsphere { <-59, 0, 1>, 6 }\n";
  stream_ << "\tcylinder { <0, 0, -49>, <0, 0, -25>, 6 open }\n";
  stream_ << "\tsphere { <0, 0, -25>, 6 }\n";
  stream_ << "\tsphere { <59, 0, -62>, 6 }\n";
  stream_ << "\tcylinder { <59, 0, -62>, <59, 0, -24>, 6 open }\n";
  stream_ << "\tsphere { <59, 0, -24>, 6 }\n";
  stream_ << "\tcylinder { <-59 ,0 ,-36>, <59, 0, -62>, 6 open }\n\n";
  
  stream_ << "\tsphere { <-35.95, 0, 57>, 6 }\n";
  stream_ << "\ttorus { 18.45, 6 clipped_by { plane { <40,0,-9>, 0 } } translate <-40, 0, 39> }\n";
  stream_ << "\tcylinder { <-44.05, 0, 21>, <35.95, 0, 3>, 6 open }\n";
  stream_ << "\ttorus { 18.45, 6 clipped_by { plane { <-40, 0, 9>, 0 } } translate <40, 0, 21> }\n";
  stream_ << "\tcylinder { <44.05, 0, 39>, <0, 0, 49>, 6 open }\n";
  stream_ << "\tsphere { <0, 0, 49>, 6 }\n";
  stream_ << "\tcylinder { <0, 0, 49>, <0, 0, 34>, 6 open }\n";
  stream_ << "\tsphere { <0, 0, 34>, 6 }\n\n";
  
  stream_ << "\ttorus { 18.45, 6 clipped_by { plane { <40, 0, -9>, 0 } } translate <-40, 0, 99> }\n";
  stream_ << "\tcylinder { <-44.05, 0, 81>, <35.95, 0, 63>, 6 open }\n";
  stream_ << "\ttorus { 18.45, 6 clipped_by { plane { <-40, 0, 9>,0 } } translate <40, 0, 81> }\n";
  stream_ << "\tcylinder { <44.05, 0, 99>,<-35.95, 0, 117>, 6 open }\n\n";
  
  stream_ << "\tscale 4.5 / 128\n";
  stream_ << "}\n\n";
}

void POVRayExporter::fillColors()
{
  std::set<int> colorList;
  
  fillColorsRecursive(model_, colorList);
}

void POVRayExporter::fillColorsRecursive(const ldraw::model *m, std::set<int> &cset)
{
  int i = 0;
  
  for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++i) {
    int c;
    
    if (i == 0) // At least, color '0' must be exported.
      c = 0;
    else if ((*it)->get_type() == ldraw::type_ref && CAST_AS_REF(*it)->get_model()) {
      c = CAST_AS_REF(*it)->get_color().get_id();
      fillColorsRecursive(CAST_AS_REF(*it)->get_model(), cset);
    } else if ((*it)->get_type() == ldraw::type_triangle)
			c = CAST_AS_TRIANGLE(*it)->get_color().get_id();
    else if ((*it)->get_type() == ldraw::type_quadrilateral)
      c = CAST_AS_QUADRILATERAL(*it)->get_color().get_id();
    else {
      if (i > 0)
        ++it;
      continue;
    }
    
    if (cset.find(c) == cset.end()) {
      ldraw::color ce(c);
      ldraw::material_type material = ce.get_entity()->material;
      const unsigned char *rgba = ce.get_entity()->rgba;
      float ambient;
      float diffuse;
      float reflection;
      int phong_size;
      
      switch (material) {
        case ldraw::material_transparent:
          ambient = 0.3f;
          diffuse = 0.6f;
          reflection = 0.25f;
          phong_size = 60;
          break;
        case ldraw::material_metallic:
          ambient = 0.25f;
          diffuse = 0.6f;
          reflection = 0.6f;
          phong_size = 75;
          break;
        default:
          ambient = 0.17f;
          diffuse = 0.4f;
          reflection = 0.025f;
          phong_size = 20;
      }
      
      cset.insert(c);
      
      stream_ << "#declare Color" << c << " = material {\n";
      stream_ << "\ttexture {\n";
      stream_ << "\t\tpigment { " << (material != ldraw::material_transparent ? "rgb" : "rgbf") << " <" << (float)rgba[0]/255.0f << ", " << (float)rgba[1]/255.0f << ", " << (float)rgba[2]/255.0f;
      if (material == ldraw::material_transparent)
        stream_ << ", " << (float)rgba[3]/255.0f;
      stream_ << "> }\n";
      stream_ << "\t\tfinish { ambient " << ambient << " diffuse " << diffuse << " reflection " << reflection << " ";
      if (material != ldraw::material_metallic)
        stream_ << "phong 0.3 phong_size " << phong_size << " ";
      else
        stream_ << "brilliance 5 metallic specular 0.7 roughness 1/100 ";
      if (material == ldraw::material_transparent)
        stream_ << "refraction 1 ior 1.5 ";
      stream_ << "}\n";
      if (material == ldraw::material_normal)
        stream_ << "\t\tnormal { bumps 0.05 scale 0.02 }\n";
      stream_ << "\t}\n}\n\n";
    }
    
    if (i > 0)
      ++it;
  }
}

void POVRayExporter::fillParts()
{
  std::set<const ldraw::model *> partList;
  std::set<const ldraw::model *> blackList; // Blacklist for null model (model with no renderable elements)
  
  fillPartsRecursive(model_->parent(), model_, partList, blackList);
  
  stream_ << "object { " << convertNameFormat(model_->name()) << " /*material { Color0 }*/ }\n"; 
}

void POVRayExporter::fillPartsRecursive(const ldraw::model_multipart *main, const ldraw::model *m, std::set<const ldraw::model *> &cset, std::set<const ldraw::model *> &blacklist)
{
  bool foundFlag;
  QString modelName = convertNameFormat(m->name());
  
  // Skip this if current part is in the blacklist
  if (blacklist.find(m) != blacklist.end())
    return;
  
  // A group of triangles and quads must be grouped into mesh { ... } block,
  // and a group of meshes must have one identical color.
  // If an object has only one (or less) object, it must be defined as an 'object'
  // 'union' if else.
  int count = 0;
  int cf = -1;
  bool pext = false;
  for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
    if ((*it)->get_type() == ldraw::type_ref) {
      pext = true;
      const ldraw::element_ref *elem = CAST_AS_CONST_REF(*it);
      
      if (!elem->get_model())
        continue;
      
      // Duplicate check
      foundFlag = false;
      
      if (cset.find(elem->get_model()) != cset.end())
        foundFlag = true;
      
      if (!foundFlag)
        fillPartsRecursive(main, elem->get_model(), cset, blacklist);
      
      if (blacklist.find(elem->get_model()) == blacklist.end())
        ++count;
    } else if ((*it)->get_type() == ldraw::type_triangle || (*it)->get_type() == ldraw::type_quadrilateral) {
      int cc = (*it)->get_type() == ldraw::type_triangle ? CAST_AS_TRIANGLE(*it)->get_color().get_id() : CAST_AS_QUADRILATERAL(*it)->get_color().get_id();
      
      if (cf != cc) {
        ++count;
        cf = cc;
      } else if (pext) {
        ++count;
        pext = false;
      }
    }
  }
  if (!count) {
    blacklist.insert(m);
    return; 
  }
  
  cset.insert(m);
  
  stream_ << "#declare " << convertNameFormat(m->name()) << " = ";
  
  // If this is not a part to substitute, serialize it normally
  if (!substitute(convertNameFormat(m->name()))) {
    stream_ << ((count == 1) ? "object" : "union") << " {\n";
    
    // Serialize data
    bool meshFlag = false;
    int colorFlag = -1;
    for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
      // Process triangles and quadrilaterals.
      if ((*it)->get_type() == ldraw::type_triangle || (*it)->get_type() == ldraw::type_quadrilateral) {
        int c = (*it)->get_type() == ldraw::type_triangle ? CAST_AS_TRIANGLE(*it)->get_color().get_id() : CAST_AS_QUADRILATERAL(*it)->get_color().get_id();
        if (colorFlag == -1) {
          colorFlag = c;
        }
	
        // If current " mesh { ... } " block has been finished and one another block is about to start,
        // Terminate the previous one and create new.
        if (colorFlag != c && meshFlag) {
          if (colorFlag != 16 && colorFlag != 24)
            stream_ << "\t\tmaterial { Color" << colorFlag << " }\n";
          stream_ << "\t}\n";
          meshFlag = false;
          colorFlag = c;
        }
	
        // new block
        if (!meshFlag) {
          stream_ << "\tmesh {\n";
          meshFlag = true;
        }
	
        // Serialize!
        if ((*it)->get_type() == ldraw::type_triangle) {
          ldraw::element_triangle *elem = CAST_AS_TRIANGLE(*it);
          stream_ << "\t\ttriangle { " << serialize(elem->pos1()) << ", " << serialize(elem->pos2()) << ", " << serialize(elem->pos3()) << " }\n";
        } else {
          ldraw::element_quadrilateral *elem = CAST_AS_QUADRILATERAL(*it);
          stream_ << "\t\ttriangle { " << serialize(elem->pos1()) << ", " << serialize(elem->pos2()) << ", " << serialize(elem->pos3()) << " }\n";
          stream_ << "\t\ttriangle { " << serialize(elem->pos3()) << ", " << serialize(elem->pos4()) << ", " << serialize(elem->pos1()) << " }\n";
        }
      } else if ((*it)->get_type() == ldraw::type_ref) {
        ldraw::element_ref *elem = CAST_AS_REF(*it);
        
        ldraw::matrix om = elem->get_matrix();
        if (ldraw::utils::is_singular_matrix(om))
          continue;
        
        if (!elem->get_model() || blacklist.find(elem->get_model()) != blacklist.end())
          continue;
	
        if (meshFlag) {
          if (colorFlag != 16 && colorFlag != 24)
            stream_ << "\t\tmaterial { Color" << colorFlag << " }\n";
          
          stream_ << "\t}\n";
          meshFlag = false;
        }
        
        if (elem->parent()->parent() == main && !m->is_submodel_of(main)) {
          // Seam width
          ldraw::metrics metrics(elem->get_model());
          metrics.update();
          float distance = ldraw::vector::distance(metrics.min(), metrics.max());
          float scale = (distance - params_->seamWidth()) / distance;
          
          ldraw::matrix sm;
          sm.value(0, 0) = scale;
          sm.value(1, 1) = scale;
          sm.value(2, 2) = scale;
          
          ldraw::vector v = om.get_translation_vector();
          om = om * sm;
          om.set_translation_vector(v);
        }
	
        stream_ << "\tobject { " << convertNameFormat(elem->get_model()->name()) << " matrix " << serialize(om.transpose());
	
        if (colorAmbiguityTest(elem->get_model()) && elem->get_color().get_id() != 16 && elem->get_color().get_id() != 24)
          stream_ << " material { Color" << elem->get_color().get_id() << " } ";
        stream_ << "}\n";
      }
    }
    
    if (meshFlag) {
      if (colorFlag != 16 && colorFlag != 24)
        stream_ << "\t\tmaterial { Color" << colorFlag << " }\n";
      
      stream_ << "\t}\n";
    }
    stream_ << "}\n\n";
  }
}

void POVRayExporter::fillFooter()
{
  ldraw::metrics metrics(const_cast<ldraw::model *>(model_));
  metrics.update();
  
  setPlane(metrics);
  setBackground();
  setCamera(metrics);
  setLights(metrics);
}

void POVRayExporter::setPlane(const ldraw::metrics &metrics)
{
  if (!params_->drawPlane())
    return;
  
  const QColor &c = params_->planeColor();
  
  stream_ << "object {\n";
  stream_ << "\tplane { y, " << (params_->planePositionAuto() ? metrics.max().y() : params_->planePosition()) << " hollow }\n";
  stream_ << "\ttexture { pigment { color rgb <" << (float)c.red()/255.0f << ", " << (float)c.green()/255.0f << ", " << (float)c.blue()/255.0f << "> } finish { ambient 0.4 diffuse 0.4 } }\n";
  stream_ << "}\n\n";
}

void POVRayExporter::setBackground()
{
  const QColor &c = params_->backgroundColor(); 
  
  stream_ << "background { color rgb <" << (float)c.red()/255.0f << ", " << (float)c.green()/255.0f << "> }\n";
}

// Determine camera position
// NOTE Current viewport calculation method doesn't fit model on screen exactly.
//      Anyone have better idea?
void POVRayExporter::setCamera(const ldraw::metrics &metrics)
{
  const float latitude = RADIAN(params_->cameraLatitude());
  const float longitude = RADIAN(params_->cameraLongitude());
  
  ldraw::vector center = (metrics.min() + metrics.max()) * 0.5f;
  const ldraw::vector lv = metrics.max() - center;
  const float diameter = sqrt(lv.x()*lv.x() + lv.y()*lv.y() + lv.z()*lv.z()) * 2.0f;
  const ldraw::vector camera = ldraw::vector(std::sin(latitude) * std::cos(longitude), -std::sin(longitude), std::cos(latitude) * std::cos(longitude)) * diameter * params_->cameraRadius() + center;
  
  stream_ << "camera {\n";
  if (params_->cameraOrthographic())
    stream_ << "\torthographic\n";
  stream_ << "\tlocation " << serialize(camera) << "\n";
  stream_ << "\tsky -y\n";
  stream_ << "\tright -" << params_->width() << " / " << params_->height() << " * x\n";
  stream_ << "\tlook_at " << serialize(center) << "\n";
  if (!params_->cameraOrthographic())
    stream_ << "\tangle " << params_->cameraFOV() << "\n";
  stream_ << "}\n\n";
}

void POVRayExporter::setLights(const ldraw::metrics &metrics)
{
  const float longitude = RADIAN(params_->lightsLongitude());
  const ldraw::vector center = (metrics.min() + metrics.max()) * 0.5f;
  const ldraw::vector lv = metrics.max() - center;
  const float diameter = std::sqrt(lv.x()*lv.x() + lv.y()*lv.y() + lv.z()*lv.z()) * 2.0f;
  const QColor color = params_->lightsColor();
  
  float latitude;
  ldraw::vector lpos;
  for (int i = 0; i < params_->lights(); ++i) {
    latitude = RADIAN(params_->lightsLatitudeOffset()) + i * (M_PI / params_->lights());
    if (latitude > M_PI) latitude -= M_PI;
    lpos = ldraw::vector(std::sin(latitude) * std::cos(longitude), -std::sin(longitude), std::cos(latitude) * std::cos(longitude)) * diameter * params_->lightsRadius() + center;
    
    stream_ << "light_source {\n";
    stream_ << "\t<" << lpos.x() << ", " << lpos.y() << ", " << lpos.z() << ">\n";
    stream_ << "\tcolor rgb <" << color.red() / 255.0f << ", " << color.green() / 255.0f << ", " << color.blue() / 255.0f << "> * " << (params_->lightsIntensity() / params_->lights() * 2.8f) << "\n";
    stream_ << "}\n\n";
  }
}

QString POVRayExporter::serialize(const ldraw::vector &v)
{
  return QString("<%1, %2, %3>").arg(v.x()).arg(v.y()).arg(v.z());
}

QString POVRayExporter::serialize(const ldraw::matrix &m)
{
  return QString("<%1, %2, %3, ").arg(m.value(0, 0)).arg(m.value(0, 1)).arg(m.value(0, 2)) +
      QString("%1, %2, %3, ").arg(m.value(1, 0)).arg(m.value(1, 1)).arg(m.value(1, 2)) +
      QString("%1, %2, %3, ").arg(m.value(2, 0)).arg(m.value(2, 1)).arg(m.value(2, 2)) +
      QString("%1, %2, %3>").arg(m.value(3, 0)).arg(m.value(3, 1)).arg(m.value(3, 2));
}

bool POVRayExporter::substitute(const QString &name)
{
  if (name == QString("_stud_dot_dat")) {
    substituteStud();
    return true;
  } else if (name == QString("_stud2_dot_dat")) {
    substituteStud2();
    return true;
  } else if (name == QString("_stud3_dot_dat")) {
    substituteStud3();
    return true;
  } else if (name == QString("_stud4_dot_dat")) {
    substituteStud4();
    return true;
  } else if (name == QString("_peghole_dot_dat")) {
    substitutePeghole();
    return true;
  } else if (name == QString("_2_dash_4cyli_dot_dat")) {
    substitute2Dash4Cyli();
    return true;
  } else if (name == QString("_2_dash_4ndis_dot_dat")) {
    substitute2Dash4Ndis();
    return true;
  } else if (name == QString("_2_dash_4disc_dot_dat")) {
    substitute2Dash4Disc();
    return true;
  } else if (name == QString("_4_dash_4cyli_dot_dat")) {
    substitute4Dash4Cyli();
    return true;
  } else if (name == QString("_4_dash_4ndis_dot_dat")) {
    substitute4Dash4Ndis();
    return true;
  } else if (name == QString("_8_dash_8sphe_dot_dat")) {
    substitute8Dash8Sphe();
    return true;
  } else
    return false;
}

void POVRayExporter::substituteStud()
{
  stream_ << "union {\n";
  stream_ << "\tcylinder { <0, 0, 0>, <0, -4, 0>, 6 }\n";
  stream_ << "\tobject { L3Logo translate <0, -4, 0> }\n";
  stream_ << "}\n";
}

void POVRayExporter::substituteStud2()
{
  stream_ << "difference {\n";
  stream_ << "\tcylinder { <0, 0, 0>, <0, -4, 0>, 6 }\n";
  stream_ << "\tcylinder { <0, 0.01 ,0>, <0, -4.01, 0>, 4 }\n";
  stream_ << "}\n";
}

void POVRayExporter::substituteStud3()
{
  stream_ << "cylinder {\n";
  stream_ << "\t<0, 0, 0>, <0, -4, 0>, 4\n";
  stream_ << "}\n";
}

void POVRayExporter::substituteStud4()
{
  stream_ << "difference {\n";
  stream_ << "\tcylinder { <0, 0, 0>, <0, -4, 0>, 8 }\n";
  stream_ << "\tcylinder { <0, 0.01 ,0>, <0, -4.01, 0>, 6 }\n";
  stream_ << "}\n";	
}

void POVRayExporter::substitutePeghole()
{
  stream_ << "union {\n";
  stream_ << "\tcylinder { <0, 0, 0>, <0, 2, 0>, 8 open }\n";
  stream_ << "\tdisc { <0, 2, 0>, <0, 1, 0>, 8, 6 }\n";
  stream_ << "}\n";
}

void POVRayExporter::substitute2Dash4Cyli()
{
  stream_ << "cylinder {\n";
  stream_ << "\t<0, 0, 0>, <0, 1, 0>, 1 open\n";
  stream_ << "\tclipped_by {\n";
  stream_ << "\t\tbox { <-2, -1, 0>, <2, 2, 2> }\n";
  stream_ << "\t}\n";
  stream_ << "}\n";
}

void POVRayExporter::substitute2Dash4Ndis()
{
  stream_ << "disc {\n";
  stream_ << "\t<0, 0, 0>, <0, 1, 0>, 2, 1\n";
  stream_ << "\tclipped_by {\n";
  stream_ << "\t\tbox { <-1, -1, 0>, <1, 1, 1> }\n";
  stream_ << "\t}\n";
  stream_ << "}\n";
}

void POVRayExporter::substitute2Dash4Disc()
{
  stream_ << "disc {\n";
  stream_ << "\t<0, 0, 0>, <0, 1, 0>, 1\n";
  stream_ << "\tclipped_by {\n";
  stream_ << "\t\tbox { <-2, -1, 0>, <2, 1, 2> }\n";
  stream_ << "\t}\n";
  stream_ << "}\n";
}

void POVRayExporter::substitute4Dash4Cyli()
{
  stream_ << "cylinder { <0, 0, 0>, <0, 1, 0>, 1 open }\n";
}

void POVRayExporter::substitute4Dash4Ndis()
{
  stream_ << "disc {\n";
  stream_ << "\t<0, 0, 0>, <0, 1, 0>, 2, 1\n";
  stream_ << "\tclipped_by {\n";
  stream_ << "\t\tbox { <-1, -1, -1>, <1, 1, 1> }\n";
  stream_ << "\t}\n";
  stream_ << "}\n";
}

void POVRayExporter::substitute8Dash8Sphe()
{
  stream_ << "sphere { <0, 0, 0>, 1 }\n";
}

bool POVRayExporter::colorAmbiguityTest(const ldraw::model *m)
{
  for (ldraw::model::const_iterator it = m->elements().begin(); it != m->elements().end(); ++it) {
    if ((*it)->capabilities() & ldraw::capability_color) {
      ldraw::element_colored_base *elem = dynamic_cast<ldraw::element_colored_base *>(*it);
      if (elem->get_color().get_id() == 16 || elem->get_color().get_id() == 24)
        return true;
    }
  }
  
  return false;
}

}
