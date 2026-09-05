#include <assert.h>
#include <math.h>
#include <stdio.h>
typedef struct { int left, top, width, height; } Scissor;
typedef struct { int width, height; Scissor scissor; } W3D_Context;
struct Window { int LeftEdge, TopEdge, BorderLeft, BorderTop, BorderRight, BorderBottom, Width, Height; };
static struct Window *window;
static int fullscreen, left, top, width, height;
static int moves, viewports, identities, clips, hostW, hostH, clipY;
static float sx, sy, tx, ty;
#define GL_MODELVIEW 0x1700
static void moveWindow(int x, int y, int w, int h) {
    assert(x == left && y == top); hostW=w; hostH=h; ++moves;
}
static void _glViewport(int x, int y, int w, int h) {
    assert(x == 0 && y == 0 && w == hostW && h == hostH); ++viewports;
}
static void _glMatrixMode(int mode) { assert(mode == GL_MODELVIEW); }
static void _glLoadIdentity(void) { ++identities; }
static void _glScalef(float x, float y, float z) { sx=x; sy=y; assert(z==1); }
static void _glTranslatef(float x, float y, float z) { tx=x; ty=y; assert(z==0); }
static void _glScissor(int x, int y, int w, int h) {
    assert(x==10 && w==100 && h==50); clipY=y; ++clips;
}
