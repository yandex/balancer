#include "longs.h"

#if defined(_darwin_)
//nothing to do?
#elif defined (__GNUC__) && defined (__i386__)
char *i386unp_bufba[256];
struct bufba_init {
    bufba_init() {
        asm volatile (
            "jmp 0f \n"
            /*ecx is an incremented ptr, eax=al is a first byte. out: edx:eax, ecx */
"i386unp_0:\n"
            "andl $0x7F,%%eax; xorl %%edx,%%edx; ret \n"
"i386unp_1:\n"
            "andl $0x3F,%%eax; movzbl (%%ecx),%%edx \n"
            "shll $8,%%eax; incl %%ecx; orl %%edx,%%eax \n"
            "xorl %%edx,%%edx; ret \n"
"i386unp_2:\n"
            "andl $0x1F,%%eax; movzwl (%%ecx),%%edx \n"
            "shll $16,%%eax; addl $2,%%ecx; orl %%edx,%%eax \n"
            "xorl %%edx,%%edx; ret \n"
"i386unp_3:\n"
            "andl $0x0F,%%eax; movzbl (%%ecx),%%edx \n"
            "shll $8,%%eax; orl %%edx,%%eax; shll $16,%%eax \n"
            "movzwl 1(%%ecx),%%edx; addl $3,%%ecx; orl %%edx,%%eax \n"
            "xorl %%edx,%%edx; ret \n"
"i386unp_4:\n"
            "movl %%eax,%%edx; movl (%%ecx),%%eax; andl $0x07,%%edx; addl $4,%%ecx; ret \n"
"i386unp_5:\n"
            "andl $0x03,%%eax; movzbl (%%ecx),%%edx \n"
            "shll $8,%%eax; orl %%eax,%%edx \n"
            "movl 1(%%ecx),%%eax; addl $5,%%ecx; ret \n"
"i386unp_6:\n"
            "andl $0x01,%%eax; movzwl (%%ecx),%%edx \n"
            "shll $16,%%eax; orl %%eax,%%edx \n"
            "movl 2(%%ecx),%%eax; addl $6,%%ecx; ret \n"
"i386unp_7:\n"
            "movzbl (%%ecx),%%eax; shll $16,%%eax \n"
            "movzwl 1(%%ecx),%%edx; orl %%eax,%%edx \n"
            "movl 3(%%ecx),%%eax; addl $7,%%ecx; ret \n"
"i386unp_8:\n"
            "movl (%%ecx),%%edx; movl 4(%%ecx),%%eax \n"
            "rorl $16, %%edx; addl $8, %%ecx; ret \n"
"0:\n"
            "leal i386unp_bufba,%%edx \n"
            "leal i386unp_0,%%eax; movl $0x80,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_1,%%eax; movl $0x40,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_2,%%eax; movl $0x20,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_3,%%eax; movl $0x10,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_4,%%eax; movl $0x08,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_5,%%eax; movl $0x04,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_6,%%eax; movl $0x02,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_7,%%eax; movl $0x01,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx; decl %%ecx; jnz 1b \n"
            "leal i386unp_8,%%eax; movl $0x01,%%ecx \n"
"1: "       "movl %%eax,(%%edx); addl $4,%%edx \n"
            :
            :
            : "eax", "ecx", "edx", "cc", "memory"
        );
    }
} bufba_init_now;

