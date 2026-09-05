int main(void) {
    struct Window win = {20, 30, 4, 20, 4, 4, 328, 280};
    W3D_Context ctx = {320, 256, {10, 15, 100, 50}};
    window=&win; left=24; top=50; width=320; height=256;
    sync_window(&ctx); assert(moves==0);
    win.Width=648; win.Height=504;
    sync_window(&ctx);
    assert(moves==1 && viewports==1 && identities==1 && clips==1);
    assert(width==640 && height==480 && ctx.width==640 && ctx.height==480);
    assert(hostW==640 && hostH==480 && clipY==415);
    /* Warp3D top-left/bottom-right must still map to the GL clip corners. */
    assert(fabsf(sx*tx+1)<0.00001f && fabsf(sy*ty-1)<0.00001f);
    assert(fabsf(sx*(640+tx)-1)<0.00001f && fabsf(sy*(480+ty)+1)<0.00001f);
    sync_window(&ctx); assert(moves==1);
    win.LeftEdge+=17; sync_window(&ctx);
    assert(moves==2 && viewports==1 && left==41);
    win.Width=328; win.Height=280; sync_window(&ctx);
    assert(moves==3 && viewports==2 && ctx.width==320 && ctx.height==256 && clipY==191);
    win.Width=8; sync_window(&ctx); assert(moves==3 && width==320);
    win.Width=808; fullscreen=1; sync_window(&ctx); assert(moves==3);
    fullscreen=0; window=NULL; sync_window(&ctx); assert(moves==3);
    window=&win; sync_window(NULL); assert(moves==3);
    puts("PASS: grow, shrink, move, unchanged, empty, fullscreen, coordinate mapping and scissor");
    return 0;
}
