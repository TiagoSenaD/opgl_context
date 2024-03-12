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

    while (1) {
        XEvent event;
        XNextEvent(display, &event);
        if (event.type == Expose) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glXSwapBuffers(display, window);
        }
        if (event.type == KeyPress) {
            break;
        }
    }

    destroyOpenGL(display, glContext);
    destroyX11(display, window);

    return 0;
}


/*Void change_color_win(){
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
  glFlush();
}change the screen color between white and balck*/
