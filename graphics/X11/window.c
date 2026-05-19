#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(){
    int win_size = 1024;
    Display *d = XOpenDisplay(NULL);

    if (!d) { fprintf(stderr, "Cannot open display\n"); return 1; }

    int screen = DefaultScreen(d);
    Window root = RootWindow(d, screen);

    XSetWindowAttributes attrs;
    attrs.override_redirect = False;
    attrs.background_pixel = BlackPixel(d, screen);

    Window win = XCreateWindow(d, root, 0, 0, win_size, win_size, 0,
                                CopyFromParent, InputOutput, CopyFromParent,
                                CWOverrideRedirect | CWBackPixel, &attrs);
    XMapWindow(d, win);
    XFlush(d);

    // shutdown gracefully
    Atom wmDeleteMessage = XInternAtom(d, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(d, win, &wmDeleteMessage, 1);
    XSelectInput(d, win, KeyPressMask | ExposureMask);

    XEvent ev;
    int running = 1;
    KeySym keysym;
    while (running) {
        XNextEvent(d, &ev);
        if (ev.type == KeyPress) {
            keysym = XLookupKeysym(&ev.xkey, 0);
            if (keysym == XK_Escape) running = 0;
        }
        // if (ev.type == ClientMessage) {
        //     if (ev.xclient.data.l[0] == wmDeleteMessage) {
        //         running = 0;
        //     }
        // }
    }

    goto cleanup;
cleanup:
    XDestroyWindow(d, win);
    XCloseDisplay(d);
    return 0;
}
