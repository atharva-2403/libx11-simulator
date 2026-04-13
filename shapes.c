#include "shapes.h"

/* ── Cube data ── */
const Vec3 CUBE_VERTS[CUBE_V] = {
    {-70,-70,-70},{70,-70,-70},{70,70,-70},{-70,70,-70},
    {-70,-70, 70},{70,-70, 70},{70,70, 70},{-70,70, 70}
};

const int CUBE_EDGES[CUBE_E][2] = {
    {0,1},{1,2},{2,3},{3,0},
    {4,5},{5,6},{6,7},{7,4},
    {0,4},{1,5},{2,6},{3,7}
};

/* ── Pyramid data ── */
const Vec3 PYR_VERTS[PYR_V] = {
    {-70, 70,-70},{ 70, 70,-70},{ 70, 70, 70},{-70, 70, 70},  /* base */
    {  0,-90,  0}                                               /* apex */
};

const int PYR_EDGES[PYR_E][2] = {
    {0,1},{1,2},{2,3},{3,0},   /* base square */
    {0,4},{1,4},{2,4},{3,4}    /* side edges  */
};

/* ── Drawing functions ── */
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

void draw_pyramid(Display *dpy, Pixmap buf, GC gc,
                  double cx, double cy, double ax, double ay, double az)
{
    XSetForeground(dpy, gc, 0xFF8800);   /* orange */
    Point2D p[PYR_V];
    for (int i = 0; i < PYR_V; i++) {
        Vec3 v = PYR_VERTS[i];
        v = rotX(v,ax); v = rotY(v,ay); v = rotZ(v,az);
        p[i] = project(v, cx, cy, 350.0);
    }
    for (int e = 0; e < PYR_E; e++)
        XDrawLine(dpy, buf, gc,
                  p[PYR_EDGES[e][0]].x, p[PYR_EDGES[e][0]].y,
                  p[PYR_EDGES[e][1]].x, p[PYR_EDGES[e][1]].y);
}

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