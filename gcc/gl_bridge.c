#include "../gl/gl.h"

ULONG qt_createContext, qt_moveWindow, qt_freeContext, qt_swapBuffers, qt_logString;
long memoffset;
static ULONG qt_host, qt_gl;
extern void qt_debug(const char *text);
extern ULONG DLLopen(char *name __asm("a0"));
extern ULONG DLLfunc(ULONG dll __asm("d1"), const char *name __asm("a0"));
extern ULONG DLLclose(ULONG dll __asm("d1"));
extern ULONG memOffset(void);
#include "gl_symbols.inc"

int qt_bridge_ready(void)
{
    unsigned int i;
    if (!qt_host || !qt_gl || !qt_createContext || !qt_moveWindow ||
        !qt_freeContext || !qt_swapBuffers) return 0;
    for (i = 0; i < sizeof(qt_gl_names)/sizeof(qt_gl_names[0]); ++i)
        if (!qt_gl_handles[i]) {
            qt_debug("[QuarkTex GCC] missing host symbol: ");
            qt_debug(qt_gl_names[i]); qt_debug("\n");
            return 0;
        }
    return 1;
}

void glInit(void)
{
    unsigned int i;
    memoffset = memOffset();
    qt_host = DLLopen("alib\\QuarkTex.alib");
    if (!qt_host) qt_host = DLLopen("winuae_dll\\QuarkTex.alib");
    if (!qt_host) qt_host = DLLopen("QuarkTex.alib");
    if (!qt_host) return;
    qt_createContext = DLLfunc(qt_host, "createContext");
    qt_moveWindow = DLLfunc(qt_host, "moveWindow");
    qt_freeContext = DLLfunc(qt_host, "freeContext");
    qt_swapBuffers = DLLfunc(qt_host, "swapBuffers");
    qt_logString = DLLfunc(qt_host, "logString");
    qt_gl = DLLopen("opengl32.dll");
    if (!qt_gl) return;
    for (i = 0; i < sizeof(qt_gl_names)/sizeof(qt_gl_names[0]); ++i)
        qt_gl_handles[i] = DLLfunc(qt_gl, qt_gl_names[i]);
}

void glExit(void)
{
    if (qt_gl) DLLclose(qt_gl);
    if (qt_host) DLLclose(qt_host);
    qt_gl = qt_host = 0;
}
