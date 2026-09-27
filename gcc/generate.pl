#!/usr/bin/env perl
use strict;
use warnings;
my $mode = shift @ARGV // die "bridge|symbols|vectors required\n";
if ($mode eq 'vectors') {
    open my $fd, '<', 'Warp3D.library/Warp3D.fd' or die $!;
    my @names;
    while (<$fd>) { push @names, $1 if /^(W3D_\w+)\(/; }
    die "Unexpected vector count: " . @names . "\n" unless @names == 84;
    print "extern void $_(void);\n" for @names;
    print "const APTR qt_vectors[] = { (APTR)LibOpen, (APTR)LibClose, (APTR)LibExpunge, (APTR)LibReserved,\n";
    print "    (APTR)$_,\n" for @names;
    print "    (APTR)-1 };\n";
    exit;
}
open my $decl, '<', 'gl/gldeclarations.auto.h' or die $!;
my @funcs;
while (<$decl>) {
    next unless /^(\w+(?:\*)?)\s+_gl(\w+)\((.*)\);/;
    push @funcs, [$1, $2, $3];
}
die 'No GL declarations' unless @funcs;
if ($mode eq 'symbols') {
    print "static const char *const qt_gl_names[] = {\n";
    print "    \"gl$_->[1]\",\n" for @funcs;
    print "};\nULONG qt_gl_handles[sizeof(qt_gl_names) / sizeof(qt_gl_names[0])];\n";
    exit;
}
die "Unknown mode $mode" unless $mode eq 'bridge';
print ".text\n.even\n";
my @slots = (map("d$_", 1..7), map("a$_", 1..5));
for my $i (0..$#funcs) {
    my ($ret, $name, $args) = @{$funcs[$i]};
    print ".globl __gl$name\n__gl$name:\n";
    print "\tmovem.l d2-d7/a2-a6,-(sp)\n";
    if ($args =~ /\"fp/) {
        print "\tsubq.l #8,sp\n";
        my $word = 0;
        for my $arg (split /,\s*/, $args) {
            if ($arg =~ /__asm\(\"(fp\d)\"\)/) {
                die "Too many host argument words: $name" if $word+1 > $#slots;
                # Host doubles use low 32 bits followed by high 32 bits.
                print "\tfmove.d $1,(sp)\n\tmove.l 4(sp),$slots[$word]\n\tmove.l (sp),$slots[$word+1]\n";
                $word += 2;
            } else { ++$word; }
        }
        print "\taddq.l #8,sp\n";
    }
    print "\tmove.l _qt_gl_handles+" . ($i*4) . ",a0\n\tmoveq #102,d0\n\tjsr 0xf0ffc0\n";
    print "\tmovem.l (sp)+,d2-d7/a2-a6\n\trts\n";
}
for my $entry (['DLLopen',100], ['DLLfunc',101], ['DLLclose',103], ['memOffset',105]) {
    print ".globl _$entry->[0]\n_$entry->[0]:\n\tmovem.l d2-d7/a2-a6,-(sp)\n\tmoveq #$entry->[1],d0\n\tjsr 0xf0ffc0\n\tmovem.l (sp)+,d2-d7/a2-a6\n\trts\n";
}
for my $name (qw(createContext moveWindow freeContext swapBuffers logString)) {
    print ".globl _$name\n_$name:\n\tmovem.l d2-d7/a2-a6,-(sp)\n\tmove.l _qt_$name,a0\n\tmoveq #102,d0\n\tjsr 0xf0ffc0\n\tmovem.l (sp)+,d2-d7/a2-a6\n\trts\n";
}
