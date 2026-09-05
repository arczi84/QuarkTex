#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gldefines.h"

typedef unsigned long ULONG;
typedef unsigned char UBYTE;
typedef void GLvoid;
struct BitMap { unsigned char data[4 * 3 * 4]; };
struct RastPort { struct BitMap *BitMap; };
struct RenderInfo { void *Memory; short BytesPerRow, pad; int RGBFormat; };
typedef struct { struct BitMap *drawregion; int yoffset; } W3D_Context;
typedef struct { int left, top, width, height; } W3D_Scissor;
#define __REGA0(x) x
#define __REGA1(x) x
#define __REGA2(x) x
#define __REGD1(x) x
#define LOG
#define W3D_SUCCESS 0
#define RGBFB_R8G8B8A8 42

static int width = 3, height = 2, fullscreen;
/* A nonzero translation exercises the guest-to-host pointer conversion. */
static ULONG memoffset = 4096;
static int finishes, reads, writes, swaps, allocations, frees, fail_alloc;
static int pack_depth, pack[4] = {8, 17, 5, 3}, saved_pack[4];
static const unsigned char host_pixels[24] = {
    10,11,12,13, 20,21,22,23, 30,31,32,33,
    40,41,42,43, 50,51,52,53, 60,61,62,63
};
static void *test_malloc(size_t size) {
    ++allocations;
    assert(size == sizeof(host_pixels));
    return fail_alloc ? NULL : malloc(size);
}
static void test_free(void *ptr) { assert(ptr); ++frees; free(ptr); }
#define malloc test_malloc
#define free test_free
static void _glFinish(void) { ++finishes; }
static void swapBuffers(void) { ++swaps; }
static void _glPushClientAttrib(int flags) {
    assert(flags == GL_CLIENT_PIXEL_STORE_BIT && pack_depth == 0);
    memcpy(saved_pack, pack, sizeof(pack));
    ++pack_depth;
}
static void _glPopClientAttrib(void) {
    assert(pack_depth == 1);
    memcpy(pack, saved_pack, sizeof(pack));
    --pack_depth;
}
static void _glPixelStorei(int key, int value) {
    assert(pack_depth == 1);
    switch (key) {
    case GL_PACK_ALIGNMENT: pack[0] = value; break;
    case GL_PACK_ROW_LENGTH: pack[1] = value; break;
    case GL_PACK_SKIP_ROWS: pack[2] = value; break;
    case GL_PACK_SKIP_PIXELS: pack[3] = value; break;
    default: assert(0);
    }
}
static void _glReadPixels(int x, int y, int w, int h, int fmt, int type, void *out) {
    assert(finishes > reads && pack_depth == 1);
    assert(pack[0] == 1 && !pack[1] && !pack[2] && !pack[3]);
    assert(!x && !y && w == 3 && h == 2);
    assert(fmt == GL_RGBA && type == GL_UNSIGNED_BYTE);
    memcpy((void *)((ULONG)out - memoffset), host_pixels, sizeof(host_pixels));
    ++reads;
}
static void InitRastPort(struct RastPort *rp) { rp->BitMap = NULL; }
static void p96WritePixelArray(struct RenderInfo *ri, int sx, int sy,
    struct RastPort *rp, int dx, int dy, int w, int h) {
    assert(!sx && !sy && !dx && dy >= 0 && dy < 4 && w == 3 && h == 1);
    assert(ri->BytesPerRow == 12 && !ri->pad && ri->RGBFormat == RGBFB_R8G8B8A8);
    assert(pack_depth == 0);
    memcpy(rp->BitMap->data + dy * 12, ri->Memory, 12);
    ++writes;
}
