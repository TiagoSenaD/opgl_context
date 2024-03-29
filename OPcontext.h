#ifndef OPCONTEXT_H
#define OPCONTEXT_H

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef struct {
    Display *display;
    Window root;
    Window window;
    XVisualInfo *visualInfo;
    GLXContext glContext;
    XEvent event;
} OpenGLContext;

Display* initX11(Display **display, Window *root, XVisualInfo **visualInfo, Window *window);
GLXContext initOpenGL(Display *display, XVisualInfo *visualInfo, Window window);
void destroyX11(Display *display, Window window);
void destroyOpenGL(Display *display, GLXContext glContext);
void changeColor();

#endif

