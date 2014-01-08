/* libLDR: Portable and easy-to-use LDraw format abstraction & I/O reference library *
 * To obtain more information about LDraw, visit http://www.ldraw.org.               *
 * Distributed in terms of the GNU Lesser General Public License v3                  *
 *                                                                                   *
 * Author: (c)2006-2008 Park "segfault" J. K. <mastermind_at_planetmono_dot_org>     */

#include <iostream>
#include <sstream>
#include <map>

#include "color.h"

namespace ldraw {

typedef std::map<int, const color_entity *> color_map_type;

const color_map_type color::color_map = color_map_type();
bool color::m_initialized = false;

// Predefined color table
const material_traits_speckle color::material_chart_speckle[] = {
  { {174, 122,  89}, 0.4f, 1, 3 },
  { { 89,  89,  89}, 0.4f, 1, 3 },
  { { 89,  89,  89}, 0.4f, 1, 3 }
};

const material_traits_glitter color::material_chart_glitter[] = {
  { {146,  57, 120}, 0.17f, 0.2, 1 },
  { {255, 255, 255}, 0.08f, 0.1, 1 },
  { {140,   0, 255}, 0.3f , 0.4, 1 }
};

const color_entity color::color_chart[] = {
  {      material_normal, { 33,  33,  33, 255}, { 89,  89,  89, 255},  0,   0,                           "Black", 0L },
  {      material_normal, {  0,  51, 178, 255}, { 51,  51,  51, 255},  0,   1,                            "Blue", 0L },
  {      material_normal, {  0, 140,  20, 255}, { 51,  51,  51, 255},  0,   2,                           "Green", 0L },
  {      material_normal, {  0, 153, 159, 255}, { 51,  51,  51, 255},  0,   3,                  "Dark Turquoise", 0L },
  {      material_normal, {196,   0,  38, 255}, { 51,  51,  51, 255},  0,   4,                             "Red", 0L },
  {      material_normal, {223, 102, 149, 255}, { 51,  51,  51, 255},  0,   5,                       "Dark Pink", 0L },
  {      material_normal, { 92,  32,   0, 255}, { 30,  30,  30, 255},  0,   6,                           "Brown", 0L },
  {      material_normal, {156, 153, 153, 255}, { 51,  51,  51, 255},  0,   7,                      "Light Gray", 0L },
  {      material_normal, { 99,  95,  82, 255}, { 51,  51,  51, 255},  0,   8,                       "Dark Gray", 0L },
  {      material_normal, {107, 171, 220, 255}, { 51,  51,  51, 255},  0,   9,                      "Light Blue", 0L },
  {      material_normal, {107, 238, 144, 255}, { 51,  51,  51, 255},  0,  10,                    "Bright Green", 0L },
  {      material_normal, { 51, 166, 167, 255}, { 51,  51,  51, 255},  0,  11,                 "Light Turquoise", 0L },
  {      material_normal, {255, 133, 122, 255}, { 51,  51,  51, 255},  0,  12,                          "Salmon", 0L },
  {      material_normal, {249, 164, 198, 255}, { 51,  51,  51, 255},  0,  13,                            "Pink", 0L },
  {      material_normal, {255, 220,   0, 255}, { 51,  51,  51, 255},  0,  14,                          "Yellow", 0L },
  {      material_normal, {255, 255, 255, 255}, { 51,  51,  51, 255},  0,  15,                           "White", 0L },
  {      material_normal, {186, 255, 206, 255}, { 51,  51,  51, 255},  0,  17,                     "Light Green", 0L },
  {      material_normal, {253, 232, 150, 255}, { 51,  51,  51, 255},  0,  18,                    "Light Yellow", 0L },
  {      material_normal, {232, 207, 161, 255}, { 51,  51,  51, 255},  0,  19,                             "Tan", 0L },
  {      material_normal, {215, 196, 230, 255}, { 51,  51,  51, 255},  0,  20,                    "Light Violet", 0L },
  {      material_normal, {129,   0, 123, 255}, { 51,  51,  51, 255},  0,  22,                          "Purple", 0L },
  {      material_normal, { 71,  50, 176, 255}, { 30,  30,  30, 255},  0,  23,                "Dark Blue Violet", 0L },
  {      material_normal, {249,  96,   0, 255}, { 51,  51,  51, 255},  0,  25,                          "Orange", 0L },
  {      material_normal, {216,  27, 109, 255}, { 51,  51,  51, 255},  0,  26,                         "Magenta", 0L },
  {      material_normal, {215, 240,   0, 255}, { 51,  51,  51, 255},  0,  27,                            "Lime", 0L },
  {      material_normal, {197, 151,  80, 255}, { 51,  51,  51, 255},  0,  28,                        "Dark Tan", 0L },
  {      material_normal, {228, 173, 200, 255}, { 51,  51,  51, 255},  0,  29,                     "Bright Pink", 0L },
  {      material_normal, {243, 207, 155, 255}, { 51,  51,  51, 255},  0,  68,               "Very Light Orange", 0L },
  {      material_normal, {205,  98, 152, 255}, { 51,  51,  51, 255},  0,  69,                    "Light Purple", 0L },
  {      material_normal, {105,  64,  39, 255}, { 51,  51,  51, 255},  0,  70,                   "Reddish Brown", 0L },
  {      material_normal, {163, 162, 164, 255}, { 51,  51,  51, 255},  0,  71,               "Light Bluish Gray", 0L },
  {      material_normal, { 99,  95,  97, 255}, { 51,  51,  51, 255},  0,  72,                "Dark Bluish Gray", 0L },
  {      material_normal, {110, 153, 201, 255}, { 51,  51,  51, 255},  0,  73,                     "Medium Blue", 0L },
  {      material_normal, {161, 196, 139, 255}, { 51,  51,  51, 255},  0,  74,                    "Medium Green", 0L },
  {      material_normal, {254, 204, 204, 255}, { 51,  51,  51, 255},  0,  77,                      "Light Pink", 0L },
  {      material_normal, {250, 215, 195, 255}, { 51,  51,  51, 255},  0,  78,                     "Light Flesh", 0L },
  {      material_normal, { 52,  43, 117, 255}, { 30,  30,  30, 255},  0,  85,                     "Dark Purple", 0L },
  {      material_normal, {124,  92,  69, 255}, { 51,  51,  51, 255},  0,  86,                      "Dark Flesh", 0L },
  {      material_normal, {108, 129, 183, 255}, { 51,  51,  51, 255},  0,  89,                     "Blue Violet", 0L },
  {      material_normal, {204, 142, 104, 255}, { 51,  51,  51, 255},  0,  92,                           "Flesh", 0L },
  {      material_normal, {238, 196, 182, 255}, { 51,  51,  51, 255},  0, 100,                    "Light Salmon", 0L },
  {      material_normal, { 67,  84, 147, 255}, { 51,  51,  51, 255},  0, 110,                          "Violet", 0L },
  {      material_normal, {104, 116, 172, 255}, { 51,  51,  51, 255},  0, 112,                   "Medium Violet", 0L },
  {      material_normal, {199, 210,  60, 255}, { 51,  51,  51, 255},  0, 115,                     "Medium Lime", 0L },
  {      material_normal, {183, 215, 213, 255}, { 51,  51,  51, 255},  0, 118,                            "Aqua", 0L },
  {      material_normal, {217, 228, 167, 255}, { 51,  51,  51, 255},  0, 120,                      "Light Lime", 0L },
  {      material_normal, {234, 184, 145, 255}, { 51,  51,  51, 255},  0, 125,                    "Light Orange", 0L },
  {      material_normal, {229, 228, 222, 255}, { 51,  51,  51, 255},  0, 151,          "Very Light Bluish Gray", 0L },
  {      material_normal, {232, 171,  45, 255}, { 51,  51,  51, 255},  0, 191,             "Bright Light Orange", 0L },
  {      material_normal, {159, 195, 233, 255}, { 51,  51,  51, 255},  0, 212,               "Bright Light Blue", 0L },
  {      material_normal, {143,  76,  42, 255}, { 51,  51,  51, 255},  0, 216,                            "Rust", 0L },
  {      material_normal, {253, 234, 140, 255}, { 51,  51,  51, 255},  0, 226,             "Bright Light Yellow", 0L },
  {      material_normal, {125, 187, 221, 255}, { 51,  51,  51, 255},  0, 232,                        "Sky Blue", 0L },
  {      material_normal, {  0,  29, 104, 255}, { 30,  30,  30, 255},  0, 272,                       "Dark Blue", 0L },
  {      material_normal, { 39,  70,  44, 255}, { 51,  51,  51, 255},  0, 288,                      "Dark Green", 0L },
  {      material_normal, { 53,  33,   0, 255}, {  0,   0,   0, 255},  0, 308,                      "Dark Brown", 0L },
  {      material_normal, { 53, 162, 189, 255}, { 51,  51,  51, 255},  0, 313,                     "Maersk Blue", 0L },
  {      material_normal, {120,   0,  28, 255}, { 51,  51,  51, 255},  0, 320,                        "Dark Red", 0L },
  {      material_normal, {191, 135, 130, 255}, { 51,  51,  51, 255},  0, 335,                        "Sand Red", 0L },
  {      material_normal, {247, 133, 177, 255}, { 51,  51,  51, 255},  0, 351,                "Medium Dark Pink", 0L },
  {      material_normal, {209, 131,   4, 255}, { 51,  51,  51, 255},  0, 366,                    "Earth Orange", 0L },
  {      material_normal, {132,  94, 132, 255}, { 51,  51,  51, 255},  0, 373,                     "Sand Purple", 0L },
  {      material_normal, {160, 188, 172, 255}, { 51,  51,  51, 255},  0, 378,                      "Sand Green", 0L },
  {      material_normal, {106, 122, 150, 255}, { 51,  51,  51, 255},  0, 379,                       "Sand Blue", 0L },
  {      material_normal, {180, 121,  86, 255}, { 51,  51,  51, 255},  0, 450,                  "Fabuland Brown", 0L },
  {      material_normal, {254, 159,   6, 255}, { 51,  51,  51, 255},  0, 462,                   "Medium Orange", 0L },
  {      material_normal, {179,  62,   0, 255}, { 51,  51,  51, 255},  0, 484,                     "Dark Orange", 0L },
  {      material_normal, {230, 227, 218, 255}, { 51,  51,  51, 255},  0, 503,                 "Very Light Gray", 0L },
  { material_transparent, {255, 255, 255, 128}, {195, 195, 195, 255},  0,  47,                     "Trans Clear", 0L },
  { material_transparent, { 99,  95,  82, 128}, { 23,  19,  22, 255},  0,  40,                     "Trans Black", 0L },
  { material_transparent, {196,   0,  38, 128}, {136,   0,   0, 255},  0,  36,                       "Trans Red", 0L },
  { material_transparent, {249,  96,   0, 128}, {189,  36,   0, 255},  0,  38,               "Trans Neon Orange", 0L },
  { material_transparent, {224, 152, 100, 128}, {164,  92,  40, 255},  0,  57,                    "Trans Orange", 0L },
  { material_transparent, {255, 246, 123, 128}, {195, 186,  63, 255},  0,  54,               "Trans Neon Yellow", 0L },
  { material_transparent, {202, 176,   0, 128}, {142, 116,   0, 255},  0,  46,                    "Trans Yellow", 0L },
  { material_transparent, {192, 255,   0, 128}, {132, 195,   0, 255},  0,  42,                "Trans Neon Green", 0L },
  { material_transparent, {217, 228, 167, 128}, {157, 168, 107, 255},  0,  35,              "Trans Bright Green", 0L },
  { material_transparent, {  6, 100,  50, 128}, {  0,  40,   0, 255},  0,  34,                     "Trans Green", 0L },
  { material_transparent, {  0,  32, 160, 128}, {  0,   0, 100, 255},  0,  33,                 "Trans Dark Blue", 0L },
  { material_transparent, { 85, 165, 175, 128}, { 25, 105, 115, 255},  0,  41,               "Trans Medium Blue", 0L },
  { material_transparent, {174, 239, 236, 128}, {114, 179, 176, 255},  0,  43,                "Trans Light Blue", 0L },
  { material_transparent, {193, 223, 240, 128}, {133, 163, 180, 255},  0,  39,           "Trans Very Light Blue", 0L },
  { material_transparent, {150, 112, 159, 128}, { 90,  52,  99, 255},  0,  44,              "Trans Light Purple", 0L },
  { material_transparent, {100,   0,  97, 128}, { 40,   0,  37, 255},  0,  52,                    "Trans Purple", 0L },
  { material_transparent, {223, 102, 149, 128}, {163,  42,  89, 255},  0,  37,                 "Trans Dark Pink", 0L },
  { material_transparent, {228, 173, 200, 128}, {168, 113, 140, 255},  0,  45,                      "Trans Pink", 0L },
  {      material_chrome, {187, 174,  61, 255}, {187, 178,  61, 255},  0, 334,                     "Chrome Gold", 0L },
  {      material_chrome, {224, 224, 224, 255}, {164, 164, 164, 255},  0, 383,                   "Chrome Silver", 0L },
  {      material_chrome, {100,  90,  76, 255}, { 40,  30,  16, 255},  0,  60,            "Chrome Antique Brass", 0L },
  {      material_chrome, { 27,  42,  52, 255}, {  0,   0,   0, 255},  0,  64,                    "Chrome Black", 0L },
  {      material_chrome, { 92, 102, 164, 255}, { 32,  42, 104, 255},  0,  61,                     "Chrome Blue", 0L },
  {      material_chrome, { 60, 179, 113, 255}, {  0, 119,  53, 255},  0,  62,                    "Chrome Green", 0L },
  {      material_chrome, {170,  77, 142, 255}, {110,  17,  82, 255},  0,  63,                     "Chrome Pink", 0L },
  { material_pearlescent, {242, 243, 242, 255}, { 51,  51,  51, 255},  0, 183,                     "Pearl White", 0L },
  { material_pearlescent, {171, 173, 172, 255}, { 51,  51,  51, 255},  0, 150,           "Pearl Very Light Grey", 0L },
  { material_pearlescent, {156, 163, 168, 255}, { 51,  51,  51, 255},  0, 135,                "Pearl Light Gray", 0L },
  { material_pearlescent, {137, 135, 136, 255}, { 51,  51,  51, 255},  0, 179,                     "Flat Silver", 0L },
  { material_pearlescent, { 87,  88,  87, 255}, { 51,  51,  51, 255},  0, 148,                 "Pearl Dark Gray", 0L },
  { material_pearlescent, {121, 136, 161, 255}, { 51,  51,  51, 255},  0, 137,                      "Metal Blue", 0L },
  { material_pearlescent, {220, 188, 129, 255}, { 51,  51,  51, 255},  0, 142,                "Pearl Light Gold", 0L },
  { material_pearlescent, {170, 127,  46, 255}, { 51,  51,  51, 255},  0, 297,                      "Pearl Gold", 0L },
  { material_pearlescent, {180, 132,  85, 255}, { 51,  51,  51, 255},  0, 178,                  "Flat Dark Gold", 0L },
  { material_pearlescent, {174, 122,  89, 255}, { 51,  51,  51, 255},  0, 134,                          "Copper", 0L },
  {    material_metallic, {171, 175, 174, 255}, { 51,  51,  51, 255},  0,  80,                 "Metallic Silver", 0L },
  {    material_metallic, {137, 152,  95, 255}, { 51,  51,  51, 255},  0,  81,                  "Metallic Green", 0L },
  {    material_metallic, {203, 169,  52, 255}, { 51,  51,  51, 255},  0,  82,                   "Metallic Gold", 0L },
  { material_transparent, {255, 255, 255, 224}, {195, 195, 195, 255},  0,  79,                     "Milky White", 0L },
  {    material_luminant, {224, 255, 176, 250}, {164, 195, 116, 255}, 15,  21,             "Glow In Dark Opaque", 0L },
  {    material_luminant, {189, 198, 173, 250}, {129, 138, 113, 255}, 15, 294,              "Glow In Dark Trans", 0L },
  {     material_glitter, {223, 102, 149, 128}, {154,  42, 102, 255},  0, 114,         "Glitter Trans Dark Pink", &material_chart_glitter[0] },
  {     material_glitter, {255, 255, 255, 128}, {195, 195, 195, 255},  0, 117,             "Glitter Trans Clear", &material_chart_glitter[1] },
  {     material_glitter, {100,   0,  97, 128}, { 40,   0,  37, 255},  0, 129,            "Glitter Trans Purple", &material_chart_glitter[2] },
  {     material_speckle, {  0,   0,   0, 255}, { 89,  89,  89, 255},  0, 132,            "Speckle Black Silver", &material_chart_speckle[0] },
  {     material_speckle, {  0,   0,   0, 255}, { 89,  89,  89, 255},  0,  75,            "Speckle Black Copper", &material_chart_speckle[1] },
  {     material_speckle, { 99,  95,  97, 255}, { 89,  89,  89, 255},  0,  76, "Speckle Dark Bluish Gray Silver", &material_chart_speckle[2] },
  {      material_rubber, {245, 205,  47, 255}, { 51,  51,  51, 255},  0,  65,                   "Rubber Yellow", 0L },
  {      material_rubber, {202, 176,   0, 128}, {142, 116,   0, 255},  0,  66,             "Rubber Trans Yellow", 0L },
  {      material_rubber, {255, 255, 255, 128}, {195, 195, 195, 255},  0,  67,              "Rubber Trans Clear", 0L },
  {      material_rubber, { 33,  33,  33, 255}, { 89,  89,  89, 255},  0, 256,                    "Rubber Black", 0L },
  {      material_rubber, {  0,  51, 178, 255}, { 51,  51,  51, 255},  0, 273,                     "Rubber Blue", 0L },
  {      material_rubber, {196,   0,  38, 255}, { 51,  51,  51, 255},  0, 324,                      "Rubber Red", 0L },
  {      material_rubber, {193, 194, 193, 255}, { 51,  51,  51, 255},  0, 375,               "Rubber Light Gray", 0L },
  {      material_rubber, {  0,  29, 104, 255}, { 51,  51,  51, 255},  0, 406,                "Rubber Dark Blue", 0L },
  {      material_rubber, {129,   0, 123, 255}, { 51,  51,  51, 255},  0, 449,                   "Rubber Purple", 0L },
  {      material_rubber, {215, 240,   0, 255}, { 51,  51,  51, 255},  0, 490,                     "Rubber Lime", 0L },
  {      material_rubber, {163, 162, 164, 255}, { 51,  51,  51, 255},  0, 496,        "Rubber Light Bluish Gray", 0L },
  {      material_rubber, {137, 135, 136, 255}, { 51,  51,  51, 255},  0, 504,              "Rubber Flat Silver", 0L },
  {      material_rubber, {255, 255, 255, 255}, { 51,  51,  51, 255},  0, 511,                    "Rubber White", 0L },
  {      material_normal, {127, 127, 127, 255}, { 51,  51,  51, 255},  0,  16,                      "Main Color", 0L },
  {      material_normal, {127, 127, 127, 255}, { 51,  51,  51, 255},  0,  24,                      "Edge Color", 0L },
  { material_transparent, {  0,   0,   0, 220}, {  5,  19,  29, 255},  0,  32,             "Trans Black IR Lens", 0L },
  {    material_metallic, {208, 208, 208, 255}, {110, 110, 110, 255},  0, 494,          "Electric Contact Alloy", 0L },
  {    material_metallic, {174, 122,  89, 255}, {114,  62,  29, 255},  0, 495,         "Electric Contact Copper", 0L }
};

const int color::color_chart_count = sizeof(color_chart) / sizeof(color_entity);

void color::init()
{
  color_map_type &m = *const_cast<color_map_type *>(&color_map);
  
  for (int i = 0; i < color_chart_count; ++i)
    m[color_chart[i].id] = &color_chart[i];
  
  m_initialized = true;
}

color::~color()
{
  if (m_custom_color)
    delete m_entity;
}

// Map color-id to appropriate color value
void color::link()
{
  if (!m_initialized)
    throw exception(__func__, exception::user_error, "Color table is not initialized! run color::init() first.");
  
  m_custom_color = false;
  m_valid = true;
  
  const color_map_type::const_iterator it = color_map.find(m_id);
  if (it != color_map.end()) {
    m_entity = (*it).second;
    return;
  }
  
  if (m_id >= 256 && m_id <= 512) {
    // Dithered color (256 <= n <= 512)
    m_custom_color = true;
    const color_entity *c1 = color((m_id - 256) / 16).get_entity();
    const color_entity *c2 = color((m_id - 256) % 16).get_entity();
    
    color_entity *nc = new color_entity;
    nc->material = material_normal;
    nc->id = m_id;
    nc->name = "Dithered color";
    nc->rgba[0] = (unsigned char) (((int) c1->rgba[0] + (int) c2->rgba[0])/2);
    nc->rgba[1] = (unsigned char) (((int) c1->rgba[1] + (int) c2->rgba[1])/2);
    nc->rgba[2] = (unsigned char) (((int) c1->rgba[2] + (int) c2->rgba[2])/2);
    nc->rgba[3] = (unsigned char) (((int) c1->rgba[3] + (int) c2->rgba[3])/2);

    nc->complement[0] = 89;
    nc->complement[1] = 89;
    nc->complement[2] = 89;
    nc->complement[3] = 255;
    
    m_entity = nc;
    
    return;
  } else if ((m_id & 0xff000000) == 0x04000000) {
    m_custom_color = true;
    unsigned int v = m_id & 0xfff;

    color_entity *nc = new color_entity;
    nc->material = material_normal;
    nc->id = m_id;
    nc->name = "MLCad custom color";
    nc->rgba[0] = (unsigned char) (((v & 0xf00) >> 8) * 255.0f / 15.0f);
    nc->rgba[1] = (unsigned char) (((v & 0x0f0) >> 4) * 255.0f / 15.0f);
    nc->rgba[2] = (unsigned char) (((v & 0x00f)     ) * 255.0f / 15.0f);
    nc->rgba[3] = 255;

    nc->complement[0] = (unsigned char) (((v & 0xf00000) >> 20) * 255.0f / 15.0f);
    nc->complement[1] = (unsigned char) (((v & 0x0f0000) >> 16) * 255.0f / 15.0f);
    nc->complement[2] = (unsigned char) (((v & 0x00f000) >> 12) * 255.0f / 15.0f);
    nc->complement[3] = 255;

    m_entity = nc;

    return;
  }
  
  if (m_id == 16 || m_id == 24) {
    m_entity = &color_chart[0];
    return;
  }
  
  std::cerr << "[libLDR] couldn't resolve color " << m_id << std::endl;
  
  // No color
  m_valid = false;
  m_entity = &color_chart[0];
  
  /*std::ostringstream s;
    s << "Invalid color value '" << m_id << "'.";
    throw exception(__func__, exception::warning, s.str());*/
}

}

