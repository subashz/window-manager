#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>

typedef struct client client;

xcb_connection_t *conn;
xcb_screen_t *screen;

uint16_t x;
uint16_t y;
 
// Counts the number of clients
uint8_t count ;

// Define the struct
struct client {
    xcb_window_t win;
    uint16_t x;
    uint16_t y;
    uint16_t height;
    uint16_t width;
} ;

//  Creates clientList
client clientList[3];

void resize(xcb_window_t win, uint16_t w, uint16_t h)
{
    uint32_t values[2] = {w,h};
    xcb_configure_window(conn,win,XCB_CONFIG_WINDOW_WIDTH
             | XCB_CONFIG_WINDOW_HEIGHT, values);
} 
void move(xcb_window_t win, uint16_t x, uint16_t y  )
{
    uint32_t values[2] = {x, y};
    xcb_configure_window(conn,win,XCB_CONFIG_WINDOW_X
            | XCB_CONFIG_WINDOW_Y,values); 
}

void draw(xcb_window_t win) {
    xcb_map_window(conn, win);
}

void border(xcb_window_t win, uint16_t width )
{
    uint32_t borderColor = 123452;
    uint32_t borderWidth[1] = { width };
    xcb_configure_window(conn, win, XCB_CONFIG_WINDOW_BORDER_WIDTH, borderWidth);
    xcb_change_window_attributes(conn, win, XCB_CW_BORDER_PIXEL, &borderColor);
}

void focus(xcb_window_t win)
{
    xcb_set_input_focus(conn, XCB_INPUT_FOCUS_POINTER_ROOT,win,XCB_CURRENT_TIME);
}

void manageClient(xcb_map_request_event_t *ev)
{
    uint16_t sHeight = screen->height_in_pixels;
    uint16_t sWidth = screen->width_in_pixels;
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
        width = width/2;
        resize(client[0],width,height);
        border(client[0],0);

        x = x + width + 20;
        move(ev->window,x,y);
        resize(ev->window,width,height);
        border(ev->window,bWidth);
        xcb_flush(conn);
    }
    else if(count == 2)
    {
        height = height/2;
        resize(client[1],width,height);
        border(client[1],0);

        y=y+height+20;
        move(ev->window,x,y);
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

void keyHandler(xcb_key_press_event_t *key)
{
    resize(client[0],200,200);
    xcb_flush(conn);
    uint32_t keyCode = key->detail;
    //50 shift 37 ctrl
    //Right
    if(keyCode == 50)
    {
        resize(client[0],200,200);
        xcb_flush(conn);
    }
    // Down
    else if(keyCode == 37)
    {
        resize(client[0],500,500);
        xcb_flush(conn);
    }
    //Left
    else if(keyCode == 116)
    {
        resize(client[0],1000,700);
        xcb_flush(conn);
    }
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
                case XCB_KEY_PRESS: {
                    xcb_key_press_event_t *e = (xcb_key_press_event_t *)eve;
                    xcb_key_sym_t keysym = xcb_key_symbols_get_keysym(e->detail);
                    if(keysym == "MOD4")
                        resize(eve->window,400,400);
                    //   <-113    114 ->
                    //   down 116 111 up  |^|
                    //keyHandler(e);
                }
                case XCB_DESTROY_NOTIFY: {
                    xcb_destroy_notify_event_t *ev = (xcb_destory_notify_event_t *)eve;
                // Do something 
                // evnet receive garxa.. window destroy huda yo event pauxa
                    printf("Window is destroying");
                 }
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
    x=10;
    y=10;
    
    // For managing child windows need to add some mask to root-window
    uint32_t values[1] = {XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY 
        | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT | XCB_EVENT_MASK_KEY_PRESS 
        | XCB_EVENT_MASK_BUTTON_PRESS} ;
 
    xcb_change_window_attributes(conn, screen->root, XCB_CW_EVENT_MASK, values);
    // Force to change effects
    xcb_flush(conn);

   eventHandler();

   return 0;
}
