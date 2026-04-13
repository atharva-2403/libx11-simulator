#ifndef RENDERER_H
#define RENDERER_H

#include "shapes.h"

/* ── Renderer constants ── */
#define INITIAL_WIN_W   900
#define INITIAL_WIN_H   600
#define FPS     60

/* ── Label helper ── */
void label(Display *dpy, Pixmap buf, GC gc, int x, int y,
           const char *text, unsigned long color);

/* ── Main function declaration ── */
int main(void);

#endif /* RENDERER_H */