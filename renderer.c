#include "renderer.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ── Label helper ── */
void label(Display *dpy, Pixmap buf, GC gc, int x, int y,
           const char *text, unsigned long color)
{
    XSetForeground(dpy, gc, color);
    XDrawString(dpy, buf, gc, x, y, text, (int)strlen(text));
}

/* ── Main function ── */
int main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    int screen = DefaultScreen(dpy);
    XSetWindowAttributes xwa = {
        .background_pixel = BlackPixel(dpy, screen),
        .event_mask       = KeyPressMask | ExposureMask | StructureNotifyMask
    };
    
    int win_w = INITIAL_WIN_W;
    int win_h = INITIAL_WIN_H;

    Window win = XCreateWindow(
        dpy, RootWindow(dpy, screen),
        100, 100, win_w, win_h, 0,
        DefaultDepth(dpy, screen), InputOutput,
        DefaultVisual(dpy, screen),
        CWBackPixel | CWEventMask, &xwa);

    /* Prompt user for shape selection */
    char shape_input[10];
    ShapeType selected_shape = SHAPE_CUBE; /* default */

    printf("Which shape do you want to render?\n");
    printf("1. Cube\n");
    printf("2. Pyramid\n");
    printf("3. Sphere\n");
    printf("Enter choice (1-3): ");
    fflush(stdout);

    if (fgets(shape_input, sizeof(shape_input), stdin) != NULL) {
        int choice = atoi(shape_input);
        switch (choice) {
            case 1: selected_shape = SHAPE_CUBE; break;
            case 2: selected_shape = SHAPE_PYRAMID; break;
            case 3: selected_shape = SHAPE_SPHERE; break;
            default:
                printf("Invalid choice, defaulting to Cube\n");
                selected_shape = SHAPE_CUBE;
        }
    } else {
        printf("No input, defaulting to Cube\n");
        selected_shape = SHAPE_CUBE;
    }

    /* Set window title based on selection */
    const char *shape_name;
    switch (selected_shape) {
        case SHAPE_CUBE: shape_name = "CUBE"; break;
        case SHAPE_PYRAMID: shape_name = "PYRAMID"; break;
        case SHAPE_SPHERE: shape_name = "SPHERE"; break;
    }

    char window_title[100];
    snprintf(window_title, sizeof(window_title), "3D Wireframe Engine | %s | Q = quit", shape_name);
    XStoreName(dpy, win, window_title);

    XMapWindow(dpy, win);

    GC     gc     = XCreateGC(dpy, win, 0, NULL);
    Pixmap buf    = XCreatePixmap(dpy, win, win_w, win_h,
                                  DefaultDepth(dpy, screen));
    GC     buf_gc = XCreateGC(dpy, buf, 0, NULL);

    /* Each shape rotates at a slightly different speed for variety */
    double ax = 0, ay = 0, az = 0;
    int running = 1;

    while (running) {
        /* Events */
        while (XPending(dpy)) {
            XEvent ev; XNextEvent(dpy, &ev);
            if (ev.type == KeyPress) {
                KeySym ks = XLookupKeysym(&ev.xkey, 0);
                if (ks == 'q' || ks == 'Q') running = 0;
            } else if (ev.type == ConfigureNotify) {
                if (ev.xconfigure.width != win_w || ev.xconfigure.height != win_h) {
                    win_w = ev.xconfigure.width;
                    win_h = ev.xconfigure.height;
                    XFreePixmap(dpy, buf);
                    buf = XCreatePixmap(dpy, win, win_w, win_h, DefaultDepth(dpy, screen));
                }
            }
        }

        /* Clear */
        XSetForeground(dpy, buf_gc, BlackPixel(dpy, screen));
        XFillRectangle(dpy, buf, buf_gc, 0, 0, win_w, win_h);

        /* Center the shape in the window */
        double cx = win_w / 2.0;
        double cy = win_h / 2.0;

        /* Draw selected shape */
        switch (selected_shape) {
            case SHAPE_CUBE:
                draw_cube(dpy, buf, buf_gc, cx, cy, ax, ay, az);
                label(dpy, buf, buf_gc, win_w/2 - 18, win_h - 30, "CUBE", 0x00FFFF);
                break;
            case SHAPE_PYRAMID:
                draw_pyramid(dpy, buf, buf_gc, cx, cy, ax*0.8, ay*1.2, az*0.6);
                label(dpy, buf, buf_gc, win_w/2 - 28, win_h - 30, "PYRAMID", 0xFF8800);
                break;
            case SHAPE_SPHERE:
                draw_sphere(dpy, buf, buf_gc, cx, cy, ax*1.1, ay*0.7, az*1.3);
                label(dpy, buf, buf_gc, win_w/2 - 22, win_h - 30, "SPHERE", 0x00FF66);
                break;
        }

        /* Flip buffer */
        XCopyArea(dpy, buf, win, gc, 0, 0, win_w, win_h, 0, 0);
        XFlush(dpy);

        /* Advance angles */
        ax += 0.013;
        ay += 0.018;
        az += 0.008;

        usleep(1000000 / FPS);
    }

    XFreeGC(dpy, gc);
    XFreeGC(dpy, buf_gc);
    XFreePixmap(dpy, buf);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return 0;
}