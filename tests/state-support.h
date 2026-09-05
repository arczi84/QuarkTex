#include <stdio.h>
#include "gldefines.h"
typedef unsigned long ULONG;
typedef struct { ULONG state; } W3D_Context;
#define LOG
#define __asm(x)
static int host_blend, host_depth_write, blend_calls;
static void _glEnable(unsigned int cap) {
    if (cap == GL_BLEND) { host_blend = 1; ++blend_calls; }
}
static void _glDisable(unsigned int cap) {
    if (cap == GL_BLEND) { host_blend = 0; ++blend_calls; }
}
static void _glDepthMask(int enabled) { host_depth_write = enabled; }
static void _glShadeModel(unsigned int mode) { (void)mode; }
