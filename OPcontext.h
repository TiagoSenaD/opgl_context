#ifndef OPCONTEXT_H
#define OPCONTEXT_H

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


typedef struct{

 Display * dpy;
 xcb_connection_t* xcb_connection;
 uint32_t xcb_colormap;
 uint32_t xcb_window;
 GLXContext glx_context;

} BGLC_Window;

void init_BGLC_teste(BGLC_Window** win);

void init_BGLC(Display** dpy,xcb_connection_t** xcb_connection);

void close_BGLC(Display** dpy,xcb_connection_t** xcb_connection);

void glx_fbconfig_meta(Display * xlib_display, GLXFBConfig glx_fbconfig);

xcb_screen_t* xcb_get_scrn (xcb_connection_t* connection, int scrn_idx);

xcb_generic_event_t* xcb_ev_poll(xcb_connection_t * connection, int timeout_ms);

#endif
