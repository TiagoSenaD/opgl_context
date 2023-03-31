#include<stdio.h>
#include<stdint.h>

#define GLX_GLXEXT_PROTOTYPES
#include<GL/glx.h>
#include<GL/glxext.h>
#define GL_GLEXT_PROTOTYPES
#include<GL/gl.h>
#include<GL/glext.h>

#include<X11/X.h>
#include<X11/Xlib-xcb.h>


xcb_screen_t * xcb_get_scrn (xcb_connection_t* connection, int scrn_idx){

  xcb_screen_iterator_t iter = xcb_setup_roots_iterator(xcb_get_setup(connection));

  for (; iter.rem; --scrn_idx, xcb_screen_next (&iter)){
    if (scrn_idx == 0) return iter.data;
  }
  return NULL;
}

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
  
  printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", fbconfig_id, fbconfig_visual, fbconfig_doublebuffer, fbconfig_sample_buffer, fbconfig_samples,fbconfig_stereo, fbconfig_aux_buffers, fbconfig_red_size,fbconfig_gree_size, fbconfig_blue_size, fbconfig_alpha_size, fbconfig_buffer_size, fbconfig_depth_size, fbconfig_stencil_size);
}


int main(int argc, char** argv){

  Display* xlib_display = XOpenDisplay(":0");
  int      x11_screen_idx = DefaultScreen(xlib_display);

  XSetEventQueueOwner(xlib_display, XCBOwnsEventQueue);
  xcb_connection_t * xcb_connection = XGetXCBConnection(xlib_display);
  xcb_screen_t * xcb_scrn = xcb_get_scrn(xcb_connection, x11_screen_idx);

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
    GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
    GLX_CONTEXT_MINOR_VERSION_ARB, 3,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    GLX_CONTEXT_OPENGL_NO_ERROR_ARB, 1,
    0,
  };
  GLXContext glx_context = glXCreateContextAttribsARB(xlib_display, glx_fbconfig, NULL, 1, glx_context_attrs);
  glXMakeContextCurrent(xlib_display, xcb_window, xcb_window, glx_context);

  //------------------------------------------------------------------------------------------

  unsigned int BUFFER = 4294967295;
  float NC=0;
  static int flag = 1;
  while(1){
    //BUFFER-=1;
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
  }
  
  //------------------------------------------------------------------------------------------
  xcb_destroy_window(xcb_connection, xcb_window);
  xcb_free_colormap(xcb_connection, xcb_colormap);
  //glXDestroyWindow(xlib_display, glx_window);
  glXDestroyContext(xlib_display, glx_context);
  XCloseDisplay(xlib_display);
  return 0;
}


// clang -o exe opgl_context.c -lx11 -lx11-xcb -lxcb -lgl
// gcc -o exe opgl_context.c -lx11 -lx11-xcb -lxcb -lgl
