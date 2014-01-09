/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2013 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifndef _MSC_VER

/* platform-specific directory I/O for POSIX compatible systems */

#include <dirent.h>

#include <iostream>

#include "part_library.h"
#include "utils.h"

namespace ldraw
{

bool part_library::read_fs(const std::string &path)
{
  // TODO recursive subdirectory handling
  DIR *de;
  struct dirent *ep;
  std::string dn1, dn2;
  std::string pdir, partsdir;
  
  // 1. find subdirectories
  if ((de = opendir(path.c_str())) == 0L)
    return false;
  
  while ((ep = readdir(de))) {
    dn1 = ep->d_name;
    dn2 = utils::translate_string(dn1);
    
    if (dn2 == "p") pdir = dn1;
    else if (dn2 == "parts") partsdir = dn1;
  }
  closedir(de);
  
  if (pdir.empty() || partsdir.empty()) {
    std::cerr << "[libLDR] No p/ or parts/ found." << std::endl;
    return false;
  }
  
  m_ldrawpath = path;
  m_primdir = pdir;
  m_partsdir = partsdir;
  
  // 2. look into p/ directory
  if ((de = opendir((m_ldrawpath + DIRECTORY_SEPARATOR + pdir).c_str())) == 0L) {
    std::cerr << "[libLDR] Couldn't open p/." << std::endl;
    return false;
  }
  while ((ep = readdir(de))) {
    dn1 = ep->d_name;
    dn2 = utils::translate_string(dn1);
    
    if (dn2.length() > 4 && dn2.substr(dn1.length()-4, 4) == ".dat")
      m_primlist[dn2] = dn1;
  }
  closedir(de);
  
  // 3. look into p/48 directory
  if ((de = opendir((m_ldrawpath + DIRECTORY_SEPARATOR + pdir + DIRECTORY_SEPARATOR + "48").c_str())) == 0L)
    std::cerr << "[libLDR] Couldn't open p/48/." << std::endl;
  else {
    while((ep = readdir(de))) {
      dn1 = std::string("48") + DIRECTORY_SEPARATOR + ep->d_name;
      dn2 = utils::translate_string(dn1);
      
      if (dn2.length() > 4 && dn2.substr(dn2.length()-4, 4) == ".dat")
        m_primlist[dn2] = dn1;
    }
    closedir(de);
  }
  
  // 4. look into parts/ directory
  if ((de = opendir((m_ldrawpath + DIRECTORY_SEPARATOR + m_partsdir).c_str())) == 0L) {
    std::cerr << "[libLDR] Couldn't open parts/." << std::endl;
    return false;
  }
  while((ep = readdir(de))) {
    dn1 = ep->d_name;
    dn2 = utils::translate_string(dn1);
    
    if (dn2.length() > 4 && dn2.substr(dn2.length() - 4, 4) == ".dat")
      m_partlist[dn2] = dn1;
  }
  closedir(de);
  
  // 5. look into parts/s directory
  std::string s;
  de = opendir((m_ldrawpath + DIRECTORY_SEPARATOR + m_partsdir + DIRECTORY_SEPARATOR + "s").c_str());
  s = "s";
  
  if(!de) {
    de = opendir((m_ldrawpath + DIRECTORY_SEPARATOR + m_partsdir + DIRECTORY_SEPARATOR + "S").c_str());
    s = "S";
  }
  
  if(!de)
    std::cerr << "[libLDR] Couldn't open parts/s/." << std::endl;
  else {
    while((ep = readdir(de))) {
      dn1 = s + DIRECTORY_SEPARATOR + ep->d_name;
      dn2 = utils::translate_string(dn1);
      
      if (dn2.length() > 4 && dn2.substr(dn2.length() - 4, 4) == ".dat")
        m_partlist[dn2] = dn1;
    }
    closedir(de);
  }
  
  return true;
}

}

#endif
