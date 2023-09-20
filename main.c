#include"OPcontext.h"


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
  glFlush();
}//change the screen color between white and balck

int main(int argc, char ** argv)
{
    BGLC_Window * dpy;
    init_BGLC(&dpy);

    XEvent ev;
    int running = 1;
    while(running){
      xcb_generic_event_t* ev = xcb_ev_poll(dpy->xcb_connection, 0);
      change_color_win();
      if(ev != NULL){
        switch(ev->response_type & 0b01111111){
          case XCB_KEY_PRESS:{
            xcb_key_press_event_t* key_press = (xcb_key_press_event_t*)ev;
            xcb_keycode_t key_code = key_press->detail;
            //printf("%d\n",key_code);
            switch(key_code){
              case 67: running = 0; break; //F1
              default: printf("default\n"); break;
            }
          }break;
        }
      free(ev);
      }
    }
    close_BGLC(&dpy);
	return 0;
}
