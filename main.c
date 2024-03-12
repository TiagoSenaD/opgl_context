#include <stdio.h>
#include"OPcontext.h"

int main() {
    Display *display;
    Window root, window;
    XVisualInfo *visualInfo;
    GLXContext glContext;

    display = initX11(&display, &root, &visualInfo, &window);
    if (!display) return 1;

    glContext = initOpenGL(display, visualInfo, window);
    if (!glContext) return 1;


    XEvent ev;
    while (1) {
        changeColor();
        glXSwapBuffers(display, window);
        if (XPending(display)) {
            XNextEvent(display, &ev);
            if (ev.type == KeyPress | ev.type == ButtonPress | ev.type == ButtonRelease | ev.type == MotionNotify) {
                printf("KeyPress: keycode %u\n",(unsigned)ev.xkey.keycode);

                if (ev.xkey.keycode == 9) break;
                }
        }
    }

    destroyOpenGL(display, glContext);
    destroyX11(display, window);
    return 0;
}

