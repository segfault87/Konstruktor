/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2013 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#ifdef _MSC_VER

/* platform-specific directory I/O for MSVC compiler */

#include <Windows.h>

#include <algorithm>
#include <iostream>

#include "part_library.h"
#include "utils.h"

namespace ldraw
{

#define FILE_EXISTS(attr) (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY))

int dirsep(int c)
{
  if (c == '/')
    return '\\';
  return c;
}

bool part_library::read_fs(const std::string &path)
{
  // TODO recursive subdirectory handling
  std::string dn1, dn2;
  
  // 1. find subdirectories  
  m_ldrawpath = path;
  std::transform(path.begin(), path.end(), m_ldrawpath.begin(), dirsep);
  m_primdir = "p";
  m_partsdir = "parts";

  DWORD primAttrib = GetFileAttributes(ldrawpath(path_type::ldraw_primitives_path).c_str());
  DWORD partsAttrib = GetFileAttributes(ldrawpath(path_type::ldraw_parts_path).c_str());
  
  if (!FILE_EXISTS(primAttrib) || !FILE_EXISTS(partsAttrib)) {
    std::cerr << "[libLDR] No p/ or parts/ found." << std::endl;
    return false;
  }

  WIN32_FIND_DATA ffd;
  HANDLE hFind = INVALID_HANDLE_VALUE;

  // 2. look into p/ directory
  hFind = FindFirstFile((ldrawpath(path_type::ldraw_primitives_path) + "\\*.dat").c_str(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    std::cerr << "[libLDR] Couldn't open p/." << std::endl;
    return false;
  }
  do {
    dn1 = ffd.cFileName;
    dn2 = utils::translate_string(dn1);

    std::cerr << dn2 << std::endl;
    
    m_primlist[dn2] = dn1;
  } while (FindNextFile(hFind, &ffd) != 0);
  FindClose(hFind);
  
  // 3. look into p/48 directory
  hFind = FindFirstFile((ldrawpath(path_type::ldraw_primitives_path) + DIRECTORY_SEPARATOR + "48" + "\\*.dat").c_str(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    std::cerr << "[libLDR] Couldn't open p/48/." << std::endl;
    return false;
  }
  do {
    dn1 = std::string("48/") + ffd.cFileName;
    dn2 = utils::translate_string(dn1);
    
    m_primlist[dn2] = dn1;
  } while (FindNextFile(hFind, &ffd) != 0);
  FindClose(hFind);
  
  // 4. look into parts/ directory
  hFind = FindFirstFile((ldrawpath(path_type::ldraw_parts_path) + "\\*.dat").c_str(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    std::cerr << "[libLDR] Couldn't open parts/." << std::endl;
    return false;
  }
  do {
    dn1 = ffd.cFileName;
    dn2 = utils::translate_string(dn1);
    
    m_partlist[dn2] = dn1;
  } while (FindNextFile(hFind, &ffd) != 0);
  FindClose(hFind);
  
  // 5. look into parts/s directory
  hFind = FindFirstFile((ldrawpath(path_type::ldraw_parts_path) + DIRECTORY_SEPARATOR + "s" + "\\*.dat").c_str(), &ffd);
  if (hFind == INVALID_HANDLE_VALUE) {
    std::cerr << "[libLDR] Couldn't open parts/s/." << std::endl;
    return false;
  }
  do {
    dn1 = std::string("s/") + ffd.cFileName;
    dn2 = utils::translate_string(dn1);
    
    m_partlist[dn2] = dn1;
  } while (FindNextFile(hFind, &ffd) != 0);
  FindClose(hFind);
  
  return true;
}

}

#endif
