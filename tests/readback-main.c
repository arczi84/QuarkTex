/* Run actual driver functions with mocked host GL and Amiga bitmap APIs. */
int main(void) {
    struct BitMap first, second;
    W3D_Context context = { &first, 0 };
    W3D_Scissor scissor = { 0, 0, 3, 2 };
    const int original_pack[4] = {8, 17, 5, 3};
    int i, old_allocations;
    memset(&first, 0xa5, sizeof(first));
    memset(&second, 0xa5, sizeof(second));
    W3D_WaitIdle(&context);
    assert(finishes == 1 && reads == 1 && writes == 2);
    assert(allocations == 1 && frees == 1);
    assert(!memcmp(first.data, host_pixels + 12, 12));
    assert(!memcmp(first.data + 12, host_pixels, 12));
    for (i = 24; i < 48; ++i) assert(first.data[i] == 0xa5);
    assert(!pack_depth && !memcmp(pack, original_pack, sizeof(pack)));

    /* Switching destinations must use the new bitmap and the supplied offset. */
    assert(W3D_SetDrawRegion(&context, &second, 1, NULL) == W3D_SUCCESS);
    assert(!swaps && context.drawregion == &second && context.yoffset == 1);
    assert(window_syncs == 1 && !scissor_updates);
    W3D_WaitIdle(&context);
    assert(reads == 2 && writes == 4 && allocations == 2 && frees == 2);
    assert(!memcmp(second.data + 12, host_pixels + 12, 12));
    assert(!memcmp(second.data + 24, host_pixels, 12));
    for (i = 0; i < 12; ++i) assert(second.data[i] == 0xa5);
    for (i = 36; i < 48; ++i) assert(second.data[i] == 0xa5);
    assert(!pack_depth && !memcmp(pack, original_pack, sizeof(pack)));
    fullscreen = 1;
    W3D_SetDrawRegion(&context, &first, 0, &scissor);
    assert(swaps == 1 && context.drawregion == &first && !context.yoffset);
    assert(window_syncs == 2 && scissor_updates == 1);

    /* Missing destination, invalid geometry and allocation failure are no-ops
     * for bitmap copying; the explicit GL finish must still happen. */
    old_allocations = allocations;
    context.drawregion = NULL;
    W3D_WaitIdle(&context);
    context.drawregion = &first;
    width = 0; W3D_WaitIdle(&context);
    width = -1; W3D_WaitIdle(&context);
    width = 8192; W3D_WaitIdle(&context);
    width = 3; height = 0; W3D_WaitIdle(&context);
    height = -1; W3D_WaitIdle(&context);
    height = 0x7fffffff; W3D_WaitIdle(&context);
    assert(allocations == old_allocations);
    height = 2; fail_alloc = 1; W3D_WaitIdle(&context);
    assert(finishes == 10 && reads == 2 && writes == 4);
    assert(allocations == 3 && frees == 2);
    assert(!pack_depth && !memcmp(pack, original_pack, sizeof(pack)));
    puts("PASS: readback, RGBA, row flip, destination/offset, pack state, bounds, allocation failure");
    return 0;
}
