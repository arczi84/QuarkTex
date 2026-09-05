#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")/.."
test_dir=$(mktemp -d /tmp/quarktex-resize.XXXXXX)
{
    cat tests/resize-support.h
    awk '/^void sync_window\(/ { copying=1 }
         /^static ULONG W3D_ClipBlit\(/ { copying=0 }
         copying { print }' Warp3D.library/Context.c
    cat tests/resize-main.c
} > "$test_dir/resize.c"
cc -std=c99 -Wall -Wextra -Werror "$test_dir/resize.c" -lm -o "$test_dir/resize"
"$test_dir/resize"
# Restore the old position-only detection: the resize assertions must fail.
sed 's/\&\& !resized//' "$test_dir/resize.c" > "$test_dir/original.c"
cc -std=c99 -Wall -Wextra -Werror "$test_dir/original.c" -lm -o "$test_dir/original"
if (ulimit -c 0; "$test_dir/original") > "$test_dir/negative.log" 2>&1; then
    echo 'FAIL: original position-only check passed' >&2
    exit 1
fi
echo 'PASS: resize regression detected with the original position-only check'
