.globl start
start:
    .code16
    cli
    cld

    xorw    %ax, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %ss

    movw    $0xb800, %ax
    movw    %ax, %es
    movw    $msgl, %si
    movw    $0xb88, %di
    movw    $0x14, %cx
    rep     movsb

spin:
    jmp spin

msgl:
    .byte 'L',0xc,'i',0x9,'P',0xc,'e',0x9,'i',0xc,'y',0x9,'a',0xc
    .byte 'o',0x9,'.',0xc,'.',0x9

    .org 510
    .word   0xAA55
