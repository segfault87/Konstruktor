/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <fstream>
#include <iostream>
#include <sstream>

#include <algorithm>
#include <set>

#include "bfc.h"
#include "elements.h"
#include "model.h"
#include "utils.h"

#include "reader.h"

namespace ldraw
{

reader::reader()
{
}

reader::reader(const std::string &basepath)
{
  m_basepath = basepath;
  
  if (!basepath.empty() && *(m_basepath.rbegin()) != '/')
    m_basepath += "/";
}

model_multipart* reader::load_from_file(const std::string &name) const
{
  std::ifstream file;
  
  file.open((m_basepath + name).c_str(), std::ios::in);
  if (!file.is_open())
    throw exception(__func__, exception::user_error, std::string("Could not open file for reading: ") + name);
  
  model_multipart *model = load_from_stream(file, name);
  
  file.close();
  
  return model;
}

model_multipart* reader::load_from_stream(std::istream &stream, std::string name)
{
  model_multipart *nm = new model_multipart;
  
  // Search for submodel names
  std::set<std::string> m_submodel_names;
  std::string line;
  while (!stream.eof()) {
    getline(stream, line);
    line = utils::trim_string(line);
    if (line.length() > 7 && line.substr(0, 6) == "0 FILE") {
      std::string filename = utils::translate_string(line.substr(7, line.length() - 7));
      m_submodel_names.insert(filename);
    }
  }

  std::string keyname;
  
  // Rewind the stream to the beginning
  stream.clear();
  stream.seekg(0, std::ios::beg);
  
  bool tmp = parse_stream(nm->main_model(), stream, true, &keyname);
  if (nm->main_model()->name().empty()) {
    std::string nfp;
    size_t o = name.find_last_of("/");
    if (o == std::string::npos)
      nfp = name;
    else
      nfp = name.substr(o + 1, name.length() - o);
    
    nm->main_model()->set_name(nfp);
  }
  if (!tmp)
    return nm;
  
  bool loop = true;
  while (loop) {
    std::string fn;
    model *m = new model;
    
    m->set_parent(nm);
    m->set_modeltype(model::submodel);
    loop = parse_stream(m, stream, true, &fn);
    m->set_name(keyname);
    
    nm->insert_submodel(m, keyname);

    keyname = fn;
  }
  
  nm->link_submodels();
  
  if (utils::cyclic_reference_test(nm->main_model()))
    throw exception(__func__, exception::fatal, "Cyclic reference detected. This model file may be corrupted.");
  for (model_multipart::submodel_const_iterator it = nm->submodel_list().begin(); it != nm->submodel_list().end(); ++it) {
    if (utils::cyclic_reference_test((*it).second))
      throw exception(__func__, exception::fatal, "Cyclic reference detected. This model file may be corrupted.");
  }
  
  return nm;
}

bool reader::parse_stream(model *m, std::istream &stream, bool multipart, std::string *keyname)
{
  std::string line;
  int lines = 0;
  int zerocnt = 0;
  bool founddesc = false;
  bool foundheader;
  
  while(!stream.eof()) {
    getline(stream, line);
    line = utils::trim_string(line);
    long llen = line.length();
    ++lines;
    if (llen == 0)
      continue;
    // Returns true when more subpart(s) available. returns false otherwise.
    if (multipart && llen > 7 && line.substr(0, 6) == "0 FILE") {
      if (keyname)
        *keyname = line.substr(7, llen - 7);
      if (lines != 1) {
        stream.seekg((long)stream.tellg() - llen - 1);
        return true;
      }
    }
    
    foundheader = false;
    
    if (line[0] == '0') {
      // parse line type 0
      foundheader = true;
      ++zerocnt;
      
      std::string cont = utils::trim_string(line.substr(1, line.length()-1));
      std::string contlc = utils::translate_string(cont);
      
      // Parse header data
      if (contlc.length() > 4 && contlc.substr(0, 4) == "file"); // Skip
      else if (contlc.length() > 6 && contlc.substr(0, 5) == "name:") { // Filename
        m->set_name(cont.substr(6, cont.length() - 6));
      } else if (contlc.length() > 5 && contlc.substr(0, 4) == "name") { // Filename without ':'
        m->set_name(cont.substr(5, cont.length() - 5));
      } else if (contlc.length() > 8 && contlc.substr(0, 7) == "author:") { // Author
        m->set_author(cont.substr(8, cont.length() - 8));
      } else if (contlc.length() > 7 && contlc.substr(0, 6) == "author") { // Author without ':'
        m->set_author(cont.substr(7, cont.length() - 7));
      } else if (zerocnt < 3 && !founddesc) { // Partname
        m->set_desc(cont);
        founddesc = true;
      } else {
        foundheader = false;
      }
    }
    
    if (!foundheader) {
      element_base *el = parse_line(line, m);
      if (el)
        m->insert_element(el);
    }
  }
  
  return false;
}

element_base* reader::parse_line(const std::string &command, model *m)
{
  std::string line = utils::trim_string(command);
  
  if (line.length() == 0)
    return 0;
  if (line[0] == '0') {
    // parse line type 0
    
    std::string cont = utils::trim_string(line.substr(1, line.length()-1));
    std::string contlc = utils::translate_string(cont);
    if (cont.length() == 0)
      return 0;
    
    if (cont[0] == '!') {
      // header data
      size_t pos = cont.find_first_of(" ");
      if (m && pos != std::string::npos)
        m->set_header(cont.substr(1, pos - 1), cont.substr(pos + 1));
    } else if (contlc == "step") {
      return new element_state(element_state::state_step);
    } else if (contlc == "pause") {
      return new element_state(element_state::state_pause);
    } else if (contlc == "clear") {
      return new element_state(element_state::state_clear);
    } else if (contlc == "save") {
      return new element_state(element_state::state_save);
    } else if (contlc.length() > 6 && (contlc.substr(0, 5) == "print" || contlc.substr(0, 5) == "write")) {
      return new element_print(cont.substr(6, line.length()-6));
    } else if (contlc.length() > 3 && contlc.substr(0, 3) == "bfc") {
      // Handle BFC statements
      std::string subs = contlc.substr(4, line.length() - 4);
      int cert = -1, winding = -1;
      
      if (subs == "ccw")
        return new element_bfc(element_bfc::ccw);
      else if (subs == "cw")
        return new element_bfc(element_bfc::cw);
      else if (subs == "clip")
        return new element_bfc(element_bfc::clip);
      else if (subs == "clip cw" || subs == "cw clip")
        return new element_bfc(element_bfc::clip_cw);
      else if (subs == "clip ccw" || subs == "ccw clip")
        return new element_bfc(element_bfc::clip_ccw);
      else if (subs == "noclip")
        return new element_bfc(element_bfc::noclip);
      else if (subs == "invertnext")
        return new element_bfc(element_bfc::invertnext);
      else if (subs == "certify" || subs == "certify ccw")
        cert = bfc_certification::certified, winding = bfc_certification::ccw;
      else if (subs == "certify cw")
        cert = bfc_certification::certified, winding = bfc_certification::cw;
      
      else if (subs == "nocertify")
        cert = bfc_certification::uncertified;
      
      if (m && cert != -1) {
        bfc_certification *c = m->init_custom_data<bfc_certification>();
        c->set_certification((bfc_certification::cert_status)cert);
        if (winding != -1)
          c->set_orientation((bfc_certification::winding)winding);
      }
      
      return 0L;
    } else if (contlc.length() > 0) {
      return new element_comment(cont);
    }
  } else if (line[0] == '1') {
    // File reference
    std::istringstream s(line.substr(2, line.length()-2));
    std::string col;
    float x, y, z, a, b, c, d, e, f, g, h, i;
    char fnbuf[255];
    
    s >> col >> x >> y >> z >> a >> b >> c >> d >> e >> f >> g >> h >> i;
    s.getline(fnbuf, 255);
    
    std::string fn = utils::trim_string(std::string(fnbuf));

    return new element_ref(color(std::stoi(col, nullptr, 0)), matrix(a, b, c, d, e, f, g, h, i, x, y, z), fn);
  } else if (line[0] == '2') {
    // Line
    std::istringstream s(line.substr(2, line.length()-2));
    int col;
    float x1, y1, z1, x2, y2, z2;
    
    s >> col >> x1 >> y1 >> z1 >> x2 >> y2 >> z2;
    
    return new element_line(color(col), vector(x1, y1, z1), vector(x2, y2, z2));
  } else if (line[0] == '3') {
    // Triangle
    std::istringstream s(line.substr(2, line.length()-2));
    int col;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3;
    
    s >> col >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3;
    
    return new element_triangle(color(col), vector(x1, y1, z1), vector(x2, y2, z2), vector(x3, y3, z3));
  } else if (line[0] == '4') {
    // Quadrilateral
    std::istringstream s(line.substr(2, line.length()-2));
    int col;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    
    s >> col >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3 >> x4 >> y4 >> z4;
    
    return new element_quadrilateral(color(col), vector(x1, y1, z1), vector(x2, y2, z2), vector(x3, y3, z3), vector(x4, y4, z4));
  } else if (line[0] == '5') {
    // Conditional line
    std::istringstream s(line.substr(2, line.length()-2));
    int col;
    float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    
    s >> col >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> x3 >> y3 >> z3 >> x4 >> y4 >> z4;
    
    return new element_condline(color(col), vector(x1, y1, z1), vector(x2, y2, z2), vector(x3, y3, z3), vector(x4, y4, z4));
  }
  
  return 0L;
}

}
