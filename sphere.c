#include <X11/Xlib.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define FPS     60
#define PI      3.14159265358979323846

/* ── Vec3 ── */
typedef struct { double x, y, z; } Vec3;
typedef struct { int x, y; }       Point2D;

/* ── Rotation matrices ── */
Vec3 rotX(Vec3 p, double a) {
    return (Vec3){ p.x, p.y*cos(a)-p.z*sin(a), p.y*sin(a)+p.z*cos(a) };
}
Vec3 rotY(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)+p.z*sin(a), p.y, -p.x*sin(a)+p.z*cos(a) };
}
Vec3 rotZ(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)-p.y*sin(a), p.x*sin(a)+p.y*cos(a), p.z };
}

/* ── Perspective projection ── */
Point2D project(Vec3 p, double cx, double cy, double fov) {
    double z = p.z + fov;
    if (z < 0.001) z = 0.001;
    double s = fov / z;
    return (Point2D){ (int)(p.x*s + cx), (int)(p.y*s + cy) };
}

/* ═══════════════════════════════════════════════
   SHAPE 3 — SPHERE  (lat/lon wireframe)
   Pure maths: parametric sphere
     x = r·sin(φ)·cos(θ)
     y = r·cos(φ)
     z = r·sin(φ)·sin(θ)
   ═══════════════════════════════════════════════ */
#define SPH_LAT  10    /* latitude  rings  */
#define SPH_LON  12    /* longitude lines  */
#define SPH_R    90.0

void draw_sphere(Display *dpy, Pixmap buf, GC gc,
                 double cx, double cy, double ax, double ay, double az)
{
    XSetForeground(dpy, gc, 0x00FF66);   /* green */

    /* Latitude rings: φ from π/(LAT+1) to π·LAT/(LAT+1) */
    for (int lat = 1; lat <= SPH_LAT; lat++) {
        double phi = PI * lat / (SPH_LAT + 1);
        Point2D prev = {0,0};
        for (int lon = 0; lon <= SPH_LON; lon++) {
            double theta = 2.0 * PI * lon / SPH_LON;
            Vec3 v = {
                SPH_R * sin(phi) * cos(theta),
                SPH_R * cos(phi),
                SPH_R * sin(phi) * sin(theta)
            };
            v = rotX(v,ax); v = rotY(v,ay); v = rotZ(v,az);
            Point2D cur = project(v, cx, cy, 350.0);
            if (lon > 0)
                XDrawLine(dpy, buf, gc, prev.x, prev.y, cur.x, cur.y);
            prev = cur;
        }
    }

    /* Longitude lines: θ fixed, φ sweeps 0→π */
    for (int lon = 0; lon < SPH_LON; lon++) {
        double theta = 2.0 * PI * lon / SPH_LON;
        Point2D prev = {0,0};
        for (int lat = 0; lat <= SPH_LAT + 1; lat++) {
            double phi = PI * lat / (SPH_LAT + 1);
            Vec3 v = {
                SPH_R * sin(phi) * cos(theta),
                SPH_R * cos(phi),
                SPH_R * sin(phi) * sin(theta)
            };
            v = rotX(v,ax); v = rotY(v,ay); v = rotZ(v,az);
            Point2D cur = project(v, cx, cy, 350.0);
            if (lat > 0)
                XDrawLine(dpy, buf, gc, prev.x, prev.y, cur.x, cur.y);
            prev = cur;
        }
    }
}

/* ═══════════════════════════════════════════════
   LABEL helper
   ═══════════════════════════════════════════════ */
void label(Display *dpy, Pixmap buf, GC gc, int x, int y,
           const char *text, unsigned long color)
{
    XSetForeground(dpy, gc, color);
    XDrawString(dpy, buf, gc, x, y, text, (int)strlen(text));
}

/* ═══════════════════════════════════════════════
   MAIN
   ═══════════════════════════════════════════════ */
int main(void)
{
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) { fprintf(stderr, "Cannot open display\n"); return 1; }

    int screen = DefaultScreen(dpy);
    int WIN_W = 900;
	int WIN_H = 600;
    XSetWindowAttributes xwa = {
        .background_pixel = BlackPixel(dpy, screen),
        .event_mask       = KeyPressMask | ExposureMask | StructureNotifyMask
    };
    Window win = XCreateWindow(
        dpy, RootWindow(dpy, screen),
        100, 100, WIN_W, WIN_H, 0,
        DefaultDepth(dpy, screen), InputOutput,
        DefaultVisual(dpy, screen),
        CWBackPixel | CWEventMask, &xwa);
    XStoreName(dpy, win, "3D Wireframe Engine | Cube  Pyramid  Sphere | Q = quit");
    XMapWindow(dpy, win);

    GC     gc     = XCreateGC(dpy, win, 0, NULL);
    Pixmap buf    = XCreatePixmap(dpy, win, WIN_W, WIN_H,
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
            }
            if (ev.type == ConfigureNotify) {
        		WIN_W = ev.xconfigure.width;
        		WIN_H = ev.xconfigure.height;

        		XFreePixmap(dpy, buf);
        		buf = XCreatePixmap(dpy, win, WIN_W, WIN_H,
        		DefaultDepth(dpy, screen));

        		XFreeGC(dpy, buf_gc);
        		buf_gc = XCreateGC(dpy, buf, 0, NULL);
    		}
        }

        /* Clear */
        XSetForeground(dpy, buf_gc, BlackPixel(dpy, screen));
        XFillRectangle(dpy, buf, buf_gc, 0, 0, WIN_W, WIN_H);

        /* ── Draw each shape in its own third of the window ── */
        draw_sphere(dpy, buf, buf_gc, WIN_W/2, WIN_H/2, ax, ay, az);

        /* Labels */
        label(dpy, buf, buf_gc, WIN_W/2 - 25, WIN_H - 30, "SPHERE", 0x00FF66);

        /* Flip buffer */
        XCopyArea(dpy, buf, win, gc, 0, 0, WIN_W, WIN_H, 0, 0);
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
