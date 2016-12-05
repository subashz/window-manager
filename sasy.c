#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

int main() 
{
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    xcb_drawable_t root;
    xcb_drawable_t win;
    xcb_generic_event_t *ev;

    // Connection to the X-server
    conn = xcb_connect(NULL,NULL);

    // Get the current screen
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    uint16_t s_width = screen->width_in_pixels;
    uint16_t s_height = screen->height_in_pixels;

    root = screen->root;
    
    // Flush all requests
    xcb_flush(conn);

    // Loops
    while((ev=xcb_wait_for_event(conn))) {
      //  switch(ev->response_type & ~0x80) {
            // Doing so much here
            //printf("Just a test");
              //} break;
    }
}
    //Free the memory hold by event
    free(ev);
    return 0;
}
