QuarkTex 0.54

QuarkTex is a 3D graphics hardware virualization solution first released in 2003.
It transforms calls to the Warp3D API inside an emulated AmigaOS to native OpenGL calls for a host Windows system.

Developed by Robert Konrad.
Author of the 0.54 update: Artur Jarosik.
Released under the LGPL license.

Changes in 0.54 (from 0.53):
- Fix window resizing: synchronize the host drawable, viewport and scissor
  coordinates when the Amiga window changes size or position.
- Copy the host back buffer to the Amiga bitmap on W3D_WaitIdle.
- Fix W3D_ZBUFFERUPDATE changing blending state instead of only the depth mask.

This maintenance release preserves the existing Warp3D API and host bridge ABI.
