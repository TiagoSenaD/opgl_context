#include"OPcontext.h"
#include <stdio.h>

OpenGLContext initSrc() {
    OpenGLContext src;
    src.display = XOpenDisplay(NULL);
    if (!src.display) {
        fprintf(stderr, "Erro ao abrir o display X11\n");
        src.root = 0;
        src.visualInfo = NULL;
        src.window = 0;
        src.glContext = NULL;
        return src;
    }

    src.root = DefaultRootWindow(src.display);

    GLint glAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    src.visualInfo = glXChooseVisual(src.display, 0, glAttribs);
    if (!src.visualInfo) {
        fprintf(stderr, "Erro ao escolher o visual X11\n");
        XCloseDisplay(src.display);
        src.display=NULL;
        return src;
    }

    XSetWindowAttributes windowAttributes;
    windowAttributes.colormap = XCreateColormap(src.display, src.root, src.visualInfo->visual, AllocNone);
    windowAttributes.event_mask = ExposureMask | KeyPressMask;
    src.window = XCreateWindow(src.display, src.root, 0, 0, 800, 600, 0,
        src.visualInfo->depth, InputOutput, src.visualInfo->visual, CWColormap | CWEventMask, &windowAttributes);

    XSelectInput(src.display, src.window, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    XMapWindow (src.display, src.window);

    src.glContext = glXCreateContext(src.display, src.visualInfo, NULL, GL_TRUE);
    if (!src.glContext) {
        fprintf(stderr, "Erro ao criar o contexto OpenGL\n");
        XDestroyWindow(src.display, src.window);
        XCloseDisplay(src.display);
        src.display=NULL;
        return src;
    }

    glXMakeCurrent(src.display, src.window, src.glContext);

    return src;
}

void endSrc(OpenGLContext win) {
    glXMakeCurrent(win.display, None, NULL);
    glXDestroyContext(win.display, win.glContext);
    XDestroyWindow(win.display, win.window);
    XCloseDisplay(win.display);
}

void changeColor(){
    static int flag = 1;
    static float NC = 0;
    glClearColor(NC, NC, NC, 1);
    if(flag){
	NC >= 1.0 ? flag = 0 : (NC += 1.0/500.0);
    }
    else{
	NC <= 0.0 ? flag = 1 : (NC -= 1.0/500.0);
    }
    glClear(GL_COLOR_BUFFER_BIT);
}

