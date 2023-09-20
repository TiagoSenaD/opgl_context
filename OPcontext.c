#include"OPcontext.h"

void init_BGLC(BGLC_Window** win){

  (*win)->dpy = XOpenDisplay(NULL);
  (*win)->xcb_connection = XGetXCBConnection((*win)->dpy);
  int x11_screen_idx = DefaultScreen((*win)->dpy);

  XSetEventQueueOwner((*win)->dpy, XCBOwnsEventQueue);
  xcb_screen_t* xcb_scrn = xcb_get_scrn((*win)->xcb_connection, x11_screen_idx);

  int glx_fbconfig_attrs[]={
    GLX_BUFFER_SIZE, 16,
    GLX_DOUBLEBUFFER, 0,
    GLX_SAMPLES, 0,
    GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
    0,
  };
  int glx_nfbconfigs;
  GLXFBConfig* glx_fbconfigs = glXChooseFBConfig((*win)->dpy, x11_screen_idx,glx_fbconfig_attrs, &glx_nfbconfigs); if(glx_nfbconfigs == 0) printf("FAIL");
  GLXFBConfig glx_fbconfig = glx_fbconfigs[0];
  XFree(glx_fbconfigs);
  int glx_visual_id; glXGetFBConfigAttrib((*win)->dpy, glx_fbconfig, GLX_VISUAL_ID, &glx_visual_id);

  (*win)->xcb_colormap = xcb_generate_id((*win)->xcb_connection); 
  xcb_create_colormap((*win)->xcb_connection, XCB_COLORMAP_ALLOC_NONE, (*win)->xcb_colormap, xcb_scrn->root, glx_visual_id);
  uint32_t value_mask = XCB_CW_BACK_PIXMAP | XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;
  uint32_t value_list[] ={XCB_BACK_PIXMAP_NONE, XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY, (*win)->xcb_colormap };
  (*win)->xcb_window = xcb_generate_id((*win)->xcb_connection);
  xcb_create_window((*win)->xcb_connection, xcb_scrn->root_depth, (*win)->xcb_window, xcb_scrn->root, 0, 0, xcb_scrn->width_in_pixels,xcb_scrn->height_in_pixels,0,XCB_WINDOW_CLASS_INPUT_OUTPUT, glx_visual_id, value_mask, value_list );
  xcb_map_window((*win)->xcb_connection, (*win)->xcb_window);
  xcb_flush((*win)->xcb_connection);

  int glx_context_attrs[] = {
    GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
    GLX_CONTEXT_MINOR_VERSION_ARB, 6,
    GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    GLX_CONTEXT_OPENGL_NO_ERROR_ARB, 1,
    0,
  };
  (*win)->glx_context = glXCreateContextAttribsARB((*win)->dpy, glx_fbconfig, NULL, 1, glx_context_attrs);
  glXMakeContextCurrent((*win)->dpy, (*win)->xcb_window, (*win)->xcb_window, (*win)->glx_context);
}

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

void close_BGLC(BGLC_Window** win){
  xcb_destroy_window((*win)->xcb_connection, (*win)->xcb_window);
  xcb_free_colormap((*win)->xcb_connection, (*win)->xcb_colormap);
  glXDestroyContext((*win)->dpy, (*win)->glx_context);
  XCloseDisplay((*win)->dpy);
  printf("terminate\n");
}
