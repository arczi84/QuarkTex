#include <exec/types.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/dos.h>

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
void qt_debug(const char *text)
{
    ULONG n = 0;
    BPTR output;
    if (!DOSBase) return;
    output = Output();
    if (!output) return;
    while (text[n]) ++n;
    Write(output, (APTR)text, n);
}
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;
extern struct Library *P96Base;
extern void INIT_0_Warp3D(void), EXIT_0_Warp3D(void);
extern int qt_bridge_ready(void);
struct QTLibrary { struct Library lib; BPTR seglist; };
static const char qt_name[] = "Warp3D.library";
static const char qt_id[] = "$VER: Warp3D.library 4.3 (27.9.2026) QuarkTex 0.54 GCC\r\n";

static BPTR LibExpunge(struct QTLibrary *base __asm("a6"));
static struct QTLibrary *LibOpen(struct QTLibrary *base __asm("a6"))
{
    ++base->lib.lib_OpenCnt;
    base->lib.lib_Flags &= ~LIBF_DELEXP;
    return base;
}
static BPTR LibClose(struct QTLibrary *base __asm("a6"))
{
    if (base->lib.lib_OpenCnt) --base->lib.lib_OpenCnt;
    if (!base->lib.lib_OpenCnt && (base->lib.lib_Flags & LIBF_DELEXP)) return LibExpunge(base);
    return 0;
}
static BPTR LibExpunge(struct QTLibrary *base __asm("a6"))
{
    BPTR seg;
    ULONG size;
    UBYTE *start;
    if (base->lib.lib_OpenCnt) { base->lib.lib_Flags |= LIBF_DELEXP; return 0; }
    seg = base->seglist;
    Remove((struct Node *)base);
    EXIT_0_Warp3D();
    if (DOSBase) { CloseLibrary((struct Library *)DOSBase); DOSBase = 0; }
    size = base->lib.lib_NegSize + base->lib.lib_PosSize;
    start = (UBYTE *)base - base->lib.lib_NegSize;
    FreeMem(start, size);
    return seg;
}
static ULONG LibReserved(void) { return 0; }
static struct QTLibrary *LibInit(struct QTLibrary *base __asm("d0"),
    BPTR seglist __asm("a0"), struct ExecBase *sys __asm("a6"))
{
    SysBase = sys;
    base->lib.lib_Node.ln_Type = NT_LIBRARY;
    base->lib.lib_Node.ln_Name = (char *)qt_name;
    base->lib.lib_Flags = LIBF_SUMUSED | LIBF_CHANGED;
    base->lib.lib_Version = 4;
    base->lib.lib_Revision = 3;
    base->lib.lib_IdString = (char *)qt_id;
    base->seglist = seglist;
    DOSBase = (struct DosLibrary *)OpenLibrary("dos.library", 37);
    qt_debug("[QuarkTex GCC] Warp3D.library 4.3: initializing\n");
    INIT_0_Warp3D();
    if (!IntuitionBase || !GfxBase || !P96Base || !qt_bridge_ready()) {
        EXIT_0_Warp3D();
        qt_debug("[QuarkTex GCC] initialization failed\n");
        if (DOSBase) { CloseLibrary((struct Library *)DOSBase); DOSBase = 0; }
        FreeMem((UBYTE *)base - base->lib.lib_NegSize,
            base->lib.lib_NegSize + base->lib.lib_PosSize);
        return 0;
    }
    qt_debug("[QuarkTex GCC] initialized; depth-mask/blend independence fixed\n");
    return base;
}
#include "vectors.inc"
static const struct { ULONG size; const APTR *vectors; APTR data; APTR init; } qt_init = {
    sizeof(struct QTLibrary), qt_vectors, 0, (APTR)LibInit
};
const struct Resident RomTag = {
    RTC_MATCHWORD, (struct Resident *)&RomTag, (APTR)(&RomTag + 1),
    RTF_AUTOINIT, 4, NT_LIBRARY, 0, (char *)qt_name, (char *)qt_id, (APTR)&qt_init
};
