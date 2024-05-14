#ifndef OPCONTEXT_H
#define OPCONTEXT_H

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

typedef struct {
    Display *display;
    Window root;
    XVisualInfo *visualInfo;
    Window window;
    GLXContext glContext;
    //XEvent event;
} OpenGLContext;

OpenGLContext initSrc();
void endSrc(OpenGLContext win);
void changeColor();

#endif

