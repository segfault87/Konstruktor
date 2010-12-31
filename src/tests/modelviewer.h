#ifndef _MODELVIEWER_H_
#define _MODELVIEWER_H_

#include <renderer/parameters.h>
#include <renderer/renderer_opengl.h>

extern int width_, height_;
extern float length_;
extern int memsiz_;
extern ldraw_renderer::parameters params_;
extern ldraw_renderer::renderer_opengl_factory::rendering_mode mode_;
extern ldraw_renderer::renderer_opengl *renderer_;

extern const char *msg_immediate;
extern const char *msg_retained;
extern const char *msg_varray;
extern const char *msg_vbo;
extern const char *msg_shader;
extern const char *msg_no_shader;

bool initializeLdraw();
bool initializeModel(const char *filename);
void initDisplay();
void render(int elapsed);
void resize(int width, int height);

#endif
