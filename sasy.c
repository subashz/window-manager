#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

xcb_connection_t *conn;
xcb_screen_t *screen;

// Counts the number of clients
uint8_t count ;

// Arrays of clients
xcb_window_t client[2];

void resize(xcb_window_t win, uint16_t w, uint16_t h)
{
    uint32_t values[2] = {w,h};
    xcb_configure_window(conn,win,XCB_CONFIG_WINDOW_WIDTH
             | XCB_CONFIG_WINDOW_HEIGHT, values);
    //xcb_flush(conn);
} 
void move(xcb_window_t win, uint16_t x, uint16_t y  )
{
    uint32_t values[2] = {x, y};
    xcb_configure_window(conn,win,XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y,values); 
    //xcb_flush(conn);
}

void draw(xcb_window_t win) {
    xcb_map_window(conn, win);
    //xcb_flush(conn);
}

void border(xcb_window_t win, uint16_t width )
{
    uint16_t borderColor = 123452;
    uint32_t borderWidth[1] = { width };
    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, borderWidth);
    xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXEL, &borderColor);
    //xcb_flush(conn);
}

void focus(xcb_window_t win)
{
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT,win,XCB_CURRENT_TIME);
    //xcb_flush(conn);
}

void manageClient(xcb_map_request_event_t *ev)
{
    uint16_t sHeight = screen->height_in_pixels;
    uint16_t sWidth = screen->width_in_pixels;
    uint8_t x = 20;
    uint8_t y = 20;
    uint8_t bWidth = 4;
    uint16_t height = sHeight-28;
    uint16_t width = sWidth-28;

    draw(ev->window);
    if (count == 0)
    {
        move(ev->window,x,y);
        resize(ev->window,width,height);
        border(ev->window,bWidth);
        xcb_flush(conn);
    }
    else if(count == 1)
    {
        width = (sWidth/2) - 20;
        resize(client[0],width,height);
        border(client[0],0);
        //xcb_flush(conn);

        x = x + width + 20;
        move(ev->window,700,y);
        resize(ev->window,width,height);
        border(ev->window,bWidth);
        xcb_flush(conn);
    }
    else 
    {
        move(ev->window,10,10);
        resize(ev->window,width,height);
        border(ev->window,bWidth);
        xcb_flush(conn);

    }
    count++;
    focus(ev->window);
}

void eventHandler()
{
    xcb_generic_event_t *eve;
    while(1) 
    {
        if((eve=xcb_wait_for_event(conn))) {
           switch(eve->response_type & ~0x80) {
                // listen for application /window opening event
                case XCB_MAP_REQUEST: {
                    xcb_map_request_event_t *e = (xcb_map_request_event_t *)eve;
                    client[count] = e->window;
                    manageClient(e);
                } break;
                default: {
                         printf("To do");
                 } break;
           }
        }
        free(eve);
    }
}


int main() 
{
    conn = xcb_connect(NULL,NULL);
    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
    count = 0;
    
    // For managing child windows need to add some mask to root-window
    uint32_t values[1] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY 
                | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT};
    xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, values);
    // Force to change effects
    xcb_flush(conn);

   eventHandler();

   return 0;
}
