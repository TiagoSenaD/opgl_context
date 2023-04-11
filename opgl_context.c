#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<poll.h>

#define GLX_GLXEXT_PROTOTYPES
#include<GL/glx.h>
#include<GL/glxext.h>
#define GL_GLEXT_PROTOTYPES
#include<GL/gl.h>
#include<GL/glext.h>

#include<X11/X.h>
#include<X11/Xlib-xcb.h>


xcb_screen_t* xcb_get_scrn (xcb_connection_t* connection, int scrn_idx){

  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

  for (; iter.rem; --scrn_idx, xcb_screen_next (&iter)){
    if (scrn_idx == 0) return iter.data;
  }
  return NULL;
} // return Xlib Screen struct, pass by all screens until find the chosen screen.

xcb_generic_event_t* xcb_ev_poll(xcb_connection_t * connection, int timeout_ms){
  struct pollfd pfd = {0x00};
  pfd.events = POLLIN;
  pfd.fd = xcb_get_file_descriptor(connection);
  int ntriggers = poll(&pfd, 1, timeout_ms);
  return xcb_poll_for_event(connection);
}

void infos(){
  GLint Ver[3];
  printf("==========INFOS==========\n");
  glGetIntegerv(GL_MAJOR_VERSION, &Ver[0]);
  glGetIntegerv(GL_MINOR_VERSION, &Ver[1]);
  printf("Version: %d.%d\n", Ver[0], Ver[1]);
}//print the Opengl version

void change_color_win(){
  static int flag = 1;
  static float NC = 0;
  glClearColor(NC, NC, NC, 1);
  if(flag){
    NC += 1.0/5000.0;
    if(NC>=1.0) flag = 0;
  }
  else{
    NC -= 1.0/5000.0;
    if(NC<=0.0) flag= 1;
  }
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}//change the screen color between white and balck

void glx_fbconfig_meta(Display * xlib_display, GLXFBConfig glx_fbconfig){
  int fbconfig_id;            glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_FBCONFIG_ID, &fbconfig_id);
  int fbconfig_visual;        glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_VISUAL_ID, &fbconfig_visual);

  int fbconfig_doublebuffer;  glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_DOUBLEBUFFER, &fbconfig_doublebuffer);
  int fbconfig_sample_buffer; glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_SAMPLE_BUFFERS, &fbconfig_sample_buffer);
  int fbconfig_samples;       glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_SAMPLES, &fbconfig_samples);
  int fbconfig_stereo;        glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_STEREO, &fbconfig_stereo);
  int fbconfig_aux_buffers;   glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_AUX_BUFFERS, &fbconfig_aux_buffers);

  int fbconfig_red_size;      glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_RED_SIZE, &fbconfig_red_size);
  int fbconfig_gree_size;     glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_GREEN_SIZE, &fbconfig_gree_size);
  int fbconfig_blue_size;     glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_BLUE_SIZE, &fbconfig_blue_size);
  int fbconfig_alpha_size;    glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_ALPHA_SIZE, &fbconfig_alpha_size);

  int fbconfig_buffer_size;   glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_BUFFER_SIZE, &fbconfig_buffer_size);
  int fbconfig_depth_size;    glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_DEPTH_SIZE, &fbconfig_depth_size);
  int fbconfig_stencil_size;  glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_STENCIL_SIZE, &fbconfig_stencil_size);
}


int main(int argc, char** argv){

  Display* xlib_display = XOpenDisplay(":0"); //cria um display genérico
  int      x11_screen_idx = DefaultScreen(xlib_display); //retorna o indice do display atual

  XSetEventQueueOwner(xlib_display, XCBOwnsEventQueue);
  xcb_connection_t* xcb_connection = XGetXCBConnection(xlib_display);
  xcb_screen_t* xcb_scrn = xcb_get_scrn(xcb_connection, x11_screen_idx);

  int glx_fbconfig_attrs[]={
    GLX_BUFFER_SIZE, 16,
    GLX_DOUBLEBUFFER, 0,
    GLX_SAMPLES, 0,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    0,
  };
  int glx_nfbconfigs;
  GLXFBConfig* glx_fbconfigs = glXChooseFBConfig(xlib_display, x11_screen_idx,glx_fbconfig_attrs, &glx_nfbconfigs); if(glx_nfbconfigs == 0) printf("FAIL");
  GLXFBConfig glx_fbconfig = glx_fbconfigs[0];
  XFree(glx_fbconfigs);
  int glx_visual_id; glXGetFBConfigAttrib(xlib_display, glx_fbconfig, GLX_VISUAL_ID, &glx_visual_id);

  uint32_t xcb_colormap = xcb_generate_id(xcb_connection); 
  xcb_create_colormap(xcb_connection, XCB_COLORMAP_ALLOC_NONE, xcb_colormap, xcb_scrn->root, glx_visual_id);
  uint32_t value_mask = XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
  uint32_t value_list[] ={XCB_BACK_PIXMAP_NONE, XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY, xcb_colormap };
  uint32_t xcb_window = xcb_generate_id(xcb_connection);
  xcb_create_window(xcb_connection, xcb_scrn->root_depth, xcb_window, xcb_scrn->root, 0, 0, xcb_scrn->width_in_pixels,xcb_scrn->height_in_pixels,0,XCB_WINDOW_CLASS_INPUT_OUTPUT, glx_visual_id, value_mask, value_list );
  xcb_map_window(xcb_connection, xcb_window);
  xcb_flush(xcb_connection);
 
  int glx_context_attrs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
    GLX_CONTEXT_MINOR_VERSION_ARB, 6,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    GLX_CONTEXT_OPENGL_NO_ERROR_ARB, 1,
    0,
  };
  GLXContext glx_context = glXCreateContextAttribsARB(xlib_display, glx_fbconfig, NULL, 1, glx_context_attrs);
  glXMakeContextCurrent(xlib_display, xcb_window, xcb_window, glx_context);

  //------------------------------------------------------------------------------------------
  
  XEvent ev;
  int running = 1;
  while(running){
    xcb_generic_event_t* ev = xcb_ev_poll(xcb_connection, 0);
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
  

  //------------------------------------------------------------------------------------------
  xcb_destroy_window(xcb_connection, xcb_window);
  xcb_free_colormap(xcb_connection, xcb_colormap);
  glXDestroyContext(xlib_display, glx_context);
  XCloseDisplay(xlib_display);
  return 0;
}


// clang -o Context_exe opgl_context.c -lX11 -lX11-xcb -lxcb -lGL
// gcc -o Context_exe opgl_context.c -lX11 -lX11-xcb -lxcb -lGL
