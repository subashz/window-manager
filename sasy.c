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
    //Testing
    int number = 20; // Connection to the X-server on screen 20: put NULL as default
    conn = xcb_connect(NULL,&number);

    // Get the current screen
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    uint16_t s_width = screen->width_in_pixels;
    uint16_t s_height = screen->height_in_pixels;
    uint16_t bord = 3;
    uint16_t gap = 5;
    uint16_t x1 = s_width-bord*3;
    uint16_t y1 = s_height-bord*3;

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
            // listen for application /window opening event
            case XCB_MAP_REQUEST: {
                xcb_map_request_event_t *e = (xcb_map_request_event_t *)ev;
                // Then maps the window to the screen
                xcb_map_window(conn,e->window);
                uint32_t values[4] = {gap,gap};
                // resize the x,y
                xcb_configure_window(conn,e->window,XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y,values); 
                uint32_t resizeValues[2] = {x1,y1};
                // resize the height and width
                xcb_configure_window(conn,e->window, XCB_CONFIG_WINDOW_WIDTH|XCB_CONFIG_WINDOW_HEIGHT,resizeValues);
                xcb_flush(conn);
                uint32_t border[1] = { bord };
                xcb_configure_window(conn, e->window, XCB_CONFIG_WINDOW_BORDER_WIDTH, border);
                // get the focus
                xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT,
                    e->window, XCB_CURRENT_TIME);
                xcb_flush(conn);
            } break;
            default: {
                printf("Other events");
            }break;
        }
    }
    // free up the event
    free(ev);
    }
    return 0;
}
