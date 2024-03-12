#include"OPcontext.h"
#include <stdio.h>

Display* initX11(Display **display, Window *root, XVisualInfo **visualInfo, Window *window) {
    *display = XOpenDisplay(NULL);
    if (!(*display)) {
        fprintf(stderr, "Erro ao abrir o display X11\n");
        return NULL;
    }

    *root = DefaultRootWindow(*display);

    GLint glAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    *visualInfo = glXChooseVisual(*display, 0, glAttribs);
    if (!(*visualInfo)) {
        fprintf(stderr, "Erro ao escolher o visual X11\n");
        return NULL;
    }

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = XCreateColormap(*display, *root, (*visualInfo)->visual, AllocNone);
    windowAttributes.event_mask = ExposureMask | KeyPressMask;
    *window = XCreateWindow(*display, *root, 0, 0, 800, 600, 0,
        (*visualInfo)->depth, InputOutput, (*visualInfo)->visual, CWColormap | CWEventMask, &windowAttributes);

    XMapWindow(*display, *window);

    return *display;
}

GLXContext initOpenGL(Display *display, XVisualInfo *visualInfo, Window window) {
    GLXContext glContext = glXCreateContext(display, visualInfo, NULL, GL_TRUE);
    if (!glContext) {
        fprintf(stderr, "Erro ao criar o contexto OpenGL\n");
        return NULL;
    }

    glXMakeCurrent(display, window, glContext);

    return glContext;
}

void destroyX11(Display *display, Window window) {
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

void destroyOpenGL(Display *display, GLXContext glContext) {
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, glContext);
}

