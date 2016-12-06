#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <signal.h> 

int main() 
{
    xcb_connection_t *conn;
    xcb_screen_t *screen;
    xcb_drawable_t win;
    xcb_generic_event_t *ev;
    uint8_t bord = 3;

    // Connection to the X-server
    conn = xcb_connect(NULL,NULL);

    // Get the current screen
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    uint16_t s_width = screen->width_in_pixels;
    uint16_t s_height = screen->height_in_pixels;

    // For managing child windows need to add some mask to root-window
    uint32_t values[1] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY |
                        XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
    xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, values);

    // Flush all requests
    xcb_flush(conn);

    // Loops
   while(1) { 
    if((ev=xcb_wait_for_event(conn))) {
        switch(ev->response_type & ~0x80) {
            case XCB_MAP_REQUEST: {
                xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;
                xcb_map_window(conn,e->window);
                uint32_t values[4] = {10,10};
                xcb_configure_window(conn,e->window,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y,values); 
                uint32_t resizeValues[2] = {1000,600};
                xcb_configure_window(conn,e->window, XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,resizeValues);
                uint32_t a[1] = { bord };
                xcb_configure_window(conn, e->window, XCB_CONFIG_WINDOW_BORDER_WIDTH, a);
                xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT,
                    e->window, XCB_CURRENT_TIME);
                xcb_flush(conn);
            } break;
            default: {
                printf("Other events");
            }break;
        }
    }
    free(ev);
    }
//    pause();

    return 0;
}
