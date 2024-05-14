#include <stdio.h>
#include"OPcontext.h"

int main() {

    OpenGLContext win = initSrc();
    if (!win.display) return 1;

    XEvent ev;
    while (1) {
        changeColor();
        glXSwapBuffers(win.display, win.window);
        if (XPending(win.display)) {
            XNextEvent(win.display, &ev);
            if (ev.type == KeyPress | ev.type == ButtonPress | ev.type == ButtonRelease | ev.type == MotionNotify) {
                printf("KeyPress: keycode %u\n",(unsigned)ev.xkey.keycode);

                if (ev.xkey.keycode == 9) break;
                }
        }
    }

    endSrc(win);
    return 0;
}

