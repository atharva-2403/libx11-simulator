#ifndef SHAPES_H
#define SHAPES_H

#include <X11/Xlib.h>
#include <math.h>

/* ── Common types ── */
typedef struct { double x, y, z; } Vec3;
typedef struct { int x, y; }       Point2D;

/* ── Math utilities ── */
#define PI 3.14159265358979323846

static inline Vec3 rotX(Vec3 p, double a) {
    return (Vec3){ p.x, p.y*cos(a)-p.z*sin(a), p.y*sin(a)+p.z*cos(a) };
}

static inline Vec3 rotY(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)+p.z*sin(a), p.y, -p.x*sin(a)+p.z*cos(a) };
}

static inline Vec3 rotZ(Vec3 p, double a) {
    return (Vec3){ p.x*cos(a)-p.y*sin(a), p.x*sin(a)+p.y*cos(a), p.z };
}

/* ── Perspective projection ── */
static inline Point2D project(Vec3 p, double cx, double cy, double fov) {
    double z = p.z + fov;
    if (z < 0.001) z = 0.001;
    double s = fov / z;
    return (Point2D){ (int)(p.x*s + cx), (int)(p.y*s + cy) };
}

/* ── Shape types ── */
typedef enum {
    SHAPE_CUBE,
    SHAPE_PYRAMID,
    SHAPE_SPHERE
} ShapeType;

/* ── Cube constants ── */
#define CUBE_V 8
#define CUBE_E 12
extern const Vec3 CUBE_VERTS[CUBE_V];
extern const int CUBE_EDGES[CUBE_E][2];

/* ── Pyramid constants ── */
#define PYR_V 5
#define PYR_E 8
extern const Vec3 PYR_VERTS[PYR_V];
extern const int PYR_EDGES[PYR_E][2];

/* ── Sphere constants ── */
#define SPH_LAT  10    /* latitude rings  */
#define SPH_LON  12    /* longitude lines  */
#define SPH_R    90.0

/* ── Drawing functions ── */
void draw_cube(Display *dpy, Pixmap buf, GC gc,
               double cx, double cy, double ax, double ay, double az);

void draw_pyramid(Display *dpy, Pixmap buf, GC gc,
                  double cx, double cy, double ax, double ay, double az);

void draw_sphere(Display *dpy, Pixmap buf, GC gc,
                 double cx, double cy, double ax, double ay, double az);

#endif /* SHAPES_H */