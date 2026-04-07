#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#define PI 3.14159265358979323846
#define INITIAL_WIN_W   900
#define INITIAL_WIN_H   600
#define FPS     60

typedef struct { double x, y, z; } Vec3;
typedef struct { int x, y; }       Point2D;

static inline Vec3 rotX(Vec3 p, double a) {
    return (Vec3){ p.x, p.y*cos(a)-p.z*sin(a), p.y*sin(a)+p.z*cos(a) };
}

static inline Vec3 rotY(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)+p.z*sin(a), p.y, -p.x*sin(a)+p.z*cos(a) };
}

static inline Vec3 rotZ(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)-p.y*sin(a), p.x*sin(a)+p.y*cos(a), p.z };
}

static inline Point2D project(Vec3 p, double cx, double cy, double fov) {
    double z = p.z + fov;
    if (z < 0.001) z = 0.001;
    double s = fov / z;
    return (Point2D){ (int)(p.x*s + cx), (int)(p.y*s + cy) };
}

#define CUBE_V 8
#define CUBE_E 12
const Vec3 CUBE_VERTS[CUBE_V] = {
    {-70,-70,-70},{70,-70,-70},{70,70,-70},{-70,70,-70},
    {-70,-70, 70},{70,-70, 70},{70,70, 70},{-70,70, 70}
};

const int CUBE_EDGES[CUBE_E][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

void draw_cube(Display *dpy, Pixmap buf, GC gc,
               double cx, double cy, double ax, double ay, double az)
{
    XSetForeground(dpy, gc, 0x00FFFF);   /* cyan */
    Point2D p[CUBE_V];
    for (int i = 0; i < CUBE_V; i++) {
        Vec3 v = CUBE_VERTS[i];
        v = rotX(v,ax); v = rotY(v,ay); v = rotZ(v,az);
        p[i] = project(v, cx, cy, 350.0);
    }
    for (int e = 0; e < CUBE_E; e++)
        XDrawLine(dpy, buf, gc,
                  p[CUBE_EDGES[e][0]].x, p[CUBE_EDGES[e][0]].y,
                  p[CUBE_EDGES[e][1]].x, p[CUBE_EDGES[e][1]].y);
}

void label(Display *dpy, Pixmap buf, GC gc, int x, int y,
           const char *text, unsigned long color)
{
    XSetForeground(dpy, gc, color);
    XDrawString(dpy, buf, gc, x, y, text, (int)strlen(text));
}

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

    XStoreName(dpy, win, "3D Wireframe Engine | CUBE | Q = quit");
    XMapWindow(dpy, win);

    GC     gc     = XCreateGC(dpy, win, 0, NULL);
    Pixmap buf    = XCreatePixmap(dpy, win, win_w, win_h,
                                  DefaultDepth(dpy, screen));
    GC     buf_gc = XCreateGC(dpy, buf, 0, NULL);

    double ax = 0, ay = 0, az = 0;
    int running = 1;

    while (running) {
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

        XSetForeground(dpy, buf_gc, BlackPixel(dpy, screen));
        XFillRectangle(dpy, buf, buf_gc, 0, 0, win_w, win_h);

        double cx = win_w / 2.0;
        double cy = win_h / 2.0;

        draw_cube(dpy, buf, buf_gc, cx, cy, ax, ay, az);
        label(dpy, buf, buf_gc, win_w/2 - 18, win_h - 30, "CUBE", 0x00FFFF);

        XCopyArea(dpy, buf, win, gc, 0, 0, win_w, win_h, 0, 0);
        XFlush(dpy);

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