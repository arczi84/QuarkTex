# W3D_SetWrapMode NULL border color (2026-09-26)

Classic MiniGL calls W3D_SetWrapMode with A2=NULL. The previous QuarkTex
implementation dereferenced bordercolor unconditionally, producing Enforcer
reads at 0, 4, 8 and 12. The test-WB binary's first read was text offset 0x1726.
Only update the stored border color when a non-NULL color is supplied;
otherwise preserve it (AllocTexObj initializes it to zero). Wrap settings and
the existing host color conversion are unchanged.

Run `python3 tests/test-wrap-null.py`: compiles the actual function with mock
host calls, tests NULL/non-NULL/NULL, and verifies that removing the guard
reproduces a NULL-pointer error under UBSan. State, readback and resize tests
also passed.

Backup: `backups/wrap-null-XU0o50/` contains original Texture.c, build-gcc and
the test Workbench's original Warp3D.library. Original library SHA256:
86430a1cc33dc4be2bd76bf7aa8d09e47203ec9a1af86ce4ce259b6a08c131ec.

The first wrap-only runtime test exposed the same NULL-color bug in
W3D_SetTexEnv (offset 0x15de). That function also allocated and leaked 16 bytes
on each call. It now copies a supplied color directly into texture state and
retains it for NULL. The host test covers both functions and both negative
controls. This changes neither the existing channel ordering nor mode mapping.

Candidate including both fixes: `build-wrap-null/Warp3D.library`, SHA256:
bf355f0b0bcd08ed147049b7fdc19dfd1b6f6d506425c711f472aee4ba2d7935.
RC7 is no longer installed. Texture.c alone was rebuilt with
/opt/amiga-gcc16-rc11/bin/m68k-amigaos-gcc using gcc/build.sh's existing flags;
all other object files were copied from build-gcc, then linked with RC11.
The original version string is retained; identify this candidate by its hash.
No game binary, MiniGL library, or Windows DLL was changed.

Installed only into D:\Amiga\MK3-WB-disks\System-ClassicWBP96\Libs\Warp3D.library
for a fresh WinUAE boot. Flyby test folder:
D:\dev\uae-tests\ue1-classic-31KpZK.

WinUAE validation: user confirmed no Enforcer hit with both fixes. Full flyby
completed and exited normally at 09:33:47. Result 110.21 FPS, wall 108.80 FPS,
worst frame 74 ms. Earlier unpatched run: 80.46 / 79.38 FPS, but included
Enforcer hits, so this is not a clean renderer-performance comparison.
The previous shared uae-tests/test launcher was restored after completion.