#elif defined (__GNUC__) && defined (__x86_64__)
char *i64unp_bufba[256];
struct bufba_init {
    bufba_init() {
        *((volatile char*)i64unp_bufba) = 0;

        asm volatile (
            "jmp 0f \n"
            /*WAS: eax=al is a first byte. out: edx:eax, ecx */
            /*rcx is an incremented ptr, rax=al is a first byte. out: rax, rcx */
"i64unp_0:\n"
            "andl $0x7F,%%eax; ret \n"
"i64unp_1:\n"
            "andl $0x3F,%%eax; movzbl (%%rcx),%%edx \n"
            "shll $8,%%eax; incq %%rcx; orl %%edx,%%eax \n"
            "ret \n"
"i64unp_2:\n"
            "andl $0x1F,%%eax; movzwl (%%rcx),%%edx \n"
            "shll $16,%%eax; addq $2,%%rcx; orl %%edx,%%eax \n"
            "ret \n"
"i64unp_3:\n"
            "andl $0x0F,%%eax; movzbl (%%rcx),%%edx \n"
            "shll $8,%%eax; orl %%edx,%%eax; shll $16,%%eax \n"
            "movzwl 1(%%rcx),%%edx; addq $3,%%rcx; orl %%edx,%%eax \n"
            "ret \n"
"i64unp_4:\n"
            "andl $0x07,%%eax; movl (%%rcx),%%edx \n"
            "shlq $32,%%rax; addq $4,%%rcx; orq %%rdx,%%rax \n"
            "ret \n"
"i64unp_5:\n"
            "andl $0x03,%%eax; movzbl (%%rcx),%%edx \n"
            "shll $8,%%eax; orl %%edx,%%eax \n"
            "shlq $32,%%rax; movl 1(%%rcx),%%edx \n"
            "addq $5,%%rcx; orq %%rdx,%%rax \n"
            "ret \n"
"i64unp_6:\n"
            "andl $0x01,%%eax; movzwl (%%rcx),%%edx \n"
            "shll $16,%%eax; orl %%edx,%%eax \n"
            "shlq $32,%%rax; movl 2(%%rcx),%%edx \n"
            "addq $6,%%rcx; orq %%rdx,%%rax \n"
            "ret \n"
"i64unp_7:\n"
            "movzbl (%%rcx),%%eax; shll $16,%%eax \n"
            "movzwl 1(%%rcx),%%edx; orl %%edx,%%eax \n"
            "shlq $32,%%rax; movl 3(%%rcx),%%edx \n"
            "addq $7,%%rcx; orq %%rdx,%%rax \n"
            "ret \n"
"i64unp_8:\n"
            "movl (%%rcx),%%eax; movl 4(%%rcx),%%edx \n"
            "rorl $16, %%eax; shlq $32,%%rax \n"
            "addq $8, %%rcx; orq %%rdx,%%rax \n"
            "ret \n"
"0:\n"
            "leaq i64unp_bufba,%%rdx \n"
            "leaq i64unp_0,%%rax; movl $0x80,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_1,%%rax; movl $0x40,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_2,%%rax; movl $0x20,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_3,%%rax; movl $0x10,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_4,%%rax; movl $0x08,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_5,%%rax; movl $0x04,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_6,%%rax; movl $0x02,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_7,%%rax; movl $0x01,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx; decl %%ecx; jnz 1b \n"
            "leaq i64unp_8,%%rax; movl $0x01,%%ecx \n"
"1: "       "movq %%rax,(%%rdx); addq $8,%%rdx \n"
            :
            :
            : "rax", "rcx", "rdx", "cc", "memory"
        );
    }
} bufba_init_now;

#elif defined(_MSC_VER)

#ifndef _win64_

static __declspec(naked)void i386unp_0() {
    _asm and eax, 0x7F _asm xor edx,edx _asm ret
}
static __declspec(naked)void i386unp_1() {
    _asm and eax, 0x3F _asm movzx edx, byte ptr [esi]
    _asm shl eax, 8 _asm inc esi _asm or eax, edx
    _asm xor edx,edx _asm ret
}
static __declspec(naked)void i386unp_2() {
    _asm and eax, 0x1F _asm movzx edx, word ptr[esi]
    _asm shl eax, 16 _asm add esi, 2 _asm or eax, edx
    _asm xor edx,edx _asm ret
}
static __declspec(naked)void i386unp_3() {
    _asm and eax, 0x0F _asm movzx edx, byte ptr[esi]
    _asm shl eax, 8 _asm or eax, edx _asm shl eax, 16
    _asm movzx edx, word ptr[esi+1] _asm add esi, 3 _asm or eax, edx
    _asm xor edx,edx _asm ret
}
static __declspec(naked)void i386unp_4() {
    _asm mov edx, eax _asm mov eax, [esi] _asm and edx, 0x07 _asm add esi,4 _asm ret
}
static __declspec(naked)void i386unp_5() {
    _asm and eax, 0x03 _asm movzx edx, byte ptr[esi]
    _asm shl eax, 8 _asm or edx, eax
    _asm mov eax, [esi+1] _asm add esi, 5 _asm ret
}
static __declspec(naked)void i386unp_6() {
    _asm and eax, 0x01 _asm movzx edx, word ptr[esi]
    _asm shl eax,16 _asm or edx, eax
    _asm mov eax, [esi+2] _asm add esi, 6 _asm ret
}
static __declspec(naked)void i386unp_7() {
    _asm movzx eax, byte ptr[esi] _asm shl eax, 16
    _asm movzx edx, word ptr[esi+1] _asm or edx, eax
    _asm mov eax, [esi+3] _asm add esi, 7 _asm ret
}
static __declspec(naked)void i386unp_8() {
    _asm mov edx,[esi] _asm mov eax,[esi+4]
    _asm ror edx, 16 _asm add esi,8 _asm ret
}

#define C(a) ((char*)&a)
char *i386unp_bufba[256] = {
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),
    C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0), C(i386unp_0),

    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),
    C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1), C(i386unp_1),

    C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2),
    C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2),
    C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2),
    C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2), C(i386unp_2),

    C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3),
    C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3), C(i386unp_3),

    C(i386unp_4), C(i386unp_4), C(i386unp_4), C(i386unp_4), C(i386unp_4), C(i386unp_4), C(i386unp_4), C(i386unp_4),

    C(i386unp_5), C(i386unp_5), C(i386unp_5), C(i386unp_5), C(i386unp_6), C(i386unp_6), C(i386unp_7), C(i386unp_8)
};

#endif
#endif
