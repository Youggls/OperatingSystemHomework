#include "mmu.h"

.set PROTECT_MODE_CSEG, 0x8
.set PROTECT_MODE_DSEG, 0x10
.set CR0_PE_NO, 0x1

.global start
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
    movw    $msg1, %si
    movw    $0xbe2, %di
    movw    $24, %cx
    rep     movsb

    movw    $hellostring, %si
    movw    $0xc04, %di
    movw    $28, %cx
    rep     movsb

seta20.1:
    inb     $0x64, %al
    testb   $0x2, %al
    jnz     seta20.1

    movb   $0xd1, %al
    outb    %al, $0x64

seta20.2:
    inb     $0x64, %al
    testb   $0x2, %al
    jnz     seta20.2

    movb    $0xdf, %al
    outb    %al, $0x60

lgdtload:
    lgdt    gdtdesc

    movl    %cr0, %eax
    orl     $CR0_PE_NO, %eax
    movl    %eax, %cr0

    ljmp    $PROTECT_MODE_CSEG, $protcesg

    .code32

protcesg:
    movw    $PROTECT_MODE_DSEG, %ax
    movw    %ax, %ds
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs
    movw    %ax, %ss

    movl    $msg2, %esi
    movl    $0xb8d22, %edi
    movl    $62, %ecx
    rep     movsb

spin:
    jmp spin

.p2align 2
gdt:
    SEG_NULL
    SEG(STA_X|STA_R, 0x0, 0xffffffff)
    SEG(STA_W, 0x0, 0xffffffff)

gdtdesc:
    .word   0x17
    .long   gdt

msg1:
    .byte 'i', 0x7, 'n', 0x7, ' ', 0x7, 'r', 0x7, 'e', 0x7, 'a', 0x7, 'l', 0x7, ' ', 0x7, 'm', 0x7, 'o', 0x7, 'd', 0x7, 'e', 0x7

msg2:
    .byte 'i', 0x7, 'n', 0x7, ' ', 0x7, 'p', 0x7, 'r', 0x7, 'o', 0x7, 't', 0x7, 'e', 0x7, 'c', 0x7, 't', 0x7, ' ', 0x7, 'm', 0x7, 'o', 0x7, 'd', 0x7, 'e', 0x7

hellostring:
    .byte ':', 0xf, ' ', 0xc, 'L', 0xc, 'i', 0xc, 'P', 0xc, 'e', 0xc, 'i', 0xc, 'y', 0xc, 'a', 0xc, 'o', 0xc, '.', 0xc, 'H', 0xc, 'e', 0xc, 'l', 0xc, 'l', 0xc, 'o', 0xc
