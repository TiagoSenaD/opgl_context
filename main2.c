/* gcc this.c -lGL -lX11 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

void change_color_win(){
  static int flag = 1;
  static float NC = 0;
  glClearColor(NC, NC, NC, 1);
  if(flag){
    NC += 1.0/500.0;
    if(NC>=1.0) flag = 0;
  }
  else{
    NC -= 1.0/500.0;
    if(NC<=0.0) flag= 1;
  }
  glClear(GL_COLOR_BUFFER_BIT);
}



typedef GLXContext (*glXCreateContextAttribsARBProc)
    (Display*, GLXFBConfig, GLXContext, Bool, const int*);

int main()
{
    Display* disp = 0;
    Window win = 0;

    disp = XOpenDisplay(0);
    win = XCreateSimpleWindow(disp, DefaultRootWindow(disp),
                              10, 10,   /* x, y */
                              800, 600, /* width, height */
                              0, 0,     /* border_width, border */
                              0);       /* background */


    static int visual_attribs[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_DOUBLEBUFFER, true,
        GLX_RED_SIZE, 1,
        GLX_GREEN_SIZE, 1,
        GLX_BLUE_SIZE, 1,
        None
    };

    int num_fbc = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(disp,
                                         DefaultScreen(disp),
                                         visual_attribs, &num_fbc);
    if (!fbc) {
        printf("glXChooseFBConfig() failed\n");
        exit(1);
    }

    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)
        glXGetProcAddress((const GLubyte*)"glXCreateContextAttribsARB");


    if (!glXCreateContextAttribsARB) {
        printf("glXCreateContextAttribsARB() not found\n");
        exit(1);
    }


    static int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        None
    };

    GLXContext ctx = glXCreateContextAttribsARB(disp, fbc[0], NULL, true,
                                                context_attribs);
    if (!ctx) {
        printf("Failed to create OpenGL context. Exiting.\n");
        exit(1);
    }

    XSelectInput(disp, win, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
    XMapWindow(disp, win);
    glXMakeCurrent(disp, win, ctx);

    int major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("OpenGL context created.\nVersion %d.%d\nVendor %s\nRenderer %s\n",
           major, minor,
           glGetString(GL_VENDOR),
           glGetString(GL_RENDERER));

    /* Application_loop
       ---------------- */
    while(1) {
        change_color_win();
        glXSwapBuffers ( disp, win );
        glFlush();

        XEvent ev;
        if (XPending(disp)) {
            XNextEvent(disp, &ev);
            if (ev.type == KeyPress | ev.type == ButtonPress | ev.type == ButtonRelease | ev.type == MotionNotify) {
                printf("KeyPress: keycode %u\n",(unsigned)ev.xkey.keycode);
                
                if (ev.xkey.keycode == 9) break;
                }
        }
    }           
    return 0;
}