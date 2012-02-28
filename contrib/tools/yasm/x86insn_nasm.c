/* ANSI-C code produced by genperf */
/* Command-line: genperf x86insn_nasm.gperf x86insn_nasm.c */
#line 10 "x86insn_nasm.gperf"
struct insnprefix_parse_data;
static const struct insnprefix_parse_data *
insnprefix_nasm_find(const char *key, size_t len)
{
  static const struct insnprefix_parse_data pd[1478] = {
#line 308 "x86insn_nasm.gperf"
    {"fcomp",	fcom_insn,	6,	NONE,	0xD8,	0x03,	0,	0,	CPU_FPU,	0,	0},
#line 398 "x86insn_nasm.gperf"
    {"fucompp",	twobyte_insn,	1,	NONE,	0xDA,	0xE9,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 187 "x86insn_nasm.gperf"
    {"comordps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1154 "x86insn_nasm.gperf"
    {"vcomisd",	avx_xmm_xmm64_insn,	2,	NONE,	0x66,	0x2F,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 101 "x86insn_nasm.gperf"
    {"cmpltpd",	ssecmp_128_insn,	3,	NONE,	0x01,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 1429 "x86insn_nasm.gperf"
    {"vpunpckhwd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x69,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 823 "x86insn_nasm.gperf"
    {"protw",	sse5prot_insn,	3,	NONE,	0x01,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1337 "x86insn_nasm.gperf"
    {"vpcmpeqw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x75,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1082 "x86insn_nasm.gperf"
    {"vcmpneq_ospd",	ssecmp_128_insn,	3,	NONE,	0x1C,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1266 "x86insn_nasm.gperf"
    {"vmovapd",	movau_insn,	6,	NONE,	0x66,	0x28,	0x01,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 283 "x86insn_nasm.gperf"
    {"dppd",	sse4imm_insn,	2,	NONE,	0x41,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1236 "x86insn_nasm.gperf"
    {"vfnmsub213sd",	vfma_sd_insn,	2,	NONE,	0xAF,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 392 "x86insn_nasm.gperf"
    {"fsubrp",	farithp_insn,	3,	NONE,	0xE0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 841 "x86insn_nasm.gperf"
    {"pslld",	pshift_insn,	4,	NONE,	0xF2,	0x72,	0x06,	0,	CPU_MMX,	0,	0},
#line 773 "x86insn_nasm.gperf"
    {"pminub",	mmxsse2_insn,	2,	NONE,	0xDA,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 892 "x86insn_nasm.gperf"
    {"repz",	NULL,	X86_LOCKREP>>8,	0xF3,	0,	0,	0,	0,	0,	0,	0},
#line 882 "x86insn_nasm.gperf"
    {"rcr",	shift_insn,	16,	NONE,	0x03,	0,	0,	0,	0,	0,	0},
#line 1381 "x86insn_nasm.gperf"
    {"vpmovsxbw",	sse4m64_insn,	2,	NONE,	0x20,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 895 "x86insn_nasm.gperf"
    {"retn",	retnf_insn,	6,	NONE,	0xC2,	0,	0,	0,	0,	0,	0},
#line 202 "x86insn_nasm.gperf"
    {"comugepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1299 "x86insn_nasm.gperf"
    {"vmulps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x59,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 979 "x86insn_nasm.gperf"
    {"subsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5C,	0,	0,	CPU_SSE2,	0,	0},
#line 539 "x86insn_nasm.gperf"
    {"movsd",	movsd_insn,	5,	NONE,	0,	0,	0,	0,	CPU_386,	0,	0},
#line 1181 "x86insn_nasm.gperf"
    {"vextractps",	extractps_insn,	2,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1117 "x86insn_nasm.gperf"
    {"vcmpnless",	ssecmp_32_insn,	4,	NONE,	0x06,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 669 "x86insn_nasm.gperf"
    {"pcomneqq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1080 "x86insn_nasm.gperf"
    {"vcmpneq_oqsd",	ssecmp_64_insn,	4,	NONE,	0x0C,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 376 "x86insn_nasm.gperf"
    {"frndint",	twobyte_insn,	1,	NONE,	0xD9,	0xFC,	0,	0,	CPU_FPU,	0,	0},
#line 644 "x86insn_nasm.gperf"
    {"pcomgtud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1087 "x86insn_nasm.gperf"
    {"vcmpneq_usps",	ssecmp_128_insn,	3,	NONE,	0x14,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 158 "x86insn_nasm.gperf"
    {"comltpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 441 "x86insn_nasm.gperf"
    {"jg",	jcc_insn,	9,	NONE,	0x0F,	0,	0,	0,	0,	0,	0},
#line 690 "x86insn_nasm.gperf"
    {"pcomud",	sse5com_insn,	1,	NONE,	0x6E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 26 "x86insn_nasm.gperf"
    {"addsubps",	xmm_xmm128_insn,	2,	NONE,	0xF2,	0xD0,	0,	0,	CPU_SSE3,	0,	0},
#line 331 "x86insn_nasm.gperf"
    {"fisub",	fiarith_insn,	2,	NONE,	0x04,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 156 "x86insn_nasm.gperf"
    {"comlesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 948 "x86insn_nasm.gperf"
    {"sgdt",	twobytemem_insn,	1,	NONE,	0x00,	0x0F,	0x01,	0,	CPU_286,	CPU_Priv,	0},
#line 735 "x86insn_nasm.gperf"
    {"phminposuw",	sse4_insn,	2,	NONE,	0x41,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 254 "x86insn_nasm.gperf"
    {"cvtph2ps",	cvtph2ps_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 566 "x86insn_nasm.gperf"
    {"out",	out_insn,	12,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 246 "x86insn_nasm.gperf"
    {"cpuid",	twobyte_insn,	1,	NONE,	0x0F,	0xA2,	0,	0,	CPU_486,	0,	0},
#line 1053 "x86insn_nasm.gperf"
    {"vcmpgess",	ssecmp_32_insn,	4,	NONE,	0x0D,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 858 "x86insn_nasm.gperf"
    {"psubusw",	mmxsse2_insn,	2,	NONE,	0xD9,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1031 "x86insn_nasm.gperf"
    {"vcmpeq_usps",	ssecmp_128_insn,	3,	NONE,	0x18,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 528 "x86insn_nasm.gperf"
    {"movntdq",	movnt_insn,	2,	NONE,	0x66,	0xE7,	0,	0,	CPU_SSE2,	0,	0},
#line 1113 "x86insn_nasm.gperf"
    {"vcmpnle_uqss",	ssecmp_32_insn,	4,	NONE,	0x16,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1017 "x86insn_nasm.gperf"
    {"vblendvpd",	avx_sse4xmm0_insn,	2,	NONE,	0x4B,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1136 "x86insn_nasm.gperf"
    {"vcmpsd",	cmpsd_insn,	5,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 388 "x86insn_nasm.gperf"
    {"fstsw",	fstsw_insn,	2,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 953 "x86insn_nasm.gperf"
    {"shufpd",	xmm_xmm128_imm_insn,	1,	NONE,	0x66,	0xC6,	0,	0,	CPU_SSE2,	0,	0},
#line 107 "x86insn_nasm.gperf"
    {"cmpneqsd",	ssecmp_64_insn,	4,	NONE,	0x04,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 51 "x86insn_nasm.gperf"
    {"call",	call_insn,	26,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 531 "x86insn_nasm.gperf"
    {"movntpd",	movnt_insn,	2,	NONE,	0x66,	0x2B,	0,	0,	CPU_SSE2,	0,	0},
#line 1091 "x86insn_nasm.gperf"
    {"vcmpneqps",	ssecmp_128_insn,	3,	NONE,	0x04,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1415 "x86insn_nasm.gperf"
    {"vpsrlq",	vpshift_insn,	4,	NONE,	0xD3,	0x73,	0x02,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 374 "x86insn_nasm.gperf"
    {"frczsd",	sse5two64_insn,	2,	NONE,	0x13,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 844 "x86insn_nasm.gperf"
    {"psllw",	pshift_insn,	4,	NONE,	0xF1,	0x71,	0x06,	0,	CPU_MMX,	0,	0},
#line 1039 "x86insn_nasm.gperf"
    {"vcmpfalse_osps",	ssecmp_128_insn,	3,	NONE,	0x1B,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 384 "x86insn_nasm.gperf"
    {"fst",	fst_insn,	3,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 506 "x86insn_nasm.gperf"
    {"minps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x5D,	0,	0,	CPU_SSE,	0,	0},
#line 31 "x86insn_nasm.gperf"
    {"aesimc",	aesimc_insn,	1,	NONE,	0x38,	0xDB,	0,	0,	CPU_AES,	0,	0},
#line 452 "x86insn_nasm.gperf"
    {"jng",	jcc_insn,	9,	NONE,	0x0E,	0,	0,	0,	0,	0,	0},
#line 1377 "x86insn_nasm.gperf"
    {"vpminuw",	ssse3_insn,	3,	NONE,	0x3A,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 977 "x86insn_nasm.gperf"
    {"subpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5C,	0,	0,	CPU_SSE2,	0,	0},
#line 215 "x86insn_nasm.gperf"
    {"comultps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 1440 "x86insn_nasm.gperf"
    {"vroundss",	sse4m32imm_insn,	4,	NONE,	0x0A,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 784 "x86insn_nasm.gperf"
    {"pmovzxbq",	sse4m16_insn,	2,	NONE,	0x32,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 859 "x86insn_nasm.gperf"
    {"psubw",	mmxsse2_insn,	2,	NONE,	0xF9,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1466 "x86insn_nasm.gperf"
    {"wait",	onebyte_insn,	1,	NONE,	0x9B,	0,	0,	0,	0,	0,	0},
#line 555 "x86insn_nasm.gperf"
    {"mulss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x59,	0,	0,	CPU_SSE,	0,	0},
#line 563 "x86insn_nasm.gperf"
    {"or",	arith_insn,	22,	NONE,	0x08,	0x01,	0,	0,	0,	0,	0},
#line 987 "x86insn_nasm.gperf"
    {"sysexit",	twobyte_insn,	1,	NONE,	0x0F,	0x35,	0,	NOT_64,	CPU_686,	CPU_Priv,	0},
#line 1032 "x86insn_nasm.gperf"
    {"vcmpeq_ussd",	ssecmp_64_insn,	4,	NONE,	0x18,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 485 "x86insn_nasm.gperf"
    {"lodsb",	onebyte_insn,	1,	NONE,	0xAC,	0x00,	0,	0,	0,	0,	0},
#line 714 "x86insn_nasm.gperf"
    {"pfrcpit1",	now3d_insn,	1,	NONE,	0xA6,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 29 "x86insn_nasm.gperf"
    {"aesenc",	aes_insn,	2,	NONE,	0x38,	0xDC,	0,	0,	CPU_AVX,	0,	0},
#line 361 "x86insn_nasm.gperf"
    {"fnmsubsd",	sse5arith64_insn,	8,	NONE,	0x1B,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 703 "x86insn_nasm.gperf"
    {"pfacc",	now3d_insn,	1,	NONE,	0xAE,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 1349 "x86insn_nasm.gperf"
    {"vpextrb",	pextrb_insn,	3,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 468 "x86insn_nasm.gperf"
    {"lar",	bsfr_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_286,	CPU_Prot,	0},
#line 749 "x86insn_nasm.gperf"
    {"pmacsdd",	sse5pmacs_insn,	1,	NONE,	0x9E,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 237 "x86insn_nasm.gperf"
    {"comunless",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 333 "x86insn_nasm.gperf"
    {"fld",	fld_insn,	4,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 688 "x86insn_nasm.gperf"
    {"pcomtruew",	sse5comcc_insn,	1,	NONE,	0x4D,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 602 "x86insn_nasm.gperf"
    {"pcmpeqb",	mmxsse2_insn,	2,	NONE,	0x74,	0,	0,	0,	CPU_MMX,	0,	0},
#line 287 "x86insn_nasm.gperf"
    {"extractps",	extractps_insn,	2,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_SSE41,	0},
#line 1404 "x86insn_nasm.gperf"
    {"vpsignb",	ssse3_insn,	3,	NONE,	0x08,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1197 "x86insn_nasm.gperf"
    {"vfmaddsub213ps",	vfma_ps_insn,	2,	NONE,	0xA6,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 354 "x86insn_nasm.gperf"
    {"fninit",	twobyte_insn,	1,	NONE,	0xDB,	0xE3,	0,	0,	CPU_FPU,	0,	0},
#line 318 "x86insn_nasm.gperf"
    {"ffreep",	ffree_insn,	1,	NONE,	0xDF,	0,	0,	0,	CPU_686,	CPU_FPU,	CPU_Undoc},
#line 762 "x86insn_nasm.gperf"
    {"pmaddwd",	mmxsse2_insn,	2,	NONE,	0xF5,	0,	0,	0,	CPU_MMX,	0,	0},
#line 30 "x86insn_nasm.gperf"
    {"aesenclast",	aes_insn,	2,	NONE,	0x38,	0xDD,	0,	0,	CPU_AVX,	0,	0},
#line 787 "x86insn_nasm.gperf"
    {"pmovzxwd",	sse4m64_insn,	2,	NONE,	0x33,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 682 "x86insn_nasm.gperf"
    {"pcomtrued",	sse5comcc_insn,	1,	NONE,	0x4E,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1119 "x86insn_nasm.gperf"
    {"vcmpnlt_uqps",	ssecmp_128_insn,	3,	NONE,	0x15,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1477 "x86insn_nasm.gperf"
    {"xcryptofb",	padlock_insn,	1,	NONE,	0xE8,	0xF3,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 556 "x86insn_nasm.gperf"
    {"mwait",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC9,	0,	CPU_SSE3,	0,	0},
#line 1309 "x86insn_nasm.gperf"
    {"vpabsw",	avx_ssse3_2op_insn,	1,	NONE,	0x1D,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1452 "x86insn_nasm.gperf"
    {"vsubsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 623 "x86insn_nasm.gperf"
    {"pcomeqw",	sse5comcc_insn,	1,	NONE,	0x4D,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 510 "x86insn_nasm.gperf"
    {"montmul",	padlock_insn,	1,	NONE,	0xC0,	0xF3,	0xA6,	0,	CPU_PadLock,	0,	0},
#line 792 "x86insn_nasm.gperf"
    {"pmulhrwa",	now3d_insn,	1,	NONE,	0xB7,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 620 "x86insn_nasm.gperf"
    {"pcomequd",	sse5comcc_insn,	1,	NONE,	0x6E,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1288 "x86insn_nasm.gperf"
    {"vmovss",	movss_insn,	4,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1183 "x86insn_nasm.gperf"
    {"vfmadd132ps",	vfma_ps_insn,	2,	NONE,	0x98,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1388 "x86insn_nasm.gperf"
    {"vpmovzxdq",	sse4m64_insn,	2,	NONE,	0x35,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 165 "x86insn_nasm.gperf"
    {"comneqps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 739 "x86insn_nasm.gperf"
    {"phsubsw",	ssse3_insn,	3,	NONE,	0x07,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 945 "x86insn_nasm.gperf"
    {"sets",	setcc_insn,	1,	NONE,	0x08,	0,	0,	0,	CPU_386,	0,	0},
#line 150 "x86insn_nasm.gperf"
    {"comgtsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 410 "x86insn_nasm.gperf"
    {"hsubpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x7D,	0,	0,	CPU_SSE3,	0,	0},
#line 991 "x86insn_nasm.gperf"
    {"ucomiss",	xmm_xmm32_insn,	4,	NONE,	0x00,	0x2E,	0,	0,	CPU_SSE,	0,	0},
#line 242 "x86insn_nasm.gperf"
    {"comunordpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 1250 "x86insn_nasm.gperf"
    {"vmaskmovdqu",	maskmovdqu_insn,	1,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 193 "x86insn_nasm.gperf"
    {"comss",	sse5com32_insn,	2,	NONE,	0x2E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1103 "x86insn_nasm.gperf"
    {"vcmpngt_uqps",	ssecmp_128_insn,	3,	NONE,	0x1A,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 281 "x86insn_nasm.gperf"
    {"divsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5E,	0,	0,	CPU_SSE2,	0,	0},
#line 662 "x86insn_nasm.gperf"
    {"pcomltuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 45 "x86insn_nasm.gperf"
    {"bsr",	bsfr_insn,	3,	NONE,	0xBD,	0,	0,	0,	CPU_386,	0,	0},
#line 174 "x86insn_nasm.gperf"
    {"comngtpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 995 "x86insn_nasm.gperf"
    {"unpckhpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x15,	0,	0,	CPU_SSE2,	0,	0},
#line 1399 "x86insn_nasm.gperf"
    {"vpsadbw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xF6,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 946 "x86insn_nasm.gperf"
    {"setz",	setcc_insn,	1,	NONE,	0x04,	0,	0,	0,	CPU_386,	0,	0},
#line 1482 "x86insn_nasm.gperf"
    {"xorps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x57,	0,	0,	CPU_SSE,	0,	0},
#line 518 "x86insn_nasm.gperf"
    {"movdqa",	movau_insn,	6,	NONE,	0x66,	0x6F,	0x10,	0,	CPU_SSE2,	0,	0},
#line 726 "x86insn_nasm.gperf"
    {"phaddubd",	sse5two_insn,	1,	NONE,	0x52,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 368 "x86insn_nasm.gperf"
    {"fpatan",	twobyte_insn,	1,	NONE,	0xD9,	0xF3,	0,	0,	CPU_FPU,	0,	0},
#line 1341 "x86insn_nasm.gperf"
    {"vpcmpgtd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1445 "x86insn_nasm.gperf"
    {"vsqrtpd",	avx_xmm_xmm128_insn,	2,	NONE,	0x66,	0x51,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 93 "x86insn_nasm.gperf"
    {"cmpeqpd",	ssecmp_128_insn,	3,	NONE,	0x00,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 965 "x86insn_nasm.gperf"
    {"sqrtss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x51,	0,	0,	CPU_SSE,	0,	0},
#line 109 "x86insn_nasm.gperf"
    {"cmpnlepd",	ssecmp_128_insn,	3,	NONE,	0x06,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 402 "x86insn_nasm.gperf"
    {"fxrstor",	twobytemem_insn,	1,	NONE,	0x01,	0x0F,	0xAE,	0,	CPU_686,	CPU_FPU,	0},
#line 1280 "x86insn_nasm.gperf"
    {"vmovntdq",	movnt_insn,	2,	NONE,	0x66,	0xE7,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 418 "x86insn_nasm.gperf"
    {"insd",	onebyte_insn,	1,	NONE,	0x6D,	0x20,	0,	0,	CPU_386,	0,	0},
#line 647 "x86insn_nasm.gperf"
    {"pcomgtw",	sse5comcc_insn,	1,	NONE,	0x4D,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 734 "x86insn_nasm.gperf"
    {"phaddwq",	sse5two_insn,	1,	NONE,	0x47,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 540 "x86insn_nasm.gperf"
    {"movshdup",	xmm_xmm128_insn,	2,	NONE,	0xF3,	0x16,	0,	0,	CPU_SSE3,	0,	0},
#line 1106 "x86insn_nasm.gperf"
    {"vcmpngtpd",	ssecmp_128_insn,	3,	NONE,	0x0A,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 338 "x86insn_nasm.gperf"
    {"fldl2t",	twobyte_insn,	1,	NONE,	0xD9,	0xE9,	0,	0,	CPU_FPU,	0,	0},
#line 1369 "x86insn_nasm.gperf"
    {"vpmaxub",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDE,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 780 "x86insn_nasm.gperf"
    {"pmovsxdq",	sse4m64_insn,	2,	NONE,	0x25,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1074 "x86insn_nasm.gperf"
    {"vcmpltpd",	ssecmp_128_insn,	3,	NONE,	0x01,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 907 "x86insn_nasm.gperf"
    {"rsts",	cyrixsmm_insn,	1,	NONE,	0x7D,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 99 "x86insn_nasm.gperf"
    {"cmplesd",	ssecmp_64_insn,	4,	NONE,	0x02,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 203 "x86insn_nasm.gperf"
    {"comugeps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 767 "x86insn_nasm.gperf"
    {"pmaxub",	mmxsse2_insn,	2,	NONE,	0xDE,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 102 "x86insn_nasm.gperf"
    {"cmpltps",	ssecmp_128_insn,	3,	NONE,	0x01,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1011 "x86insn_nasm.gperf"
    {"vandnpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x55,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 58 "x86insn_nasm.gperf"
    {"clgi",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xDD,	0,	CPU_SVM,	0,	0},
#line 695 "x86insn_nasm.gperf"
    {"permpd",	sse5arith_insn,	4,	NONE,	0x21,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 115 "x86insn_nasm.gperf"
    {"cmpnltsd",	ssecmp_64_insn,	4,	NONE,	0x05,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 1002 "x86insn_nasm.gperf"
    {"vaddss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x58,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 472 "x86insn_nasm.gperf"
    {"lea",	lea_insn,	3,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 68 "x86insn_nasm.gperf"
    {"cmovg",	cmovcc_insn,	3,	NONE,	0x0F,	0,	0,	0,	CPU_686,	0,	0},
#line 1361 "x86insn_nasm.gperf"
    {"vpinsrd",	pinsrd_insn,	2,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1178 "x86insn_nasm.gperf"
    {"verr",	prot286_insn,	1,	NONE,	0x04,	0x00,	0,	0,	CPU_286,	CPU_Prot,	0},
#line 1116 "x86insn_nasm.gperf"
    {"vcmpnlesd",	ssecmp_64_insn,	4,	NONE,	0x06,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1107 "x86insn_nasm.gperf"
    {"vcmpngtps",	ssecmp_128_insn,	3,	NONE,	0x0A,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 640 "x86insn_nasm.gperf"
    {"pcomgtb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 276 "x86insn_nasm.gperf"
    {"das",	onebyte_insn,	1,	NONE,	0x2F,	0,	0,	NOT_64,	0,	0,	0},
#line 25 "x86insn_nasm.gperf"
    {"addsubpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xD0,	0,	0,	CPU_SSE3,	0,	0},
#line 1318 "x86insn_nasm.gperf"
    {"vpaddsw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xED,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1489 "x86insn_nasm.gperf"
    {"xstorerng",	padlock_insn,	1,	NONE,	0xC0,	0x00,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 1188 "x86insn_nasm.gperf"
    {"vfmadd213sd",	vfma_sd_insn,	2,	NONE,	0xA9,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 140 "x86insn_nasm.gperf"
    {"comfalsepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0B,	0,	0,	CPU_SSE5,	0,	0},
#line 1132 "x86insn_nasm.gperf"
    {"vcmpordsd",	ssecmp_64_insn,	4,	NONE,	0x07,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1139 "x86insn_nasm.gperf"
    {"vcmptrue_usps",	ssecmp_128_insn,	3,	NONE,	0x1F,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 681 "x86insn_nasm.gperf"
    {"pcomtrueb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 14 "x86insn_nasm.gperf"
    {"a64",	NULL,	X86_ADDRSIZE>>8,	0x40,	0,	0,	0,	ONLY_64,	0,	0,	0},
#line 1345 "x86insn_nasm.gperf"
    {"vpcmpistrm",	sse4pcmpstr_insn,	1,	NONE,	0x62,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1296 "x86insn_nasm.gperf"
    {"vmrun",	svm_rax_insn,	2,	NONE,	0xD8,	0,	0,	0,	CPU_SVM,	0,	0},
#line 575 "x86insn_nasm.gperf"
    {"packusdw",	sse4_insn,	2,	NONE,	0x2B,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 214 "x86insn_nasm.gperf"
    {"comultpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 1410 "x86insn_nasm.gperf"
    {"vpsllw",	vpshift_insn,	4,	NONE,	0xF1,	0x71,	0x06,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 130 "x86insn_nasm.gperf"
    {"cmpunordsd",	ssecmp_64_insn,	4,	NONE,	0x03,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 189 "x86insn_nasm.gperf"
    {"comordss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1378 "x86insn_nasm.gperf"
    {"vpmovmskb",	pmovmskb_insn,	4,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 781 "x86insn_nasm.gperf"
    {"pmovsxwd",	sse4m64_insn,	2,	NONE,	0x23,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1395 "x86insn_nasm.gperf"
    {"vpmulld",	ssse3_insn,	3,	NONE,	0x40,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1226 "x86insn_nasm.gperf"
    {"vfnmadd231pd",	vfma_pd_insn,	2,	NONE,	0xBC,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 501 "x86insn_nasm.gperf"
    {"maxps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x5F,	0,	0,	CPU_SSE,	0,	0},
#line 801 "x86insn_nasm.gperf"
    {"pmvnzb",	cyrixmmx_insn,	1,	NONE,	0x5A,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 589 "x86insn_nasm.gperf"
    {"pause",	onebyte_prefix_insn,	1,	NONE,	0xF3,	0x90,	0,	0,	CPU_P4,	0,	0},
#line 249 "x86insn_nasm.gperf"
    {"cvtdq2pd",	xmm_xmm64_insn,	4,	NONE,	0xF3,	0xE6,	0,	0,	CPU_SSE2,	0,	0},
#line 1483 "x86insn_nasm.gperf"
    {"xrstor",	twobytemem_insn,	1,	NONE,	0x05,	0x0F,	0xAE,	0,	CPU_386,	CPU_XSAVE,	0},
#line 572 "x86insn_nasm.gperf"
    {"pabsw",	ssse3_insn,	3,	NONE,	0x1D,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 835 "x86insn_nasm.gperf"
    {"pshufhw",	xmm_xmm128_imm_insn,	1,	NONE,	0xF3,	0x70,	0,	0,	CPU_SSE2,	0,	0},
#line 389 "x86insn_nasm.gperf"
    {"fsub",	farith_insn,	7,	NONE,	0xE8,	0xE0,	0x04,	0,	CPU_FPU,	0,	0},
#line 1319 "x86insn_nasm.gperf"
    {"vpaddusb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDC,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 444 "x86insn_nasm.gperf"
    {"jle",	jcc_insn,	9,	NONE,	0x0E,	0,	0,	0,	0,	0,	0},
#line 319 "x86insn_nasm.gperf"
    {"fiadd",	fiarith_insn,	2,	NONE,	0x00,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 141 "x86insn_nasm.gperf"
    {"comfalseps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0B,	0,	0,	CPU_SSE5,	0,	0},
#line 424 "x86insn_nasm.gperf"
    {"int3",	onebyte_insn,	1,	NONE,	0xCC,	0,	0,	0,	0,	0,	0},
#line 117 "x86insn_nasm.gperf"
    {"cmpordpd",	ssecmp_128_insn,	3,	NONE,	0x07,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 1172 "x86insn_nasm.gperf"
    {"vdivpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x5E,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 842 "x86insn_nasm.gperf"
    {"pslldq",	pslrldq_insn,	2,	NONE,	0x07,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 891 "x86insn_nasm.gperf"
    {"repnz",	NULL,	X86_LOCKREP>>8,	0xF2,	0,	0,	0,	0,	0,	0,	0},
#line 1371 "x86insn_nasm.gperf"
    {"vpmaxuw",	ssse3_insn,	3,	NONE,	0x3E,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1131 "x86insn_nasm.gperf"
    {"vcmpordps",	ssecmp_128_insn,	3,	NONE,	0x07,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1276 "x86insn_nasm.gperf"
    {"vmovlpd",	movhlp_insn,	3,	NONE,	0x66,	0x12,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1101 "x86insn_nasm.gperf"
    {"vcmpngess",	ssecmp_32_insn,	4,	NONE,	0x09,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 12 "x86insn_nasm.gperf"
    {"a16",	NULL,	X86_ADDRSIZE>>8,	0x10,	0,	0,	0,	0,	0,	0,	0},
#line 219 "x86insn_nasm.gperf"
    {"comuneps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 50 "x86insn_nasm.gperf"
    {"bts",	bittest_insn,	6,	NONE,	0xAB,	0x05,	0,	0,	CPU_386,	0,	0},
#line 1126 "x86insn_nasm.gperf"
    {"vcmpord_spd",	ssecmp_128_insn,	3,	NONE,	0x17,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1454 "x86insn_nasm.gperf"
    {"vtestpd",	sse4_insn,	2,	NONE,	0x0F,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1348 "x86insn_nasm.gperf"
    {"vpermilps",	vpermil_insn,	4,	NONE,	0x04,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1170 "x86insn_nasm.gperf"
    {"vcvttsd2si",	cvt_rx_xmm64_insn,	4,	NONE,	0xF2,	0x2C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 795 "x86insn_nasm.gperf"
    {"pmulhw",	mmxsse2_insn,	2,	NONE,	0xE5,	0,	0,	0,	CPU_MMX,	0,	0},
#line 38 "x86insn_nasm.gperf"
    {"arpl",	arpl_insn,	1,	NONE,	0,	0,	0,	NOT_64,	CPU_286,	CPU_Prot,	0},
#line 1174 "x86insn_nasm.gperf"
    {"vdivsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5E,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 874 "x86insn_nasm.gperf"
    {"pushf",	onebyte_insn,	1,	NONE,	0x9C,	0x00,	0x40,	0,	0,	0,	0},
#line 1271 "x86insn_nasm.gperf"
    {"vmovdqu",	movau_insn,	6,	NONE,	0xF3,	0x6F,	0x10,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 81 "x86insn_nasm.gperf"
    {"cmovnle",	cmovcc_insn,	3,	NONE,	0x0F,	0,	0,	0,	CPU_686,	0,	0},
#line 777 "x86insn_nasm.gperf"
    {"pmovsxbd",	sse4m32_insn,	2,	NONE,	0x21,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 670 "x86insn_nasm.gperf"
    {"pcomnequb",	sse5comcc_insn,	1,	NONE,	0x6C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 482 "x86insn_nasm.gperf"
    {"loadall",	twobyte_insn,	1,	NONE,	0x0F,	0x07,	0,	0,	CPU_386,	CPU_Undoc,	0},
#line 1485 "x86insn_nasm.gperf"
    {"xsetbv",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xD1,	0,	CPU_386,	CPU_Priv,	CPU_XSAVE},
#line 358 "x86insn_nasm.gperf"
    {"fnmaddss",	sse5arith32_insn,	8,	NONE,	0x12,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 121 "x86insn_nasm.gperf"
    {"cmppd",	xmm_xmm128_imm_insn,	1,	NONE,	0x66,	0xC2,	0,	0,	CPU_SSE2,	0,	0},
#line 1044 "x86insn_nasm.gperf"
    {"vcmpfalsesd",	ssecmp_64_insn,	4,	NONE,	0x0B,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1243 "x86insn_nasm.gperf"
    {"vhaddps",	xmm_xmm128_256_insn,	3,	NONE,	0xF2,	0x7C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 676 "x86insn_nasm.gperf"
    {"pcomneud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1405 "x86insn_nasm.gperf"
    {"vpsignd",	ssse3_insn,	3,	NONE,	0x0A,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 78 "x86insn_nasm.gperf"
    {"cmovng",	cmovcc_insn,	3,	NONE,	0x0E,	0,	0,	0,	CPU_686,	0,	0},
#line 1363 "x86insn_nasm.gperf"
    {"vpinsrw",	pinsrw_insn,	9,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1249 "x86insn_nasm.gperf"
    {"vldmxcsr",	ldstmxcsr_insn,	1,	NONE,	0x02,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 326 "x86insn_nasm.gperf"
    {"fincstp",	twobyte_insn,	1,	NONE,	0xD9,	0xF7,	0,	0,	CPU_FPU,	0,	0},
#line 131 "x86insn_nasm.gperf"
    {"cmpunordss",	ssecmp_32_insn,	4,	NONE,	0x03,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 923 "x86insn_nasm.gperf"
    {"setg",	setcc_insn,	1,	NONE,	0x0F,	0,	0,	0,	CPU_386,	0,	0},
#line 478 "x86insn_nasm.gperf"
    {"lgs",	lfgss_insn,	2,	NONE,	0xB5,	0,	0,	0,	CPU_386,	0,	0},
#line 390 "x86insn_nasm.gperf"
    {"fsubp",	farithp_insn,	3,	NONE,	0xE8,	0,	0,	0,	CPU_FPU,	0,	0},
#line 168 "x86insn_nasm.gperf"
    {"comnesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1215 "x86insn_nasm.gperf"
    {"vfmsubadd213ps",	vfma_ps_insn,	2,	NONE,	0xA7,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1251 "x86insn_nasm.gperf"
    {"vmaskmovpd",	vmaskmov_insn,	4,	NONE,	0x2D,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 253 "x86insn_nasm.gperf"
    {"cvtpd2ps",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5A,	0,	0,	CPU_SSE2,	0,	0},
#line 558 "x86insn_nasm.gperf"
    {"nop",	onebyte_insn,	1,	NONE,	0x90,	0,	0,	0,	0,	0,	0},
#line 990 "x86insn_nasm.gperf"
    {"ucomisd",	xmm_xmm64_insn,	4,	NONE,	0x66,	0x2E,	0,	0,	CPU_SSE2,	0,	0},
#line 1462 "x86insn_nasm.gperf"
    {"vxorpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x57,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 940 "x86insn_nasm.gperf"
    {"setnz",	setcc_insn,	1,	NONE,	0x05,	0,	0,	0,	CPU_386,	0,	0},
#line 691 "x86insn_nasm.gperf"
    {"pcomuq",	sse5com_insn,	1,	NONE,	0x6F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 375 "x86insn_nasm.gperf"
    {"frczss",	sse5two32_insn,	2,	NONE,	0x12,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1230 "x86insn_nasm.gperf"
    {"vfnmsub132pd",	vfma_pd_insn,	2,	NONE,	0x9E,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1187 "x86insn_nasm.gperf"
    {"vfmadd213ps",	vfma_ps_insn,	2,	NONE,	0xA8,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1387 "x86insn_nasm.gperf"
    {"vpmovzxbw",	sse4m64_insn,	2,	NONE,	0x30,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 84 "x86insn_nasm.gperf"
    {"cmovns",	cmovcc_insn,	3,	NONE,	0x09,	0,	0,	0,	CPU_686,	0,	0},
#line 1112 "x86insn_nasm.gperf"
    {"vcmpnle_uqsd",	ssecmp_64_insn,	4,	NONE,	0x16,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1068 "x86insn_nasm.gperf"
    {"vcmplesd",	ssecmp_64_insn,	4,	NONE,	0x02,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 689 "x86insn_nasm.gperf"
    {"pcomub",	sse5com_insn,	1,	NONE,	0x6C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 543 "x86insn_nasm.gperf"
    {"movss",	movss_insn,	4,	NONE,	0,	0,	0,	0,	CPU_SSE,	0,	0},
#line 32 "x86insn_nasm.gperf"
    {"aeskeygenassist",	aes_imm_insn,	1,	NONE,	0x3A,	0xDF,	0,	0,	CPU_AES,	0,	0},
#line 997 "x86insn_nasm.gperf"
    {"unpcklpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x14,	0,	0,	CPU_SSE2,	0,	0},
#line 1206 "x86insn_nasm.gperf"
    {"vfmsub213sd",	vfma_sd_insn,	2,	NONE,	0xAB,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1065 "x86insn_nasm.gperf"
    {"vcmple_oqss",	ssecmp_32_insn,	4,	NONE,	0x12,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 230 "x86insn_nasm.gperf"
    {"comungtpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 190 "x86insn_nasm.gperf"
    {"compd",	sse5com_insn,	1,	NONE,	0x2D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 429 "x86insn_nasm.gperf"
    {"iret",	onebyte_insn,	1,	NONE,	0xCF,	0,	0,	0,	0,	0,	0},
#line 956 "x86insn_nasm.gperf"
    {"skinit",	skinit_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SVM,	0,	0},
#line 292 "x86insn_nasm.gperf"
    {"faddp",	farithp_insn,	3,	NONE,	0xC0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 974 "x86insn_nasm.gperf"
    {"stosw",	onebyte_insn,	1,	NONE,	0xAB,	0x10,	0,	0,	0,	0,	0},
#line 301 "x86insn_nasm.gperf"
    {"fcmovnbe",	fcmovcc_insn,	1,	NONE,	0xDB,	0xD0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 421 "x86insn_nasm.gperf"
    {"insw",	onebyte_insn,	1,	NONE,	0x6D,	0x10,	0,	0,	0,	0,	0},
#line 741 "x86insn_nasm.gperf"
    {"phsubwd",	sse5two_insn,	1,	NONE,	0x62,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 848 "x86insn_nasm.gperf"
    {"psrldq",	pslrldq_insn,	2,	NONE,	0x03,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 961 "x86insn_nasm.gperf"
    {"smsw",	sldtmsw_insn,	6,	NONE,	0x04,	0x01,	0,	0,	CPU_286,	0,	0},
#line 471 "x86insn_nasm.gperf"
    {"lds",	ldes_insn,	2,	NONE,	0xC5,	0,	0,	NOT_64,	0,	0,	0},
#line 834 "x86insn_nasm.gperf"
    {"pshufd",	xmm_xmm128_imm_insn,	1,	NONE,	0x66,	0x70,	0,	0,	CPU_SSE2,	0,	0},
#line 167 "x86insn_nasm.gperf"
    {"comneqss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 604 "x86insn_nasm.gperf"
    {"pcmpeqq",	sse4_insn,	2,	NONE,	0x29,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1027 "x86insn_nasm.gperf"
    {"vcmpeq_uqps",	ssecmp_128_insn,	3,	NONE,	0x08,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 655 "x86insn_nasm.gperf"
    {"pcomlew",	sse5comcc_insn,	1,	NONE,	0x4D,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 862 "x86insn_nasm.gperf"
    {"punpckhbw",	mmxsse2_insn,	2,	NONE,	0x68,	0,	0,	0,	CPU_MMX,	0,	0},
#line 796 "x86insn_nasm.gperf"
    {"pmulld",	sse4_insn,	2,	NONE,	0x40,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 369 "x86insn_nasm.gperf"
    {"fprem",	twobyte_insn,	1,	NONE,	0xD9,	0xF8,	0,	0,	CPU_FPU,	0,	0},
#line 527 "x86insn_nasm.gperf"
    {"movmskps",	movmsk_insn,	4,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_SSE,	0},
#line 363 "x86insn_nasm.gperf"
    {"fnop",	twobyte_insn,	1,	NONE,	0xD9,	0xD0,	0,	0,	CPU_FPU,	0,	0},
#line 309 "x86insn_nasm.gperf"
    {"fcompp",	twobyte_insn,	1,	NONE,	0xDE,	0xD9,	0,	0,	CPU_FPU,	0,	0},
#line 597 "x86insn_nasm.gperf"
    {"pclmulhqlqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x01,	0,	0,	0,	CPU_AVX,	0,	0},
#line 134 "x86insn_nasm.gperf"
    {"cmpxchg486",	cmpxchgxadd_insn,	4,	NONE,	0xA6,	0,	0,	0,	CPU_486,	CPU_Undoc,	0},
#line 1291 "x86insn_nasm.gperf"
    {"vmpsadbw",	sse4imm_insn,	2,	NONE,	0x42,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 340 "x86insn_nasm.gperf"
    {"fldln2",	twobyte_insn,	1,	NONE,	0xD9,	0xED,	0,	0,	CPU_FPU,	0,	0},
#line 342 "x86insn_nasm.gperf"
    {"fldz",	twobyte_insn,	1,	NONE,	0xD9,	0xEE,	0,	0,	CPU_FPU,	0,	0},
#line 1212 "x86insn_nasm.gperf"
    {"vfmsubadd132pd",	vfma_pd_insn,	2,	NONE,	0x97,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 883 "x86insn_nasm.gperf"
    {"rdmsr",	twobyte_insn,	1,	NONE,	0x0F,	0x32,	0,	0,	CPU_586,	CPU_Priv,	0},
#line 502 "x86insn_nasm.gperf"
    {"maxsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5F,	0,	0,	CPU_SSE2,	0,	0},
#line 1268 "x86insn_nasm.gperf"
    {"vmovd",	vmovd_insn,	2,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_386,	CPU_AVX,	0},
#line 186 "x86insn_nasm.gperf"
    {"comordpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 63 "x86insn_nasm.gperf"
    {"cmovae",	cmovcc_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_686,	0,	0},
#line 1043 "x86insn_nasm.gperf"
    {"vcmpfalseps",	ssecmp_128_insn,	3,	NONE,	0x0B,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1292 "x86insn_nasm.gperf"
    {"vmptrld",	vmxtwobytemem_insn,	1,	NONE,	0x06,	0,	0,	0,	CPU_P4,	0,	0},
#line 511 "x86insn_nasm.gperf"
    {"mov",	mov_insn,	69,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 1474 "x86insn_nasm.gperf"
    {"xcryptcfb",	padlock_insn,	1,	NONE,	0xE0,	0xF3,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 1122 "x86insn_nasm.gperf"
    {"vcmpnltpd",	ssecmp_128_insn,	3,	NONE,	0x05,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1173 "x86insn_nasm.gperf"
    {"vdivps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x5E,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1305 "x86insn_nasm.gperf"
    {"vorpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x56,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1265 "x86insn_nasm.gperf"
    {"vmmcall",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xD9,	0,	CPU_SVM,	0,	0},
#line 534 "x86insn_nasm.gperf"
    {"movntsd",	movntsd_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE4a,	0,	0},
#line 423 "x86insn_nasm.gperf"
    {"int03",	onebyte_insn,	1,	NONE,	0xCC,	0,	0,	0,	0,	0,	0},
#line 111 "x86insn_nasm.gperf"
    {"cmpnlesd",	ssecmp_64_insn,	4,	NONE,	0x06,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 673 "x86insn_nasm.gperf"
    {"pcomnequw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1311 "x86insn_nasm.gperf"
    {"vpacksswb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x63,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 435 "x86insn_nasm.gperf"
    {"jb",	jcc_insn,	9,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 1179 "x86insn_nasm.gperf"
    {"verw",	prot286_insn,	1,	NONE,	0x05,	0x00,	0,	0,	CPU_286,	CPU_Prot,	0},
#line 1001 "x86insn_nasm.gperf"
    {"vaddsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x58,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 826 "x86insn_nasm.gperf"
    {"pshad",	sse5psh_insn,	2,	NONE,	0x06,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 951 "x86insn_nasm.gperf"
    {"shr",	shift_insn,	16,	NONE,	0x05,	0,	0,	0,	0,	0,	0},
#line 960 "x86insn_nasm.gperf"
    {"smintold",	twobyte_insn,	1,	NONE,	0x0F,	0x7E,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_Obs},
#line 1287 "x86insn_nasm.gperf"
    {"vmovsldup",	avx_xmm_xmm128_insn,	2,	NONE,	0xF3,	0x12,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 289 "x86insn_nasm.gperf"
    {"f2xm1",	twobyte_insn,	1,	NONE,	0xD9,	0xF0,	0,	0,	CPU_FPU,	0,	0},
#line 483 "x86insn_nasm.gperf"
    {"loadall286",	twobyte_insn,	1,	NONE,	0x0F,	0x05,	0,	0,	CPU_286,	CPU_Undoc,	0},
#line 661 "x86insn_nasm.gperf"
    {"pcomltuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1038 "x86insn_nasm.gperf"
    {"vcmpfalse_ospd",	ssecmp_128_insn,	3,	NONE,	0x1B,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 696 "x86insn_nasm.gperf"
    {"permps",	sse5arith_insn,	4,	NONE,	0x20,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1104 "x86insn_nasm.gperf"
    {"vcmpngt_uqsd",	ssecmp_64_insn,	4,	NONE,	0x1A,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1433 "x86insn_nasm.gperf"
    {"vpunpcklwd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x61,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 868 "x86insn_nasm.gperf"
    {"punpcklqdq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6C,	0,	0,	CPU_SSE2,	0,	0},
#line 861 "x86insn_nasm.gperf"
    {"ptest",	sse4_insn,	2,	NONE,	0x17,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 807 "x86insn_nasm.gperf"
    {"popcnt",	cnt_insn,	3,	NONE,	0xB8,	0,	0,	0,	CPU_SSE42,	0,	0},
#line 591 "x86insn_nasm.gperf"
    {"pavgb",	mmxsse2_insn,	2,	NONE,	0xE0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 845 "x86insn_nasm.gperf"
    {"psrad",	pshift_insn,	4,	NONE,	0xE2,	0x72,	0x04,	0,	CPU_MMX,	0,	0},
#line 618 "x86insn_nasm.gperf"
    {"pcomeqq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1161 "x86insn_nasm.gperf"
    {"vcvtps2pd",	avx_cvt_xmm64_insn,	3,	NONE,	0x00,	0x5A,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1134 "x86insn_nasm.gperf"
    {"vcmppd",	xmm_xmm128_imm_256_insn,	3,	NONE,	0x66,	0xC2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 352 "x86insn_nasm.gperf"
    {"fmulp",	farithp_insn,	3,	NONE,	0xC8,	0,	0,	0,	CPU_FPU,	0,	0},
#line 924 "x86insn_nasm.gperf"
    {"setge",	setcc_insn,	1,	NONE,	0x0D,	0,	0,	0,	CPU_386,	0,	0},
#line 114 "x86insn_nasm.gperf"
    {"cmpnltps",	ssecmp_128_insn,	3,	NONE,	0x05,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1362 "x86insn_nasm.gperf"
    {"vpinsrq",	pinsrq_insn,	2,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 177 "x86insn_nasm.gperf"
    {"comngtss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 461 "x86insn_nasm.gperf"
    {"jp",	jcc_insn,	9,	NONE,	0x0A,	0,	0,	0,	0,	0,	0},
#line 347 "x86insn_nasm.gperf"
    {"fmsubpd",	sse5arith_insn,	4,	NONE,	0x09,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 898 "x86insn_nasm.gperf"
    {"roundpd",	sse4imm_insn,	2,	NONE,	0x09,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 567 "x86insn_nasm.gperf"
    {"outsb",	onebyte_insn,	1,	NONE,	0x6E,	0x00,	0,	0,	0,	0,	0},
#line 1254 "x86insn_nasm.gperf"
    {"vmaxps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x5F,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1057 "x86insn_nasm.gperf"
    {"vcmpgt_oqss",	ssecmp_32_insn,	4,	NONE,	0x1E,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 208 "x86insn_nasm.gperf"
    {"comugtsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 1301 "x86insn_nasm.gperf"
    {"vmulss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x59,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 840 "x86insn_nasm.gperf"
    {"psignw",	ssse3_insn,	3,	NONE,	0x09,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1449 "x86insn_nasm.gperf"
    {"vstmxcsr",	ldstmxcsr_insn,	1,	NONE,	0x03,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1334 "x86insn_nasm.gperf"
    {"vpcmpeqb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x74,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 794 "x86insn_nasm.gperf"
    {"pmulhuw",	mmxsse2_insn,	2,	NONE,	0xE4,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 227 "x86insn_nasm.gperf"
    {"comungeps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 1465 "x86insn_nasm.gperf"
    {"vzeroupper",	vzero_insn,	1,	NONE,	0xC0,	0,	0,	0,	CPU_AVX,	0,	0},
#line 1208 "x86insn_nasm.gperf"
    {"vfmsub231pd",	vfma_pd_insn,	2,	NONE,	0xBA,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 513 "x86insn_nasm.gperf"
    {"movaps",	movau_insn,	6,	NONE,	0x00,	0x28,	0x01,	0,	CPU_SSE,	0,	0},
#line 359 "x86insn_nasm.gperf"
    {"fnmsubpd",	sse5arith_insn,	4,	NONE,	0x19,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1233 "x86insn_nasm.gperf"
    {"vfnmsub132ss",	vfma_ss_insn,	2,	NONE,	0x9F,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 79 "x86insn_nasm.gperf"
    {"cmovnge",	cmovcc_insn,	3,	NONE,	0x0C,	0,	0,	0,	CPU_686,	0,	0},
#line 1167 "x86insn_nasm.gperf"
    {"vcvtss2si",	cvt_rx_xmm32_insn,	4,	NONE,	0xF3,	0x2D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1138 "x86insn_nasm.gperf"
    {"vcmptrue_uspd",	ssecmp_128_insn,	3,	NONE,	0x1F,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 587 "x86insn_nasm.gperf"
    {"pand",	mmxsse2_insn,	2,	NONE,	0xDB,	0,	0,	0,	CPU_MMX,	0,	0},
#line 64 "x86insn_nasm.gperf"
    {"cmovb",	cmovcc_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_686,	0,	0},
#line 1258 "x86insn_nasm.gperf"
    {"vmclear",	vmxthreebytemem_insn,	1,	NONE,	0x66,	0,	0,	0,	CPU_P4,	0,	0},
#line 1009 "x86insn_nasm.gperf"
    {"vaesimc",	aesimc_insn,	1,	NONE,	0x38,	0xDB,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 380 "x86insn_nasm.gperf"
    {"fsetpm",	twobyte_insn,	1,	NONE,	0xDB,	0xE4,	0,	0,	CPU_286,	CPU_FPU,	CPU_Obs},
#line 622 "x86insn_nasm.gperf"
    {"pcomequw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1078 "x86insn_nasm.gperf"
    {"vcmpneq_oqpd",	ssecmp_128_insn,	3,	NONE,	0x0C,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 799 "x86insn_nasm.gperf"
    {"pmvgezb",	cyrixmmx_insn,	1,	NONE,	0x5C,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 62 "x86insn_nasm.gperf"
    {"cmova",	cmovcc_insn,	3,	NONE,	0x07,	0,	0,	0,	CPU_686,	0,	0},
#line 684 "x86insn_nasm.gperf"
    {"pcomtrueub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 494 "x86insn_nasm.gperf"
    {"lsl",	bsfr_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_286,	CPU_Prot,	0},
#line 1014 "x86insn_nasm.gperf"
    {"vandps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x54,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1177 "x86insn_nasm.gperf"
    {"vdpps",	sse4imm_256_insn,	3,	NONE,	0x40,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1218 "x86insn_nasm.gperf"
    {"vfnmadd132pd",	vfma_pd_insn,	2,	NONE,	0x9C,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1290 "x86insn_nasm.gperf"
    {"vmovups",	movau_insn,	6,	NONE,	0x00,	0x10,	0x01,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1051 "x86insn_nasm.gperf"
    {"vcmpgeps",	ssecmp_128_insn,	3,	NONE,	0x0D,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1114 "x86insn_nasm.gperf"
    {"vcmpnlepd",	ssecmp_128_insn,	3,	NONE,	0x06,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 416 "x86insn_nasm.gperf"
    {"inc",	incdec_insn,	6,	NONE,	0x40,	0x00,	0,	0,	0,	0,	0},
#line 1008 "x86insn_nasm.gperf"
    {"vaesenclast",	aes_insn,	2,	NONE,	0x38,	0xDD,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 323 "x86insn_nasm.gperf"
    {"fidivr",	fiarith_insn,	2,	NONE,	0x07,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 417 "x86insn_nasm.gperf"
    {"insb",	onebyte_insn,	1,	NONE,	0x6C,	0x00,	0,	0,	0,	0,	0},
#line 1407 "x86insn_nasm.gperf"
    {"vpslld",	vpshift_insn,	4,	NONE,	0xF2,	0x72,	0x06,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 608 "x86insn_nasm.gperf"
    {"pcmpgtb",	mmxsse2_insn,	2,	NONE,	0x64,	0,	0,	0,	CPU_MMX,	0,	0},
#line 406 "x86insn_nasm.gperf"
    {"fyl2xp1",	twobyte_insn,	1,	NONE,	0xD9,	0xF9,	0,	0,	CPU_FPU,	0,	0},
#line 399 "x86insn_nasm.gperf"
    {"fwait",	onebyte_insn,	1,	NONE,	0x9B,	0,	0,	0,	CPU_FPU,	0,	0},
#line 437 "x86insn_nasm.gperf"
    {"jc",	jcc_insn,	9,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 385 "x86insn_nasm.gperf"
    {"fstcw",	fstcw_insn,	1,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 181 "x86insn_nasm.gperf"
    {"comnless",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 312 "x86insn_nasm.gperf"
    {"fdiv",	farith_insn,	7,	NONE,	0xF8,	0xF0,	0x06,	0,	CPU_FPU,	0,	0},
#line 1464 "x86insn_nasm.gperf"
    {"vzeroall",	vzero_insn,	1,	NONE,	0xC4,	0,	0,	0,	CPU_AVX,	0,	0},
#line 271 "x86insn_nasm.gperf"
    {"cvttsd2si",	cvt_rx_xmm64_insn,	4,	NONE,	0xF2,	0x2C,	0,	0,	CPU_SSE2,	0,	0},
#line 306 "x86insn_nasm.gperf"
    {"fcomi",	fcom2_insn,	2,	NONE,	0xDB,	0xF0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 251 "x86insn_nasm.gperf"
    {"cvtpd2dq",	xmm_xmm128_insn,	2,	NONE,	0xF2,	0xE6,	0,	0,	CPU_SSE2,	0,	0},
#line 387 "x86insn_nasm.gperf"
    {"fstp",	fstp_insn,	4,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 631 "x86insn_nasm.gperf"
    {"pcomfalsew",	sse5comcc_insn,	1,	NONE,	0x4D,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 986 "x86insn_nasm.gperf"
    {"sysenter",	twobyte_insn,	1,	NONE,	0x0F,	0x34,	0,	NOT_64,	CPU_686,	0,	0},
#line 524 "x86insn_nasm.gperf"
    {"movlpd",	movhlp_insn,	3,	NONE,	0x66,	0x12,	0,	0,	CPU_SSE2,	0,	0},
#line 178 "x86insn_nasm.gperf"
    {"comnlepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 1064 "x86insn_nasm.gperf"
    {"vcmple_oqsd",	ssecmp_64_insn,	4,	NONE,	0x12,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 829 "x86insn_nasm.gperf"
    {"pshlb",	sse5psh_insn,	2,	NONE,	0x00,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1148 "x86insn_nasm.gperf"
    {"vcmpunord_ssd",	ssecmp_64_insn,	4,	NONE,	0x13,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1193 "x86insn_nasm.gperf"
    {"vfmadd231ss",	vfma_ss_insn,	2,	NONE,	0xB9,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 330 "x86insn_nasm.gperf"
    {"fisttp",	fildstp_insn,	4,	NONE,	0x01,	0x00,	0x01,	0,	CPU_SSE3,	0,	0},
#line 1304 "x86insn_nasm.gperf"
    {"vmxon",	vmxthreebytemem_insn,	1,	NONE,	0xF3,	0,	0,	0,	CPU_P4,	0,	0},
#line 18 "x86insn_nasm.gperf"
    {"aas",	onebyte_insn,	1,	NONE,	0x3F,	0,	0,	NOT_64,	0,	0,	0},
#line 40 "x86insn_nasm.gperf"
    {"blendps",	sse4imm_insn,	2,	NONE,	0x0C,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1298 "x86insn_nasm.gperf"
    {"vmulpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x59,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1468 "x86insn_nasm.gperf"
    {"wrmsr",	twobyte_insn,	1,	NONE,	0x0F,	0x30,	0,	0,	CPU_586,	CPU_Priv,	0},
#line 327 "x86insn_nasm.gperf"
    {"finit",	threebyte_insn,	1,	NONE,	0x9B,	0xDB,	0xE3,	0,	CPU_FPU,	0,	0},
#line 746 "x86insn_nasm.gperf"
    {"pinsrq",	pinsrq_insn,	2,	NONE,	0,	0,	0,	ONLY_64,	CPU_SSE41,	0,	0},
#line 155 "x86insn_nasm.gperf"
    {"comleps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 194 "x86insn_nasm.gperf"
    {"comtruepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0F,	0,	0,	CPU_SSE5,	0,	0},
#line 624 "x86insn_nasm.gperf"
    {"pcomfalseb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 522 "x86insn_nasm.gperf"
    {"movhps",	movhlp_insn,	3,	NONE,	0x00,	0x16,	0,	0,	CPU_SSE,	0,	0},
#line 67 "x86insn_nasm.gperf"
    {"cmove",	cmovcc_insn,	3,	NONE,	0x04,	0,	0,	0,	CPU_686,	0,	0},
#line 646 "x86insn_nasm.gperf"
    {"pcomgtuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 199 "x86insn_nasm.gperf"
    {"comueqps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x08,	0,	0,	CPU_SSE5,	0,	0},
#line 1221 "x86insn_nasm.gperf"
    {"vfnmadd132ss",	vfma_ss_insn,	2,	NONE,	0x9D,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 610 "x86insn_nasm.gperf"
    {"pcmpgtq",	sse4_insn,	2,	NONE,	0x37,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 463 "x86insn_nasm.gperf"
    {"jpo",	jcc_insn,	9,	NONE,	0x0B,	0,	0,	0,	0,	0,	0},
#line 1282 "x86insn_nasm.gperf"
    {"vmovntpd",	movnt_insn,	2,	NONE,	0x66,	0x2B,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1041 "x86insn_nasm.gperf"
    {"vcmpfalse_osss",	ssecmp_32_insn,	4,	NONE,	0x1B,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 581 "x86insn_nasm.gperf"
    {"paddsiw",	cyrixmmx_insn,	1,	NONE,	0x51,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 942 "x86insn_nasm.gperf"
    {"setp",	setcc_insn,	1,	NONE,	0x0A,	0,	0,	0,	CPU_386,	0,	0},
#line 339 "x86insn_nasm.gperf"
    {"fldlg2",	twobyte_insn,	1,	NONE,	0xD9,	0xEC,	0,	0,	CPU_FPU,	0,	0},
#line 1346 "x86insn_nasm.gperf"
    {"vperm2f128",	vperm2f128_insn,	1,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1267 "x86insn_nasm.gperf"
    {"vmovaps",	movau_insn,	6,	NONE,	0x00,	0x28,	0x01,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1285 "x86insn_nasm.gperf"
    {"vmovsd",	movsd_insn,	5,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1322 "x86insn_nasm.gperf"
    {"vpalignr",	sse4imm_insn,	2,	NONE,	0x0F,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 350 "x86insn_nasm.gperf"
    {"fmsubss",	sse5arith32_insn,	8,	NONE,	0x0A,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 519 "x86insn_nasm.gperf"
    {"movdqu",	movau_insn,	6,	NONE,	0xF3,	0x6F,	0x10,	0,	CPU_SSE2,	0,	0},
#line 302 "x86insn_nasm.gperf"
    {"fcmovne",	fcmovcc_insn,	1,	NONE,	0xDB,	0xC8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1306 "x86insn_nasm.gperf"
    {"vorps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x56,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 619 "x86insn_nasm.gperf"
    {"pcomequb",	sse5comcc_insn,	1,	NONE,	0x6C,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1028 "x86insn_nasm.gperf"
    {"vcmpeq_uqsd",	ssecmp_64_insn,	4,	NONE,	0x08,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1259 "x86insn_nasm.gperf"
    {"vminpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x5D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 658 "x86insn_nasm.gperf"
    {"pcomltq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 89 "x86insn_nasm.gperf"
    {"cmovpo",	cmovcc_insn,	3,	NONE,	0x0B,	0,	0,	0,	CPU_686,	0,	0},
#line 112 "x86insn_nasm.gperf"
    {"cmpnless",	ssecmp_32_insn,	4,	NONE,	0x06,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 125 "x86insn_nasm.gperf"
    {"cmpsq",	onebyte_insn,	1,	NONE,	0xA7,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 813 "x86insn_nasm.gperf"
    {"pperm",	sse5arith_insn,	4,	NONE,	0x23,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 897 "x86insn_nasm.gperf"
    {"ror",	shift_insn,	16,	NONE,	0x01,	0,	0,	0,	0,	0,	0},
#line 580 "x86insn_nasm.gperf"
    {"paddsb",	mmxsse2_insn,	2,	NONE,	0xEC,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1020 "x86insn_nasm.gperf"
    {"vbroadcastsd",	vbroadcastsd_insn,	1,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 526 "x86insn_nasm.gperf"
    {"movmskpd",	movmsk_insn,	4,	NONE,	0x66,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 382 "x86insn_nasm.gperf"
    {"fsincos",	twobyte_insn,	1,	NONE,	0xD9,	0xFB,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 944 "x86insn_nasm.gperf"
    {"setpo",	setcc_insn,	1,	NONE,	0x0B,	0,	0,	0,	CPU_386,	0,	0},
#line 1184 "x86insn_nasm.gperf"
    {"vfmadd132sd",	vfma_sd_insn,	2,	NONE,	0x99,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 557 "x86insn_nasm.gperf"
    {"neg",	f6_insn,	4,	NONE,	0x03,	0,	0,	0,	0,	0,	0},
#line 1397 "x86insn_nasm.gperf"
    {"vpmuludq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xF4,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 264 "x86insn_nasm.gperf"
    {"cvtsi2ss",	cvt_xmm_rmx_insn,	6,	NONE,	0xF3,	0x2A,	0,	0,	CPU_386,	CPU_SSE,	0},
#line 712 "x86insn_nasm.gperf"
    {"pfpnacc",	now3d_insn,	1,	NONE,	0x8E,	0,	0,	0,	CPU_3DNow,	CPU_Athlon,	0},
#line 1140 "x86insn_nasm.gperf"
    {"vcmptrue_ussd",	ssecmp_64_insn,	4,	NONE,	0x1F,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 282 "x86insn_nasm.gperf"
    {"divss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5E,	0,	0,	CPU_SSE,	0,	0},
#line 1390 "x86insn_nasm.gperf"
    {"vpmovzxwq",	sse4m32_insn,	2,	NONE,	0x34,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1450 "x86insn_nasm.gperf"
    {"vsubpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x5C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 772 "x86insn_nasm.gperf"
    {"pminsw",	mmxsse2_insn,	2,	NONE,	0xEA,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 500 "x86insn_nasm.gperf"
    {"maxpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5F,	0,	0,	CPU_SSE2,	0,	0},
#line 912 "x86insn_nasm.gperf"
    {"sbb",	arith_insn,	22,	NONE,	0x18,	0x03,	0,	0,	0,	0,	0},
#line 1352 "x86insn_nasm.gperf"
    {"vpextrw",	pextrw_insn,	7,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 702 "x86insn_nasm.gperf"
    {"pf2iw",	now3d_insn,	1,	NONE,	0x1C,	0,	0,	0,	CPU_3DNow,	CPU_Athlon,	0},
#line 1273 "x86insn_nasm.gperf"
    {"vmovhpd",	movhlp_insn,	3,	NONE,	0x66,	0x16,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 310 "x86insn_nasm.gperf"
    {"fcos",	twobyte_insn,	1,	NONE,	0xD9,	0xFF,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 1239 "x86insn_nasm.gperf"
    {"vfnmsub231ps",	vfma_ps_insn,	2,	NONE,	0xBE,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 616 "x86insn_nasm.gperf"
    {"pcomeqb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 142 "x86insn_nasm.gperf"
    {"comfalsesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0B,	0,	0,	CPU_SSE5,	0,	0},
#line 1398 "x86insn_nasm.gperf"
    {"vpor",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xEB,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 544 "x86insn_nasm.gperf"
    {"movsw",	onebyte_insn,	1,	NONE,	0xA5,	0x10,	0,	0,	0,	0,	0},
#line 1035 "x86insn_nasm.gperf"
    {"vcmpeqps",	ssecmp_128_insn,	3,	NONE,	0x00,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 976 "x86insn_nasm.gperf"
    {"sub",	arith_insn,	22,	NONE,	0x28,	0x05,	0,	0,	0,	0,	0},
#line 1061 "x86insn_nasm.gperf"
    {"vcmpgtss",	ssecmp_32_insn,	4,	NONE,	0x0E,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 366 "x86insn_nasm.gperf"
    {"fnstenv",	onebytemem_insn,	1,	NONE,	0x06,	0xD9,	0,	0,	CPU_FPU,	0,	0},
#line 1350 "x86insn_nasm.gperf"
    {"vpextrd",	pextrd_insn,	1,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 357 "x86insn_nasm.gperf"
    {"fnmaddsd",	sse5arith64_insn,	8,	NONE,	0x13,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 491 "x86insn_nasm.gperf"
    {"loopne",	loop_insn,	8,	NONE,	0x00,	0,	0,	0,	0,	0,	0},
#line 499 "x86insn_nasm.gperf"
    {"maskmovq",	maskmovq_insn,	1,	NONE,	0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 454 "x86insn_nasm.gperf"
    {"jnl",	jcc_insn,	9,	NONE,	0x0D,	0,	0,	0,	0,	0,	0},
#line 931 "x86insn_nasm.gperf"
    {"setnc",	setcc_insn,	1,	NONE,	0x03,	0,	0,	0,	CPU_386,	0,	0},
#line 626 "x86insn_nasm.gperf"
    {"pcomfalseq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 645 "x86insn_nasm.gperf"
    {"pcomgtuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1408 "x86insn_nasm.gperf"
    {"vpslldq",	pslrldq_insn,	2,	NONE,	0x07,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 434 "x86insn_nasm.gperf"
    {"jae",	jcc_insn,	9,	NONE,	0x03,	0,	0,	0,	0,	0,	0},
#line 1312 "x86insn_nasm.gperf"
    {"vpackusdw",	ssse3_insn,	3,	NONE,	0x2B,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 819 "x86insn_nasm.gperf"
    {"prefetchw",	twobytemem_insn,	1,	NONE,	0x01,	0x0F,	0x0D,	0,	CPU_3DNow,	0,	0},
#line 621 "x86insn_nasm.gperf"
    {"pcomequq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 49 "x86insn_nasm.gperf"
    {"btr",	bittest_insn,	6,	NONE,	0xB3,	0x06,	0,	0,	CPU_386,	0,	0},
#line 1204 "x86insn_nasm.gperf"
    {"vfmsub213pd",	vfma_pd_insn,	2,	NONE,	0xAA,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1338 "x86insn_nasm.gperf"
    {"vpcmpestri",	sse4pcmpstr_insn,	1,	NONE,	0x61,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 678 "x86insn_nasm.gperf"
    {"pcomneuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 191 "x86insn_nasm.gperf"
    {"comps",	sse5com_insn,	1,	NONE,	0x2C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 28 "x86insn_nasm.gperf"
    {"aesdeclast",	aes_insn,	2,	NONE,	0x38,	0xDF,	0,	0,	CPU_AVX,	0,	0},
#line 1444 "x86insn_nasm.gperf"
    {"vshufps",	xmm_xmm128_imm_256_insn,	3,	NONE,	0x00,	0xC6,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 704 "x86insn_nasm.gperf"
    {"pfadd",	now3d_insn,	1,	NONE,	0x9E,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 108 "x86insn_nasm.gperf"
    {"cmpneqss",	ssecmp_32_insn,	4,	NONE,	0x04,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 135 "x86insn_nasm.gperf"
    {"cmpxchg8b",	cmpxchg8b_insn,	1,	NONE,	0,	0,	0,	0,	CPU_586,	0,	0},
#line 1111 "x86insn_nasm.gperf"
    {"vcmpnle_uqps",	ssecmp_128_insn,	3,	NONE,	0x16,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1049 "x86insn_nasm.gperf"
    {"vcmpge_oqss",	ssecmp_32_insn,	4,	NONE,	0x1D,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 487 "x86insn_nasm.gperf"
    {"lodsq",	onebyte_insn,	1,	NONE,	0xAD,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 356 "x86insn_nasm.gperf"
    {"fnmaddps",	sse5arith_insn,	4,	NONE,	0x10,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1460 "x86insn_nasm.gperf"
    {"vunpcklpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x14,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 436 "x86insn_nasm.gperf"
    {"jbe",	jcc_insn,	9,	NONE,	0x06,	0,	0,	0,	0,	0,	0},
#line 1242 "x86insn_nasm.gperf"
    {"vhaddpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x7C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 507 "x86insn_nasm.gperf"
    {"minsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5D,	0,	0,	CPU_SSE2,	0,	0},
#line 1093 "x86insn_nasm.gperf"
    {"vcmpneqss",	ssecmp_32_insn,	4,	NONE,	0x04,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 963 "x86insn_nasm.gperf"
    {"sqrtps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x51,	0,	0,	CPU_SSE,	0,	0},
#line 789 "x86insn_nasm.gperf"
    {"pmuldq",	sse4_insn,	2,	NONE,	0x28,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 724 "x86insn_nasm.gperf"
    {"phadddq",	sse5two_insn,	1,	NONE,	0x4B,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 137 "x86insn_nasm.gperf"
    {"comeqps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1079 "x86insn_nasm.gperf"
    {"vcmpneq_oqps",	ssecmp_128_insn,	3,	NONE,	0x0C,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1246 "x86insn_nasm.gperf"
    {"vinsertf128",	vinsertf128_insn,	1,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 104 "x86insn_nasm.gperf"
    {"cmpltss",	ssecmp_32_insn,	4,	NONE,	0x01,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 83 "x86insn_nasm.gperf"
    {"cmovnp",	cmovcc_insn,	3,	NONE,	0x0B,	0,	0,	0,	CPU_686,	0,	0},
#line 949 "x86insn_nasm.gperf"
    {"shl",	shift_insn,	16,	NONE,	0x04,	0,	0,	0,	0,	0,	0},
#line 1241 "x86insn_nasm.gperf"
    {"vfnmsub231ss",	vfma_ss_insn,	2,	NONE,	0xBF,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 541 "x86insn_nasm.gperf"
    {"movsldup",	xmm_xmm128_insn,	2,	NONE,	0xF3,	0x12,	0,	0,	CPU_SSE3,	0,	0},
#line 996 "x86insn_nasm.gperf"
    {"unpckhps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x15,	0,	0,	CPU_SSE,	0,	0},
#line 1289 "x86insn_nasm.gperf"
    {"vmovupd",	movau_insn,	6,	NONE,	0x66,	0x10,	0x01,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 821 "x86insn_nasm.gperf"
    {"protd",	sse5prot_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1257 "x86insn_nasm.gperf"
    {"vmcall",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC1,	0,	CPU_P4,	0,	0},
#line 1264 "x86insn_nasm.gperf"
    {"vmload",	svm_rax_insn,	2,	NONE,	0xDA,	0,	0,	0,	CPU_SVM,	0,	0},
#line 1153 "x86insn_nasm.gperf"
    {"vcmpunordss",	ssecmp_32_insn,	4,	NONE,	0x03,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1409 "x86insn_nasm.gperf"
    {"vpsllq",	vpshift_insn,	4,	NONE,	0xF3,	0x73,	0x06,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 832 "x86insn_nasm.gperf"
    {"pshlw",	sse5psh_insn,	2,	NONE,	0x01,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 474 "x86insn_nasm.gperf"
    {"les",	ldes_insn,	2,	NONE,	0xC4,	0,	0,	NOT_64,	0,	0,	0},
#line 831 "x86insn_nasm.gperf"
    {"pshlq",	sse5psh_insn,	2,	NONE,	0x03,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 731 "x86insn_nasm.gperf"
    {"phadduwq",	sse5two_insn,	1,	NONE,	0x57,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1210 "x86insn_nasm.gperf"
    {"vfmsub231sd",	vfma_sd_insn,	2,	NONE,	0xBB,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 74 "x86insn_nasm.gperf"
    {"cmovnb",	cmovcc_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_686,	0,	0},
#line 1223 "x86insn_nasm.gperf"
    {"vfnmadd213ps",	vfma_ps_insn,	2,	NONE,	0xAC,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1487 "x86insn_nasm.gperf"
    {"xsha256",	padlock_insn,	1,	NONE,	0xD0,	0xF3,	0xA6,	0,	CPU_PadLock,	0,	0},
#line 1376 "x86insn_nasm.gperf"
    {"vpminud",	ssse3_insn,	3,	NONE,	0x3B,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 957 "x86insn_nasm.gperf"
    {"sldt",	sldtmsw_insn,	6,	NONE,	0x00,	0x00,	0,	0,	CPU_286,	0,	0},
#line 1297 "x86insn_nasm.gperf"
    {"vmsave",	svm_rax_insn,	2,	NONE,	0xDB,	0,	0,	0,	CPU_SVM,	0,	0},
#line 962 "x86insn_nasm.gperf"
    {"sqrtpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x51,	0,	0,	CPU_SSE2,	0,	0},
#line 1375 "x86insn_nasm.gperf"
    {"vpminub",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDA,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 649 "x86insn_nasm.gperf"
    {"pcomled",	sse5comcc_insn,	1,	NONE,	0x4E,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 207 "x86insn_nasm.gperf"
    {"comugtps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 1145 "x86insn_nasm.gperf"
    {"vcmptruess",	ssecmp_32_insn,	4,	NONE,	0x0F,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 76 "x86insn_nasm.gperf"
    {"cmovnc",	cmovcc_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_686,	0,	0},
#line 521 "x86insn_nasm.gperf"
    {"movhpd",	movhlp_insn,	3,	NONE,	0x66,	0x16,	0,	0,	CPU_SSE2,	0,	0},
#line 885 "x86insn_nasm.gperf"
    {"rdshr",	rdwrshr_insn,	1,	NONE,	0x00,	0,	0,	0,	CPU_686,	CPU_Cyrix,	CPU_SMM},
#line 1329 "x86insn_nasm.gperf"
    {"vpclmulhqhqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x11,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 599 "x86insn_nasm.gperf"
    {"pclmullqlqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x00,	0,	0,	0,	CPU_AVX,	0,	0},
#line 863 "x86insn_nasm.gperf"
    {"punpckhdq",	mmxsse2_insn,	2,	NONE,	0x6A,	0,	0,	0,	CPU_MMX,	0,	0},
#line 262 "x86insn_nasm.gperf"
    {"cvtsd2ss",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5A,	0,	0,	CPU_SSE2,	0,	0},
#line 394 "x86insn_nasm.gperf"
    {"fucom",	fcom2_insn,	2,	NONE,	0xDD,	0xE0,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 600 "x86insn_nasm.gperf"
    {"pclmulqdq",	pclmulqdq_insn,	2,	NONE,	0x3A,	0x44,	0,	0,	CPU_AVX,	0,	0},
#line 1383 "x86insn_nasm.gperf"
    {"vpmovsxwd",	sse4m64_insn,	2,	NONE,	0x23,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 298 "x86insn_nasm.gperf"
    {"fcmovbe",	fcmovcc_insn,	1,	NONE,	0xDA,	0xD0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 902 "x86insn_nasm.gperf"
    {"rsdc",	rsdc_insn,	1,	NONE,	0,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 1456 "x86insn_nasm.gperf"
    {"vucomisd",	avx_xmm_xmm64_insn,	2,	NONE,	0x66,	0x2E,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1320 "x86insn_nasm.gperf"
    {"vpaddusw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDD,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1222 "x86insn_nasm.gperf"
    {"vfnmadd213pd",	vfma_pd_insn,	2,	NONE,	0xAC,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 370 "x86insn_nasm.gperf"
    {"fprem1",	twobyte_insn,	1,	NONE,	0xD9,	0xF5,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 709 "x86insn_nasm.gperf"
    {"pfmin",	now3d_insn,	1,	NONE,	0x94,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 183 "x86insn_nasm.gperf"
    {"comnltps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1036 "x86insn_nasm.gperf"
    {"vcmpeqsd",	ssecmp_64_insn,	4,	NONE,	0x00,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 479 "x86insn_nasm.gperf"
    {"lidt",	twobytemem_insn,	1,	NONE,	0x03,	0x0F,	0x01,	0,	CPU_286,	CPU_Priv,	0},
#line 409 "x86insn_nasm.gperf"
    {"hlt",	onebyte_insn,	1,	NONE,	0xF4,	0,	0,	0,	CPU_Priv,	0,	0},
#line 1325 "x86insn_nasm.gperf"
    {"vpavgb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE0,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 520 "x86insn_nasm.gperf"
    {"movhlps",	movhllhps_insn,	2,	NONE,	0x12,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1216 "x86insn_nasm.gperf"
    {"vfmsubadd231pd",	vfma_pd_insn,	2,	NONE,	0xB7,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 578 "x86insn_nasm.gperf"
    {"paddd",	mmxsse2_insn,	2,	NONE,	0xFE,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1386 "x86insn_nasm.gperf"
    {"vpmovzxbq",	sse4m16_insn,	2,	NONE,	0x32,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 542 "x86insn_nasm.gperf"
    {"movsq",	onebyte_insn,	1,	NONE,	0xA5,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 880 "x86insn_nasm.gperf"
    {"rcpps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x53,	0,	0,	CPU_SSE,	0,	0},
#line 1121 "x86insn_nasm.gperf"
    {"vcmpnlt_uqss",	ssecmp_32_insn,	4,	NONE,	0x15,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 993 "x86insn_nasm.gperf"
    {"ud2",	twobyte_insn,	1,	NONE,	0x0F,	0x0B,	0,	0,	CPU_286,	0,	0},
#line 812 "x86insn_nasm.gperf"
    {"por",	mmxsse2_insn,	2,	NONE,	0xEB,	0,	0,	0,	CPU_MMX,	0,	0},
#line 694 "x86insn_nasm.gperf"
    {"pdistib",	cyrixmmx_insn,	1,	NONE,	0x54,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 577 "x86insn_nasm.gperf"
    {"paddb",	mmxsse2_insn,	2,	NONE,	0xFC,	0,	0,	0,	CPU_MMX,	0,	0},
#line 745 "x86insn_nasm.gperf"
    {"pinsrd",	pinsrd_insn,	2,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_SSE41,	0},
#line 552 "x86insn_nasm.gperf"
    {"mulpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x59,	0,	0,	CPU_SSE2,	0,	0},
#line 588 "x86insn_nasm.gperf"
    {"pandn",	mmxsse2_insn,	2,	NONE,	0xDF,	0,	0,	0,	CPU_MMX,	0,	0},
#line 805 "x86insn_nasm.gperf"
    {"popad",	onebyte_insn,	1,	NONE,	0x61,	0x20,	0,	NOT_64,	CPU_386,	0,	0},
#line 952 "x86insn_nasm.gperf"
    {"shrd",	shlrd_insn,	9,	NONE,	0xAC,	0,	0,	0,	CPU_386,	0,	0},
#line 721 "x86insn_nasm.gperf"
    {"phaddbq",	sse5two_insn,	1,	NONE,	0x43,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1073 "x86insn_nasm.gperf"
    {"vcmplt_oqss",	ssecmp_32_insn,	4,	NONE,	0x11,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 91 "x86insn_nasm.gperf"
    {"cmovz",	cmovcc_insn,	3,	NONE,	0x04,	0,	0,	0,	CPU_686,	0,	0},
#line 1420 "x86insn_nasm.gperf"
    {"vpsubsb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE8,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 381 "x86insn_nasm.gperf"
    {"fsin",	twobyte_insn,	1,	NONE,	0xD9,	0xFE,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 947 "x86insn_nasm.gperf"
    {"sfence",	threebyte_insn,	1,	NONE,	0x0F,	0xAE,	0xF8,	0,	CPU_P3,	0,	0},
#line 1071 "x86insn_nasm.gperf"
    {"vcmplt_oqps",	ssecmp_128_insn,	3,	NONE,	0x11,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1159 "x86insn_nasm.gperf"
    {"vcvtpd2ps",	avx_cvt_xmm128_insn,	2,	NONE,	0x66,	0x5A,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1332 "x86insn_nasm.gperf"
    {"vpclmullqlqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x00,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1385 "x86insn_nasm.gperf"
    {"vpmovzxbd",	sse4m32_insn,	2,	NONE,	0x31,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 679 "x86insn_nasm.gperf"
    {"pcomnew",	sse5comcc_insn,	1,	NONE,	0x4D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 34 "x86insn_nasm.gperf"
    {"andnpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x55,	0,	0,	CPU_SSE2,	0,	0},
#line 1042 "x86insn_nasm.gperf"
    {"vcmpfalsepd",	ssecmp_128_insn,	3,	NONE,	0x0B,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1426 "x86insn_nasm.gperf"
    {"vpunpckhbw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x68,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 685 "x86insn_nasm.gperf"
    {"pcomtrueud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1128 "x86insn_nasm.gperf"
    {"vcmpord_ssd",	ssecmp_64_insn,	4,	NONE,	0x17,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1072 "x86insn_nasm.gperf"
    {"vcmplt_oqsd",	ssecmp_64_insn,	4,	NONE,	0x11,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 100 "x86insn_nasm.gperf"
    {"cmpless",	ssecmp_32_insn,	4,	NONE,	0x02,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 612 "x86insn_nasm.gperf"
    {"pcmpistri",	sse4pcmpstr_insn,	1,	NONE,	0x63,	0,	0,	0,	CPU_SSE42,	0,	0},
#line 869 "x86insn_nasm.gperf"
    {"punpcklwd",	mmxsse2_insn,	2,	NONE,	0x61,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1048 "x86insn_nasm.gperf"
    {"vcmpge_oqsd",	ssecmp_64_insn,	4,	NONE,	0x1D,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 41 "x86insn_nasm.gperf"
    {"blendvpd",	sse4xmm0_insn,	2,	NONE,	0x15,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1446 "x86insn_nasm.gperf"
    {"vsqrtps",	avx_xmm_xmm128_insn,	2,	NONE,	0x00,	0x51,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 785 "x86insn_nasm.gperf"
    {"pmovzxbw",	sse4m64_insn,	2,	NONE,	0x30,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1424 "x86insn_nasm.gperf"
    {"vpsubw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xF9,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 768 "x86insn_nasm.gperf"
    {"pmaxud",	sse4_insn,	2,	NONE,	0x3F,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1441 "x86insn_nasm.gperf"
    {"vrsqrtps",	avx_xmm_xmm128_insn,	2,	NONE,	0x00,	0x52,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 149 "x86insn_nasm.gperf"
    {"comgtps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 344 "x86insn_nasm.gperf"
    {"fmaddps",	sse5arith_insn,	4,	NONE,	0x00,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1476 "x86insn_nasm.gperf"
    {"xcryptecb",	padlock_insn,	1,	NONE,	0xC8,	0xF3,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 1110 "x86insn_nasm.gperf"
    {"vcmpnle_uqpd",	ssecmp_128_insn,	3,	NONE,	0x16,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 833 "x86insn_nasm.gperf"
    {"pshufb",	ssse3_insn,	3,	NONE,	0x00,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 922 "x86insn_nasm.gperf"
    {"sete",	setcc_insn,	1,	NONE,	0x04,	0,	0,	0,	CPU_386,	0,	0},
#line 748 "x86insn_nasm.gperf"
    {"pmachriw",	pmachriw_insn,	1,	NONE,	0,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 407 "x86insn_nasm.gperf"
    {"haddpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x7C,	0,	0,	CPU_SSE3,	0,	0},
#line 1283 "x86insn_nasm.gperf"
    {"vmovntps",	movnt_insn,	2,	NONE,	0x00,	0x2B,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1272 "x86insn_nasm.gperf"
    {"vmovhlps",	movhllhps_insn,	2,	NONE,	0x12,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 872 "x86insn_nasm.gperf"
    {"pushad",	onebyte_insn,	1,	NONE,	0x60,	0x20,	0,	NOT_64,	CPU_386,	0,	0},
#line 223 "x86insn_nasm.gperf"
    {"comuneqss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 1162 "x86insn_nasm.gperf"
    {"vcvtsd2si",	cvt_rx_xmm64_insn,	4,	NONE,	0xF2,	0x2D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 877 "x86insn_nasm.gperf"
    {"pushfw",	onebyte_insn,	1,	NONE,	0x9C,	0x10,	0x40,	0,	0,	0,	0},
#line 250 "x86insn_nasm.gperf"
    {"cvtdq2ps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x5B,	0,	0,	CPU_SSE2,	0,	0},
#line 60 "x86insn_nasm.gperf"
    {"clts",	twobyte_insn,	1,	NONE,	0x0F,	0x06,	0,	0,	CPU_286,	CPU_Priv,	0},
#line 1150 "x86insn_nasm.gperf"
    {"vcmpunordpd",	ssecmp_128_insn,	3,	NONE,	0x03,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1365 "x86insn_nasm.gperf"
    {"vpmaddwd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xF5,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 353 "x86insn_nasm.gperf"
    {"fnclex",	twobyte_insn,	1,	NONE,	0xDB,	0xE2,	0,	0,	CPU_FPU,	0,	0},
#line 420 "x86insn_nasm.gperf"
    {"insertq",	insertq_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SSE4a,	0,	0},
#line 964 "x86insn_nasm.gperf"
    {"sqrtsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x51,	0,	0,	CPU_SSE2,	0,	0},
#line 257 "x86insn_nasm.gperf"
    {"cvtps2dq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5B,	0,	0,	CPU_SSE2,	0,	0},
#line 985 "x86insn_nasm.gperf"
    {"syscall",	twobyte_insn,	1,	NONE,	0x0F,	0x05,	0,	0,	CPU_686,	CPU_AMD,	0},
#line 305 "x86insn_nasm.gperf"
    {"fcom",	fcom_insn,	6,	NONE,	0xD0,	0x02,	0,	0,	CPU_FPU,	0,	0},
#line 596 "x86insn_nasm.gperf"
    {"pclmulhqhqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x11,	0,	0,	0,	CPU_AVX,	0,	0},
#line 1180 "x86insn_nasm.gperf"
    {"vextractf128",	vextractf128_insn,	1,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 241 "x86insn_nasm.gperf"
    {"comunltss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 1146 "x86insn_nasm.gperf"
    {"vcmpunord_spd",	ssecmp_128_insn,	3,	NONE,	0x13,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 615 "x86insn_nasm.gperf"
    {"pcomd",	sse5com_insn,	1,	NONE,	0x4E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1461 "x86insn_nasm.gperf"
    {"vunpcklps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x14,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1060 "x86insn_nasm.gperf"
    {"vcmpgtsd",	ssecmp_64_insn,	4,	NONE,	0x0E,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 761 "x86insn_nasm.gperf"
    {"pmaddubsw",	ssse3_insn,	3,	NONE,	0x04,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1414 "x86insn_nasm.gperf"
    {"vpsrldq",	pslrldq_insn,	2,	NONE,	0x03,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 503 "x86insn_nasm.gperf"
    {"maxss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5F,	0,	0,	CPU_SSE,	0,	0},
#line 320 "x86insn_nasm.gperf"
    {"ficom",	fiarith_insn,	2,	NONE,	0x02,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 138 "x86insn_nasm.gperf"
    {"comeqsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 470 "x86insn_nasm.gperf"
    {"ldmxcsr",	ldstmxcsr_insn,	1,	NONE,	0x02,	0,	0,	0,	CPU_SSE,	0,	0},
#line 153 "x86insn_nasm.gperf"
    {"comiss",	xmm_xmm32_insn,	4,	NONE,	0x00,	0x2F,	0,	0,	CPU_SSE,	0,	0},
#line 603 "x86insn_nasm.gperf"
    {"pcmpeqd",	mmxsse2_insn,	2,	NONE,	0x76,	0,	0,	0,	CPU_MMX,	0,	0},
#line 96 "x86insn_nasm.gperf"
    {"cmpeqss",	ssecmp_32_insn,	4,	NONE,	0x00,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 899 "x86insn_nasm.gperf"
    {"roundps",	sse4imm_insn,	2,	NONE,	0x08,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 705 "x86insn_nasm.gperf"
    {"pfcmpeq",	now3d_insn,	1,	NONE,	0xB0,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 280 "x86insn_nasm.gperf"
    {"divps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x5E,	0,	0,	CPU_SSE,	0,	0},
#line 793 "x86insn_nasm.gperf"
    {"pmulhrwc",	cyrixmmx_insn,	1,	NONE,	0x59,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 256 "x86insn_nasm.gperf"
    {"cvtpi2ps",	cvt_xmm_mm_ps_insn,	1,	NONE,	0x2A,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1141 "x86insn_nasm.gperf"
    {"vcmptrue_usss",	ssecmp_32_insn,	4,	NONE,	0x1F,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 718 "x86insn_nasm.gperf"
    {"pfsub",	now3d_insn,	1,	NONE,	0x9A,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 753 "x86insn_nasm.gperf"
    {"pmacssdqh",	sse5pmacs_insn,	1,	NONE,	0x8F,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 154 "x86insn_nasm.gperf"
    {"comlepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 926 "x86insn_nasm.gperf"
    {"setle",	setcc_insn,	1,	NONE,	0x0E,	0,	0,	0,	CPU_386,	0,	0},
#line 411 "x86insn_nasm.gperf"
    {"hsubps",	xmm_xmm128_insn,	2,	NONE,	0xF2,	0x7D,	0,	0,	CPU_SSE3,	0,	0},
#line 701 "x86insn_nasm.gperf"
    {"pf2id",	now3d_insn,	1,	NONE,	0x1D,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 901 "x86insn_nasm.gperf"
    {"roundss",	sse4m32imm_insn,	4,	NONE,	0x0A,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 955 "x86insn_nasm.gperf"
    {"sidt",	twobytemem_insn,	1,	NONE,	0x01,	0x0F,	0x01,	0,	CPU_286,	CPU_Priv,	0},
#line 535 "x86insn_nasm.gperf"
    {"movntss",	movntss_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE4a,	0,	0},
#line 743 "x86insn_nasm.gperf"
    {"pi2fw",	now3d_insn,	1,	NONE,	0x0C,	0,	0,	0,	CPU_3DNow,	CPU_Athlon,	0},
#line 866 "x86insn_nasm.gperf"
    {"punpcklbw",	mmxsse2_insn,	2,	NONE,	0x60,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1124 "x86insn_nasm.gperf"
    {"vcmpnltsd",	ssecmp_64_insn,	4,	NONE,	0x05,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 904 "x86insn_nasm.gperf"
    {"rsm",	twobyte_insn,	1,	NONE,	0x0F,	0xAA,	0,	0,	CPU_586,	CPU_SMM,	0},
#line 1253 "x86insn_nasm.gperf"
    {"vmaxpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x5F,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1428 "x86insn_nasm.gperf"
    {"vpunpckhqdq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1056 "x86insn_nasm.gperf"
    {"vcmpgt_oqsd",	ssecmp_64_insn,	4,	NONE,	0x1E,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1228 "x86insn_nasm.gperf"
    {"vfnmadd231sd",	vfma_sd_insn,	2,	NONE,	0xBD,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 128 "x86insn_nasm.gperf"
    {"cmpunordpd",	ssecmp_128_insn,	3,	NONE,	0x03,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 984 "x86insn_nasm.gperf"
    {"swapgs",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xF8,	ONLY_64,	0,	0,	0},
#line 950 "x86insn_nasm.gperf"
    {"shld",	shlrd_insn,	9,	NONE,	0xA4,	0,	0,	0,	CPU_386,	0,	0},
#line 1084 "x86insn_nasm.gperf"
    {"vcmpneq_ossd",	ssecmp_64_insn,	4,	NONE,	0x1C,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 260 "x86insn_nasm.gperf"
    {"cvtps2pi",	cvt_mm_xmm64_insn,	2,	NONE,	0x2D,	0,	0,	0,	CPU_SSE,	0,	0},
#line 760 "x86insn_nasm.gperf"
    {"pmadcswd",	sse5pmacs_insn,	1,	NONE,	0xB6,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 284 "x86insn_nasm.gperf"
    {"dpps",	sse4imm_insn,	2,	NONE,	0x40,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 442 "x86insn_nasm.gperf"
    {"jge",	jcc_insn,	9,	NONE,	0x0D,	0,	0,	0,	0,	0,	0},
#line 235 "x86insn_nasm.gperf"
    {"comunleps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 706 "x86insn_nasm.gperf"
    {"pfcmpge",	now3d_insn,	1,	NONE,	0x90,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 1324 "x86insn_nasm.gperf"
    {"vpandn",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDF,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 77 "x86insn_nasm.gperf"
    {"cmovne",	cmovcc_insn,	3,	NONE,	0x05,	0,	0,	0,	CPU_686,	0,	0},
#line 590 "x86insn_nasm.gperf"
    {"paveb",	cyrixmmx_insn,	1,	NONE,	0x50,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 345 "x86insn_nasm.gperf"
    {"fmaddsd",	sse5arith64_insn,	8,	NONE,	0x03,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1191 "x86insn_nasm.gperf"
    {"vfmadd231ps",	vfma_ps_insn,	2,	NONE,	0xB8,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 71 "x86insn_nasm.gperf"
    {"cmovle",	cmovcc_insn,	3,	NONE,	0x0E,	0,	0,	0,	CPU_686,	0,	0},
#line 959 "x86insn_nasm.gperf"
    {"smint",	twobyte_insn,	1,	NONE,	0x0F,	0x38,	0,	0,	CPU_686,	CPU_Cyrix,	0},
#line 1356 "x86insn_nasm.gperf"
    {"vphminposuw",	avx_ssse3_2op_insn,	1,	NONE,	0x41,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 939 "x86insn_nasm.gperf"
    {"setns",	setcc_insn,	1,	NONE,	0x09,	0,	0,	0,	CPU_386,	0,	0},
#line 377 "x86insn_nasm.gperf"
    {"frstor",	onebytemem_insn,	1,	NONE,	0x04,	0xDD,	0,	0,	CPU_FPU,	0,	0},
#line 1198 "x86insn_nasm.gperf"
    {"vfmaddsub231pd",	vfma_pd_insn,	2,	NONE,	0xB6,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 560 "x86insn_nasm.gperf"
    {"o16",	NULL,	X86_OPERSIZE>>8,	0x10,	0,	0,	0,	0,	0,	0,	0},
#line 967 "x86insn_nasm.gperf"
    {"std",	onebyte_insn,	1,	NONE,	0xFD,	0,	0,	0,	0,	0,	0},
#line 460 "x86insn_nasm.gperf"
    {"jo",	jcc_insn,	9,	NONE,	0x00,	0,	0,	0,	0,	0,	0},
#line 192 "x86insn_nasm.gperf"
    {"comsd",	sse5com64_insn,	2,	NONE,	0x2F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1358 "x86insn_nasm.gperf"
    {"vphsubsw",	ssse3_insn,	3,	NONE,	0x07,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 48 "x86insn_nasm.gperf"
    {"btc",	bittest_insn,	6,	NONE,	0xBB,	0x07,	0,	0,	CPU_386,	0,	0},
#line 290 "x86insn_nasm.gperf"
    {"fabs",	twobyte_insn,	1,	NONE,	0xD9,	0xE1,	0,	0,	CPU_FPU,	0,	0},
#line 1406 "x86insn_nasm.gperf"
    {"vpsignw",	ssse3_insn,	3,	NONE,	0x09,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 547 "x86insn_nasm.gperf"
    {"movupd",	movau_insn,	6,	NONE,	0x66,	0x10,	0x01,	0,	CPU_SSE2,	0,	0},
#line 1021 "x86insn_nasm.gperf"
    {"vbroadcastss",	vbroadcastss_insn,	2,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1248 "x86insn_nasm.gperf"
    {"vlddqu",	lddqu_insn,	2,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1099 "x86insn_nasm.gperf"
    {"vcmpngeps",	ssecmp_128_insn,	3,	NONE,	0x09,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 43 "x86insn_nasm.gperf"
    {"bound",	bound_insn,	2,	NONE,	0,	0,	0,	NOT_64,	CPU_186,	0,	0},
#line 975 "x86insn_nasm.gperf"
    {"str",	str_insn,	4,	NONE,	0,	0,	0,	0,	CPU_286,	CPU_Prot,	0},
#line 1013 "x86insn_nasm.gperf"
    {"vandpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x54,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1166 "x86insn_nasm.gperf"
    {"vcvtss2sd",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5A,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1010 "x86insn_nasm.gperf"
    {"vaeskeygenassist",	aes_imm_insn,	1,	NONE,	0x3A,	0xDF,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 412 "x86insn_nasm.gperf"
    {"ibts",	ibts_insn,	2,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_Obs,	CPU_Undoc},
#line 438 "x86insn_nasm.gperf"
    {"jcxz",	jcxz_insn,	2,	NONE,	0x10,	0,	0,	0,	0,	0,	0},
#line 617 "x86insn_nasm.gperf"
    {"pcomeqd",	sse5comcc_insn,	1,	NONE,	0x4E,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 172 "x86insn_nasm.gperf"
    {"comngesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 469 "x86insn_nasm.gperf"
    {"lddqu",	lddqu_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SSE3,	0,	0},
#line 229 "x86insn_nasm.gperf"
    {"comungess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 934 "x86insn_nasm.gperf"
    {"setnge",	setcc_insn,	1,	NONE,	0x0C,	0,	0,	0,	CPU_386,	0,	0},
#line 523 "x86insn_nasm.gperf"
    {"movlhps",	movhllhps_insn,	2,	NONE,	0x16,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1081 "x86insn_nasm.gperf"
    {"vcmpneq_oqss",	ssecmp_32_insn,	4,	NONE,	0x0C,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 770 "x86insn_nasm.gperf"
    {"pminsb",	sse4_insn,	2,	NONE,	0x38,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 932 "x86insn_nasm.gperf"
    {"setne",	setcc_insn,	1,	NONE,	0x05,	0,	0,	0,	CPU_386,	0,	0},
#line 16 "x86insn_nasm.gperf"
    {"aad",	aadm_insn,	2,	NONE,	0x01,	0,	0,	NOT_64,	0,	0,	0},
#line 1016 "x86insn_nasm.gperf"
    {"vblendps",	sse4imm_256_insn,	3,	NONE,	0x0C,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1006 "x86insn_nasm.gperf"
    {"vaesdeclast",	aes_insn,	2,	NONE,	0x38,	0xDF,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 771 "x86insn_nasm.gperf"
    {"pminsd",	sse4_insn,	2,	NONE,	0x39,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 13 "x86insn_nasm.gperf"
    {"a32",	NULL,	X86_ADDRSIZE>>8,	0x20,	0,	0,	0,	0,	0,	0,	0},
#line 938 "x86insn_nasm.gperf"
    {"setnp",	setcc_insn,	1,	NONE,	0x0B,	0,	0,	0,	CPU_386,	0,	0},
#line 92 "x86insn_nasm.gperf"
    {"cmp",	arith_insn,	22,	NONE,	0x38,	0x07,	0,	0,	0,	0,	0},
#line 27 "x86insn_nasm.gperf"
    {"aesdec",	aes_insn,	2,	NONE,	0x38,	0xDE,	0,	0,	CPU_AVX,	0,	0},
#line 498 "x86insn_nasm.gperf"
    {"maskmovdqu",	maskmovdqu_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 66 "x86insn_nasm.gperf"
    {"cmovc",	cmovcc_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_686,	0,	0},
#line 440 "x86insn_nasm.gperf"
    {"jecxz",	jcxz_insn,	2,	NONE,	0x20,	0,	0,	0,	CPU_386,	0,	0},
#line 982 "x86insn_nasm.gperf"
    {"svldt",	cyrixsmm_insn,	1,	NONE,	0x7A,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 129 "x86insn_nasm.gperf"
    {"cmpunordps",	ssecmp_128_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_SSE,	0,	0},
#line 97 "x86insn_nasm.gperf"
    {"cmplepd",	ssecmp_128_insn,	3,	NONE,	0x02,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 17 "x86insn_nasm.gperf"
    {"aam",	aadm_insn,	2,	NONE,	0x00,	0,	0,	NOT_64,	0,	0,	0},
#line 1152 "x86insn_nasm.gperf"
    {"vcmpunordsd",	ssecmp_64_insn,	4,	NONE,	0x03,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1321 "x86insn_nasm.gperf"
    {"vpaddw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xFD,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 426 "x86insn_nasm.gperf"
    {"invd",	twobyte_insn,	1,	NONE,	0x0F,	0x08,	0,	0,	CPU_486,	CPU_Priv,	0},
#line 477 "x86insn_nasm.gperf"
    {"lgdt",	twobytemem_insn,	1,	NONE,	0x02,	0x0F,	0x01,	0,	CPU_286,	CPU_Priv,	0},
#line 1330 "x86insn_nasm.gperf"
    {"vpclmulhqlqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x01,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1225 "x86insn_nasm.gperf"
    {"vfnmadd213ss",	vfma_ss_insn,	2,	NONE,	0xAD,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 915 "x86insn_nasm.gperf"
    {"scasq",	onebyte_insn,	1,	NONE,	0xAF,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 906 "x86insn_nasm.gperf"
    {"rsqrtss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x52,	0,	0,	CPU_SSE,	0,	0},
#line 274 "x86insn_nasm.gperf"
    {"cwde",	onebyte_insn,	1,	NONE,	0x98,	0x20,	0,	0,	CPU_386,	0,	0},
#line 69 "x86insn_nasm.gperf"
    {"cmovge",	cmovcc_insn,	3,	NONE,	0x0D,	0,	0,	0,	CPU_686,	0,	0},
#line 888 "x86insn_nasm.gperf"
    {"rep",	NULL,	X86_LOCKREP>>8,	0xF3,	0,	0,	0,	0,	0,	0,	0},
#line 169 "x86insn_nasm.gperf"
    {"comness",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 824 "x86insn_nasm.gperf"
    {"psadbw",	mmxsse2_insn,	2,	NONE,	0xF6,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 372 "x86insn_nasm.gperf"
    {"frczpd",	sse5two_insn,	1,	NONE,	0x11,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 132 "x86insn_nasm.gperf"
    {"cmpxchg",	cmpxchgxadd_insn,	4,	NONE,	0xB0,	0,	0,	0,	CPU_486,	0,	0},
#line 245 "x86insn_nasm.gperf"
    {"comunordss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 716 "x86insn_nasm.gperf"
    {"pfrsqit1",	now3d_insn,	1,	NONE,	0xA7,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 299 "x86insn_nasm.gperf"
    {"fcmove",	fcmovcc_insn,	1,	NONE,	0xDA,	0xC8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1372 "x86insn_nasm.gperf"
    {"vpminsb",	ssse3_insn,	3,	NONE,	0x38,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 311 "x86insn_nasm.gperf"
    {"fdecstp",	twobyte_insn,	1,	NONE,	0xD9,	0xF6,	0,	0,	CPU_FPU,	0,	0},
#line 1447 "x86insn_nasm.gperf"
    {"vsqrtsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x51,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 828 "x86insn_nasm.gperf"
    {"pshaw",	sse5psh_insn,	2,	NONE,	0x05,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 559 "x86insn_nasm.gperf"
    {"not",	f6_insn,	4,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 136 "x86insn_nasm.gperf"
    {"comeqpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 457 "x86insn_nasm.gperf"
    {"jnp",	jcc_insn,	9,	NONE,	0x0B,	0,	0,	0,	0,	0,	0},
#line 20 "x86insn_nasm.gperf"
    {"add",	arith_insn,	22,	NONE,	0x00,	0x00,	0,	0,	0,	0,	0},
#line 958 "x86insn_nasm.gperf"
    {"smi",	onebyte_insn,	1,	NONE,	0xF1,	0,	0,	0,	CPU_386,	CPU_Undoc,	0},
#line 903 "x86insn_nasm.gperf"
    {"rsldt",	cyrixsmm_insn,	1,	NONE,	0x7B,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 512 "x86insn_nasm.gperf"
    {"movapd",	movau_insn,	6,	NONE,	0x66,	0x28,	0x01,	0,	CPU_SSE2,	0,	0},
#line 439 "x86insn_nasm.gperf"
    {"je",	jcc_insn,	9,	NONE,	0x04,	0,	0,	0,	0,	0,	0},
#line 1400 "x86insn_nasm.gperf"
    {"vpshufb",	ssse3_insn,	3,	NONE,	0x00,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 656 "x86insn_nasm.gperf"
    {"pcomltb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1457 "x86insn_nasm.gperf"
    {"vucomiss",	avx_xmm_xmm32_insn,	2,	NONE,	0x00,	0x2E,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1310 "x86insn_nasm.gperf"
    {"vpackssdw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6B,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1260 "x86insn_nasm.gperf"
    {"vminps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x5D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 401 "x86insn_nasm.gperf"
    {"fxch",	fxch_insn,	4,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 1411 "x86insn_nasm.gperf"
    {"vpsrad",	vpshift_insn,	4,	NONE,	0xE2,	0x72,	0x04,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 551 "x86insn_nasm.gperf"
    {"mul",	f6_insn,	4,	NONE,	0x04,	0,	0,	0,	0,	0,	0},
#line 1196 "x86insn_nasm.gperf"
    {"vfmaddsub213pd",	vfma_pd_insn,	2,	NONE,	0xA6,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1095 "x86insn_nasm.gperf"
    {"vcmpnge_uqps",	ssecmp_128_insn,	3,	NONE,	0x19,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1434 "x86insn_nasm.gperf"
    {"vpxor",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xEF,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 515 "x86insn_nasm.gperf"
    {"movd",	movd_insn,	8,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_MMX,	0},
#line 533 "x86insn_nasm.gperf"
    {"movntq",	movntq_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE,	0,	0},
#line 736 "x86insn_nasm.gperf"
    {"phsubbw",	sse5two_insn,	1,	NONE,	0x61,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 273 "x86insn_nasm.gperf"
    {"cwd",	onebyte_insn,	1,	NONE,	0x99,	0x10,	0,	0,	0,	0,	0},
#line 450 "x86insn_nasm.gperf"
    {"jnc",	jcc_insn,	9,	NONE,	0x03,	0,	0,	0,	0,	0,	0},
#line 1047 "x86insn_nasm.gperf"
    {"vcmpge_oqps",	ssecmp_128_insn,	3,	NONE,	0x1D,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1092 "x86insn_nasm.gperf"
    {"vcmpneqsd",	ssecmp_64_insn,	4,	NONE,	0x04,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 911 "x86insn_nasm.gperf"
    {"sar",	shift_insn,	16,	NONE,	0x07,	0,	0,	0,	0,	0,	0},
#line 627 "x86insn_nasm.gperf"
    {"pcomfalseub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 198 "x86insn_nasm.gperf"
    {"comueqpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x08,	0,	0,	CPU_SSE5,	0,	0},
#line 692 "x86insn_nasm.gperf"
    {"pcomuw",	sse5com_insn,	1,	NONE,	0x6D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 613 "x86insn_nasm.gperf"
    {"pcmpistrm",	sse4pcmpstr_insn,	1,	NONE,	0x62,	0,	0,	0,	CPU_SSE42,	0,	0},
#line 1224 "x86insn_nasm.gperf"
    {"vfnmadd213sd",	vfma_sd_insn,	2,	NONE,	0xAD,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1088 "x86insn_nasm.gperf"
    {"vcmpneq_ussd",	ssecmp_64_insn,	4,	NONE,	0x14,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 414 "x86insn_nasm.gperf"
    {"imul",	imul_insn,	19,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 972 "x86insn_nasm.gperf"
    {"stosd",	onebyte_insn,	1,	NONE,	0xAB,	0x20,	0,	0,	CPU_386,	0,	0},
#line 1469 "x86insn_nasm.gperf"
    {"wrshr",	rdwrshr_insn,	1,	NONE,	0x01,	0,	0,	0,	CPU_686,	CPU_Cyrix,	CPU_SMM},
#line 1262 "x86insn_nasm.gperf"
    {"vminss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 890 "x86insn_nasm.gperf"
    {"repne",	NULL,	X86_LOCKREP>>8,	0xF2,	0,	0,	0,	0,	0,	0,	0},
#line 400 "x86insn_nasm.gperf"
    {"fxam",	twobyte_insn,	1,	NONE,	0xD9,	0xE5,	0,	0,	CPU_FPU,	0,	0},
#line 994 "x86insn_nasm.gperf"
    {"umov",	umov_insn,	6,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_Undoc,	0},
#line 968 "x86insn_nasm.gperf"
    {"stgi",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xDC,	0,	CPU_SVM,	0,	0},
#line 765 "x86insn_nasm.gperf"
    {"pmaxsd",	sse4_insn,	2,	NONE,	0x3D,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 160 "x86insn_nasm.gperf"
    {"comltsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 729 "x86insn_nasm.gperf"
    {"phaddudq",	sse5two_insn,	1,	NONE,	0x5B,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 151 "x86insn_nasm.gperf"
    {"comgtss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 1054 "x86insn_nasm.gperf"
    {"vcmpgt_oqpd",	ssecmp_128_insn,	3,	NONE,	0x1E,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 788 "x86insn_nasm.gperf"
    {"pmovzxwq",	sse4m32_insn,	2,	NONE,	0x34,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 514 "x86insn_nasm.gperf"
    {"movbe",	movbe_insn,	6,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 87 "x86insn_nasm.gperf"
    {"cmovp",	cmovcc_insn,	3,	NONE,	0x0A,	0,	0,	0,	CPU_686,	0,	0},
#line 200 "x86insn_nasm.gperf"
    {"comueqsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x08,	0,	0,	CPU_SSE5,	0,	0},
#line 755 "x86insn_nasm.gperf"
    {"pmacsswd",	sse5pmacs_insn,	1,	NONE,	0x86,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 395 "x86insn_nasm.gperf"
    {"fucomi",	fcom2_insn,	2,	NONE,	0xDB,	0xE8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 422 "x86insn_nasm.gperf"
    {"int",	int_insn,	1,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 475 "x86insn_nasm.gperf"
    {"lfence",	threebyte_insn,	1,	NONE,	0x0F,	0xAE,	0xE8,	0,	CPU_P3,	0,	0},
#line 573 "x86insn_nasm.gperf"
    {"packssdw",	mmxsse2_insn,	2,	NONE,	0x6B,	0,	0,	0,	CPU_MMX,	0,	0},
#line 157 "x86insn_nasm.gperf"
    {"comless",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 239 "x86insn_nasm.gperf"
    {"comunltps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 159 "x86insn_nasm.gperf"
    {"comltps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 816 "x86insn_nasm.gperf"
    {"prefetcht0",	twobytemem_insn,	1,	NONE,	0x01,	0x0F,	0x18,	0,	CPU_P3,	0,	0},
#line 930 "x86insn_nasm.gperf"
    {"setnbe",	setcc_insn,	1,	NONE,	0x07,	0,	0,	0,	CPU_386,	0,	0},
#line 1448 "x86insn_nasm.gperf"
    {"vsqrtss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x51,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1120 "x86insn_nasm.gperf"
    {"vcmpnlt_uqsd",	ssecmp_64_insn,	4,	NONE,	0x15,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1323 "x86insn_nasm.gperf"
    {"vpand",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xDB,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1062 "x86insn_nasm.gperf"
    {"vcmple_oqpd",	ssecmp_128_insn,	3,	NONE,	0x12,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 335 "x86insn_nasm.gperf"
    {"fldcw",	fldnstcw_insn,	1,	NONE,	0x05,	0,	0,	0,	CPU_FPU,	0,	0},
#line 937 "x86insn_nasm.gperf"
    {"setno",	setcc_insn,	1,	NONE,	0x01,	0,	0,	0,	CPU_386,	0,	0},
#line 1019 "x86insn_nasm.gperf"
    {"vbroadcastf128",	vbroadcastf128_insn,	1,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1481 "x86insn_nasm.gperf"
    {"xorpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x57,	0,	0,	CPU_SSE2,	0,	0},
#line 355 "x86insn_nasm.gperf"
    {"fnmaddpd",	sse5arith_insn,	4,	NONE,	0x11,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 261 "x86insn_nasm.gperf"
    {"cvtsd2si",	cvt_rx_xmm64_insn,	4,	NONE,	0xF2,	0x2D,	0,	0,	CPU_386,	CPU_SSE2,	0},
#line 127 "x86insn_nasm.gperf"
    {"cmpsw",	onebyte_insn,	1,	NONE,	0xA7,	0x10,	0,	0,	0,	0,	0},
#line 1419 "x86insn_nasm.gperf"
    {"vpsubq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xFB,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 864 "x86insn_nasm.gperf"
    {"punpckhqdq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6D,	0,	0,	CPU_SSE2,	0,	0},
#line 525 "x86insn_nasm.gperf"
    {"movlps",	movhlp_insn,	3,	NONE,	0x00,	0x12,	0,	0,	CPU_SSE,	0,	0},
#line 144 "x86insn_nasm.gperf"
    {"comgepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 221 "x86insn_nasm.gperf"
    {"comuneqps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 857 "x86insn_nasm.gperf"
    {"psubusb",	mmxsse2_insn,	2,	NONE,	0xD8,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1328 "x86insn_nasm.gperf"
    {"vpblendw",	sse4imm_insn,	2,	NONE,	0x0E,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 568 "x86insn_nasm.gperf"
    {"outsd",	onebyte_insn,	1,	NONE,	0x6F,	0x20,	0,	0,	CPU_386,	0,	0},
#line 814 "x86insn_nasm.gperf"
    {"prefetch",	twobytemem_insn,	1,	NONE,	0x00,	0x0F,	0x0D,	0,	CPU_3DNow,	0,	0},
#line 1359 "x86insn_nasm.gperf"
    {"vphsubw",	ssse3_insn,	3,	NONE,	0x05,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 285 "x86insn_nasm.gperf"
    {"emms",	twobyte_insn,	1,	NONE,	0x0F,	0x77,	0,	0,	CPU_MMX,	0,	0},
#line 1286 "x86insn_nasm.gperf"
    {"vmovshdup",	avx_xmm_xmm128_insn,	2,	NONE,	0xF3,	0x16,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 992 "x86insn_nasm.gperf"
    {"ud1",	twobyte_insn,	1,	NONE,	0x0F,	0xB9,	0,	0,	CPU_286,	CPU_Undoc,	0},
#line 266 "x86insn_nasm.gperf"
    {"cvtss2si",	cvt_rx_xmm32_insn,	4,	NONE,	0xF3,	0x2D,	0,	0,	CPU_386,	CPU_SSE,	0},
#line 654 "x86insn_nasm.gperf"
    {"pcomleuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 504 "x86insn_nasm.gperf"
    {"mfence",	threebyte_insn,	1,	NONE,	0x0F,	0xAE,	0xF0,	0,	CPU_P3,	0,	0},
#line 329 "x86insn_nasm.gperf"
    {"fistp",	fildstp_insn,	4,	NONE,	0x03,	0x02,	0x07,	0,	CPU_FPU,	0,	0},
#line 33 "x86insn_nasm.gperf"
    {"and",	arith_insn,	22,	NONE,	0x20,	0x04,	0,	0,	0,	0,	0},
#line 925 "x86insn_nasm.gperf"
    {"setl",	setcc_insn,	1,	NONE,	0x0C,	0,	0,	0,	CPU_386,	0,	0},
#line 448 "x86insn_nasm.gperf"
    {"jnb",	jcc_insn,	9,	NONE,	0x03,	0,	0,	0,	0,	0,	0},
#line 54 "x86insn_nasm.gperf"
    {"cdqe",	onebyte_insn,	1,	NONE,	0x98,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 1059 "x86insn_nasm.gperf"
    {"vcmpgtps",	ssecmp_128_insn,	3,	NONE,	0x0E,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 699 "x86insn_nasm.gperf"
    {"pextrq",	pextrq_insn,	1,	NONE,	0,	0,	0,	ONLY_64,	CPU_SSE41,	0,	0},
#line 936 "x86insn_nasm.gperf"
    {"setnle",	setcc_insn,	1,	NONE,	0x0F,	0,	0,	0,	CPU_386,	0,	0},
#line 1055 "x86insn_nasm.gperf"
    {"vcmpgt_oqps",	ssecmp_128_insn,	3,	NONE,	0x1E,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1194 "x86insn_nasm.gperf"
    {"vfmaddsub132pd",	vfma_pd_insn,	2,	NONE,	0x96,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1171 "x86insn_nasm.gperf"
    {"vcvttss2si",	cvt_rx_xmm32_insn,	4,	NONE,	0xF3,	0x2C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 601 "x86insn_nasm.gperf"
    {"pcmov",	sse5arith_insn,	4,	NONE,	0x22,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 664 "x86insn_nasm.gperf"
    {"pcomneb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 86 "x86insn_nasm.gperf"
    {"cmovo",	cmovcc_insn,	3,	NONE,	0x00,	0,	0,	0,	CPU_686,	0,	0},
#line 674 "x86insn_nasm.gperf"
    {"pcomneqw",	sse5comcc_insn,	1,	NONE,	0x4D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 147 "x86insn_nasm.gperf"
    {"comgess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 889 "x86insn_nasm.gperf"
    {"repe",	NULL,	X86_LOCKREP>>8,	0xF3,	0,	0,	0,	0,	0,	0,	0},
#line 185 "x86insn_nasm.gperf"
    {"comnltss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1155 "x86insn_nasm.gperf"
    {"vcomiss",	avx_xmm_xmm32_insn,	2,	NONE,	0x00,	0x2F,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 391 "x86insn_nasm.gperf"
    {"fsubr",	farith_insn,	7,	NONE,	0xE0,	0xE8,	0x05,	0,	CPU_FPU,	0,	0},
#line 1023 "x86insn_nasm.gperf"
    {"vcmpeq_osps",	ssecmp_128_insn,	3,	NONE,	0x10,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1076 "x86insn_nasm.gperf"
    {"vcmpltsd",	ssecmp_64_insn,	4,	NONE,	0x01,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1284 "x86insn_nasm.gperf"
    {"vmovq",	vmovq_insn,	5,	NONE,	0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1351 "x86insn_nasm.gperf"
    {"vpextrq",	pextrq_insn,	1,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 860 "x86insn_nasm.gperf"
    {"pswapd",	now3d_insn,	1,	NONE,	0xBB,	0,	0,	0,	CPU_3DNow,	CPU_Athlon,	0},
#line 738 "x86insn_nasm.gperf"
    {"phsubdq",	sse5two_insn,	1,	NONE,	0x63,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 913 "x86insn_nasm.gperf"
    {"scasb",	onebyte_insn,	1,	NONE,	0xAE,	0x00,	0,	0,	0,	0,	0},
#line 929 "x86insn_nasm.gperf"
    {"setnb",	setcc_insn,	1,	NONE,	0x03,	0,	0,	0,	CPU_386,	0,	0},
#line 1102 "x86insn_nasm.gperf"
    {"vcmpngt_uqpd",	ssecmp_128_insn,	3,	NONE,	0x1A,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 493 "x86insn_nasm.gperf"
    {"loopz",	loop_insn,	8,	NONE,	0x01,	0,	0,	0,	0,	0,	0},
#line 295 "x86insn_nasm.gperf"
    {"fchs",	twobyte_insn,	1,	NONE,	0xD9,	0xE0,	0,	0,	CPU_FPU,	0,	0},
#line 1275 "x86insn_nasm.gperf"
    {"vmovlhps",	movhllhps_insn,	2,	NONE,	0x16,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1200 "x86insn_nasm.gperf"
    {"vfmsub132pd",	vfma_pd_insn,	2,	NONE,	0x9A,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1370 "x86insn_nasm.gperf"
    {"vpmaxud",	ssse3_insn,	3,	NONE,	0x3F,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1238 "x86insn_nasm.gperf"
    {"vfnmsub231pd",	vfma_pd_insn,	2,	NONE,	0xBE,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1037 "x86insn_nasm.gperf"
    {"vcmpeqss",	ssecmp_32_insn,	4,	NONE,	0x00,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1438 "x86insn_nasm.gperf"
    {"vroundps",	avx_sse4imm_insn,	3,	NONE,	0x08,	0,	0,	ONLY_AVX,	CPU_SSE41,	0,	0},
#line 1000 "x86insn_nasm.gperf"
    {"vaddps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x58,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 810 "x86insn_nasm.gperf"
    {"popfq",	onebyte_insn,	1,	NONE,	0x9D,	0x40,	0x40,	ONLY_64,	0,	0,	0},
#line 489 "x86insn_nasm.gperf"
    {"loop",	loop_insn,	8,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 1108 "x86insn_nasm.gperf"
    {"vcmpngtsd",	ssecmp_64_insn,	4,	NONE,	0x0A,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1165 "x86insn_nasm.gperf"
    {"vcvtsi2ss",	cvt_xmm_rmx_insn,	6,	NONE,	0xF3,	0x2A,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 464 "x86insn_nasm.gperf"
    {"jrcxz",	jcxz_insn,	2,	NONE,	0x40,	0,	0,	ONLY_64,	0,	0,	0},
#line 1295 "x86insn_nasm.gperf"
    {"vmresume",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC3,	0,	CPU_P4,	0,	0},
#line 1186 "x86insn_nasm.gperf"
    {"vfmadd213pd",	vfma_pd_insn,	2,	NONE,	0xA8,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 847 "x86insn_nasm.gperf"
    {"psrld",	pshift_insn,	4,	NONE,	0xD2,	0x72,	0x02,	0,	CPU_MMX,	0,	0},
#line 687 "x86insn_nasm.gperf"
    {"pcomtrueuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1274 "x86insn_nasm.gperf"
    {"vmovhps",	movhlp_insn,	3,	NONE,	0x00,	0x16,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 585 "x86insn_nasm.gperf"
    {"paddw",	mmxsse2_insn,	2,	NONE,	0xFD,	0,	0,	0,	CPU_MMX,	0,	0},
#line 569 "x86insn_nasm.gperf"
    {"outsw",	onebyte_insn,	1,	NONE,	0x6F,	0x10,	0,	0,	0,	0,	0},
#line 1270 "x86insn_nasm.gperf"
    {"vmovdqa",	movau_insn,	6,	NONE,	0x66,	0x6F,	0x10,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 113 "x86insn_nasm.gperf"
    {"cmpnltpd",	ssecmp_128_insn,	3,	NONE,	0x05,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 884 "x86insn_nasm.gperf"
    {"rdpmc",	twobyte_insn,	1,	NONE,	0x0F,	0x33,	0,	0,	CPU_686,	0,	0},
#line 427 "x86insn_nasm.gperf"
    {"invlpg",	twobytemem_insn,	1,	NONE,	0x07,	0x0F,	0x01,	0,	CPU_486,	CPU_Priv,	0},
#line 1353 "x86insn_nasm.gperf"
    {"vphaddd",	ssse3_insn,	3,	NONE,	0x02,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 908 "x86insn_nasm.gperf"
    {"sahf",	onebyte_insn,	1,	NONE,	0x9E,	0,	0,	0,	0,	0,	0},
#line 598 "x86insn_nasm.gperf"
    {"pclmullqhqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x10,	0,	0,	0,	CPU_AVX,	0,	0},
#line 1307 "x86insn_nasm.gperf"
    {"vpabsb",	avx_ssse3_2op_insn,	1,	NONE,	0x1C,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 343 "x86insn_nasm.gperf"
    {"fmaddpd",	sse5arith_insn,	4,	NONE,	0x01,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 379 "x86insn_nasm.gperf"
    {"fscale",	twobyte_insn,	1,	NONE,	0xD9,	0xFD,	0,	0,	CPU_FPU,	0,	0},
#line 1231 "x86insn_nasm.gperf"
    {"vfnmsub132ps",	vfma_ps_insn,	2,	NONE,	0x9E,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1077 "x86insn_nasm.gperf"
    {"vcmpltss",	ssecmp_32_insn,	4,	NONE,	0x01,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1331 "x86insn_nasm.gperf"
    {"vpclmullqhqdq",	pclmulqdq_fixed_insn,	2,	NONE,	0x10,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 713 "x86insn_nasm.gperf"
    {"pfrcp",	now3d_insn,	1,	NONE,	0x96,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 715 "x86insn_nasm.gperf"
    {"pfrcpit2",	now3d_insn,	1,	NONE,	0xB6,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 1263 "x86insn_nasm.gperf"
    {"vmlaunch",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC2,	0,	CPU_P4,	0,	0},
#line 998 "x86insn_nasm.gperf"
    {"unpcklps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x14,	0,	0,	CPU_SSE,	0,	0},
#line 1366 "x86insn_nasm.gperf"
    {"vpmaxsb",	ssse3_insn,	3,	NONE,	0x3C,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 324 "x86insn_nasm.gperf"
    {"fild",	fildstp_insn,	4,	NONE,	0x00,	0x02,	0x05,	0,	CPU_FPU,	0,	0},
#line 1070 "x86insn_nasm.gperf"
    {"vcmplt_oqpd",	ssecmp_128_insn,	3,	NONE,	0x11,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 243 "x86insn_nasm.gperf"
    {"comunordps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 37 "x86insn_nasm.gperf"
    {"andps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x54,	0,	0,	CPU_SSE,	0,	0},
#line 88 "x86insn_nasm.gperf"
    {"cmovpe",	cmovcc_insn,	3,	NONE,	0x0A,	0,	0,	0,	CPU_686,	0,	0},
#line 700 "x86insn_nasm.gperf"
    {"pextrw",	pextrw_insn,	7,	NONE,	0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 865 "x86insn_nasm.gperf"
    {"punpckhwd",	mmxsse2_insn,	2,	NONE,	0x69,	0,	0,	0,	CPU_MMX,	0,	0},
#line 737 "x86insn_nasm.gperf"
    {"phsubd",	ssse3_insn,	3,	NONE,	0x06,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1432 "x86insn_nasm.gperf"
    {"vpunpcklqdq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 576 "x86insn_nasm.gperf"
    {"packuswb",	mmxsse2_insn,	2,	NONE,	0x67,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1040 "x86insn_nasm.gperf"
    {"vcmpfalse_ossd",	ssecmp_64_insn,	4,	NONE,	0x1B,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 530 "x86insn_nasm.gperf"
    {"movnti",	movnti_insn,	2,	NONE,	0,	0,	0,	0,	CPU_P4,	0,	0},
#line 954 "x86insn_nasm.gperf"
    {"shufps",	xmm_xmm128_imm_insn,	1,	NONE,	0x00,	0xC6,	0,	0,	CPU_SSE,	0,	0},
#line 1217 "x86insn_nasm.gperf"
    {"vfmsubadd231ps",	vfma_ps_insn,	2,	NONE,	0xB7,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 492 "x86insn_nasm.gperf"
    {"loopnz",	loop_insn,	8,	NONE,	0x00,	0,	0,	0,	0,	0,	0},
#line 1293 "x86insn_nasm.gperf"
    {"vmptrst",	vmxtwobytemem_insn,	1,	NONE,	0x07,	0,	0,	0,	CPU_P4,	0,	0},
#line 408 "x86insn_nasm.gperf"
    {"haddps",	xmm_xmm128_insn,	2,	NONE,	0xF2,	0x7C,	0,	0,	CPU_SSE3,	0,	0},
#line 1085 "x86insn_nasm.gperf"
    {"vcmpneq_osss",	ssecmp_32_insn,	4,	NONE,	0x1C,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 22 "x86insn_nasm.gperf"
    {"addps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x58,	0,	0,	CPU_SSE,	0,	0},
#line 396 "x86insn_nasm.gperf"
    {"fucomip",	fcom2_insn,	2,	NONE,	0xDF,	0xE8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1190 "x86insn_nasm.gperf"
    {"vfmadd231pd",	vfma_pd_insn,	2,	NONE,	0xB8,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 455 "x86insn_nasm.gperf"
    {"jnle",	jcc_insn,	9,	NONE,	0x0F,	0,	0,	0,	0,	0,	0},
#line 1143 "x86insn_nasm.gperf"
    {"vcmptrueps",	ssecmp_128_insn,	3,	NONE,	0x0F,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1202 "x86insn_nasm.gperf"
    {"vfmsub132sd",	vfma_sd_insn,	2,	NONE,	0x9B,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 204 "x86insn_nasm.gperf"
    {"comugesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 538 "x86insn_nasm.gperf"
    {"movsb",	onebyte_insn,	1,	NONE,	0xA4,	0x00,	0,	0,	0,	0,	0},
#line 405 "x86insn_nasm.gperf"
    {"fyl2x",	twobyte_insn,	1,	NONE,	0xD9,	0xF1,	0,	0,	CPU_FPU,	0,	0},
#line 571 "x86insn_nasm.gperf"
    {"pabsd",	ssse3_insn,	3,	NONE,	0x1E,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1175 "x86insn_nasm.gperf"
    {"vdivss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5E,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1067 "x86insn_nasm.gperf"
    {"vcmpleps",	ssecmp_128_insn,	3,	NONE,	0x02,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 210 "x86insn_nasm.gperf"
    {"comulepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 184 "x86insn_nasm.gperf"
    {"comnltsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1294 "x86insn_nasm.gperf"
    {"vmread",	vmxmemrd_insn,	2,	NONE,	0,	0,	0,	0,	CPU_P4,	0,	0},
#line 727 "x86insn_nasm.gperf"
    {"phaddubq",	sse5two_insn,	1,	NONE,	0x53,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 105 "x86insn_nasm.gperf"
    {"cmpneqpd",	ssecmp_128_insn,	3,	NONE,	0x04,	0x66,	0,	0,	CPU_SSE,	0,	0},
#line 1354 "x86insn_nasm.gperf"
    {"vphaddsw",	ssse3_insn,	3,	NONE,	0x03,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1089 "x86insn_nasm.gperf"
    {"vcmpneq_usss",	ssecmp_32_insn,	4,	NONE,	0x14,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1256 "x86insn_nasm.gperf"
    {"vmaxss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5F,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 548 "x86insn_nasm.gperf"
    {"movups",	movau_insn,	6,	NONE,	0x00,	0x10,	0x01,	0,	CPU_SSE,	0,	0},
#line 582 "x86insn_nasm.gperf"
    {"paddsw",	mmxsse2_insn,	2,	NONE,	0xED,	0,	0,	0,	CPU_MMX,	0,	0},
#line 680 "x86insn_nasm.gperf"
    {"pcomq",	sse5com_insn,	1,	NONE,	0x4F,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 751 "x86insn_nasm.gperf"
    {"pmacsdql",	sse5pmacs_insn,	1,	NONE,	0x97,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1214 "x86insn_nasm.gperf"
    {"vfmsubadd213pd",	vfma_pd_insn,	2,	NONE,	0xA7,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1303 "x86insn_nasm.gperf"
    {"vmxoff",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC4,	0,	CPU_P4,	0,	0},
#line 894 "x86insn_nasm.gperf"
    {"retf",	retnf_insn,	6,	NONE,	0xCA,	0x40,	0,	0,	0,	0,	0},
#line 451 "x86insn_nasm.gperf"
    {"jne",	jcc_insn,	9,	NONE,	0x05,	0,	0,	0,	0,	0,	0},
#line 850 "x86insn_nasm.gperf"
    {"psrlw",	pshift_insn,	4,	NONE,	0xD1,	0x71,	0x02,	0,	CPU_MMX,	0,	0},
#line 1245 "x86insn_nasm.gperf"
    {"vhsubps",	xmm_xmm128_256_insn,	3,	NONE,	0xF2,	0x7D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1435 "x86insn_nasm.gperf"
    {"vrcpps",	avx_xmm_xmm128_insn,	2,	NONE,	0x00,	0x53,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 637 "x86insn_nasm.gperf"
    {"pcomgeuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 941 "x86insn_nasm.gperf"
    {"seto",	setcc_insn,	1,	NONE,	0x00,	0,	0,	0,	CPU_386,	0,	0},
#line 1417 "x86insn_nasm.gperf"
    {"vpsubb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xF8,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1050 "x86insn_nasm.gperf"
    {"vcmpgepd",	ssecmp_128_insn,	3,	NONE,	0x0D,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 425 "x86insn_nasm.gperf"
    {"into",	onebyte_insn,	1,	NONE,	0xCE,	0,	0,	NOT_64,	0,	0,	0},
#line 1127 "x86insn_nasm.gperf"
    {"vcmpord_sps",	ssecmp_128_insn,	3,	NONE,	0x17,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1327 "x86insn_nasm.gperf"
    {"vpblendvb",	avx_sse4xmm0_128_insn,	1,	NONE,	0x4C,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 564 "x86insn_nasm.gperf"
    {"orpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x56,	0,	0,	CPU_SSE2,	0,	0},
#line 293 "x86insn_nasm.gperf"
    {"fbld",	fbldstp_insn,	1,	NONE,	0x04,	0,	0,	0,	CPU_FPU,	0,	0},
#line 161 "x86insn_nasm.gperf"
    {"comltss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 607 "x86insn_nasm.gperf"
    {"pcmpestrm",	sse4pcmpstr_insn,	1,	NONE,	0x60,	0,	0,	0,	CPU_SSE42,	0,	0},
#line 1046 "x86insn_nasm.gperf"
    {"vcmpge_oqpd",	ssecmp_128_insn,	3,	NONE,	0x1D,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 403 "x86insn_nasm.gperf"
    {"fxsave",	twobytemem_insn,	1,	NONE,	0x00,	0x0F,	0xAE,	0,	CPU_686,	CPU_FPU,	0},
#line 1211 "x86insn_nasm.gperf"
    {"vfmsub231ss",	vfma_ss_insn,	2,	NONE,	0xBB,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1025 "x86insn_nasm.gperf"
    {"vcmpeq_osss",	ssecmp_32_insn,	4,	NONE,	0x10,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 277 "x86insn_nasm.gperf"
    {"dec",	incdec_insn,	6,	NONE,	0x48,	0x01,	0,	0,	0,	0,	0},
#line 116 "x86insn_nasm.gperf"
    {"cmpnltss",	ssecmp_32_insn,	4,	NONE,	0x05,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 119 "x86insn_nasm.gperf"
    {"cmpordsd",	ssecmp_64_insn,	4,	NONE,	0x07,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 224 "x86insn_nasm.gperf"
    {"comunesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 763 "x86insn_nasm.gperf"
    {"pmagw",	cyrixmmx_insn,	1,	NONE,	0x52,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 146 "x86insn_nasm.gperf"
    {"comgesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 466 "x86insn_nasm.gperf"
    {"jz",	jcc_insn,	9,	NONE,	0x04,	0,	0,	0,	0,	0,	0},
#line 1033 "x86insn_nasm.gperf"
    {"vcmpeq_usss",	ssecmp_32_insn,	4,	NONE,	0x18,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 459 "x86insn_nasm.gperf"
    {"jnz",	jcc_insn,	9,	NONE,	0x05,	0,	0,	0,	0,	0,	0},
#line 1391 "x86insn_nasm.gperf"
    {"vpmuldq",	ssse3_insn,	3,	NONE,	0x28,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 428 "x86insn_nasm.gperf"
    {"invlpga",	invlpga_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SVM,	0,	0},
#line 75 "x86insn_nasm.gperf"
    {"cmovnbe",	cmovcc_insn,	3,	NONE,	0x07,	0,	0,	0,	CPU_686,	0,	0},
#line 809 "x86insn_nasm.gperf"
    {"popfd",	onebyte_insn,	1,	NONE,	0x9D,	0x20,	0,	NOT_64,	CPU_386,	0,	0},
#line 900 "x86insn_nasm.gperf"
    {"roundsd",	sse4m64imm_insn,	4,	NONE,	0x0B,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1244 "x86insn_nasm.gperf"
    {"vhsubpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x7D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 98 "x86insn_nasm.gperf"
    {"cmpleps",	ssecmp_128_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1034 "x86insn_nasm.gperf"
    {"vcmpeqpd",	ssecmp_128_insn,	3,	NONE,	0x00,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 970 "x86insn_nasm.gperf"
    {"stmxcsr",	ldstmxcsr_insn,	1,	NONE,	0x03,	0,	0,	0,	CPU_SSE,	0,	0},
#line 648 "x86insn_nasm.gperf"
    {"pcomleb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 629 "x86insn_nasm.gperf"
    {"pcomfalseuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 1130 "x86insn_nasm.gperf"
    {"vcmpordpd",	ssecmp_128_insn,	3,	NONE,	0x07,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 579 "x86insn_nasm.gperf"
    {"paddq",	mmxsse2_insn,	2,	NONE,	0xD4,	0,	0,	0,	CPU_MMX,	0,	0},
#line 791 "x86insn_nasm.gperf"
    {"pmulhrsw",	ssse3_insn,	3,	NONE,	0x0B,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 742 "x86insn_nasm.gperf"
    {"pi2fd",	now3d_insn,	1,	NONE,	0x0D,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 52 "x86insn_nasm.gperf"
    {"cbw",	onebyte_insn,	1,	NONE,	0x98,	0x10,	0,	0,	0,	0,	0},
#line 486 "x86insn_nasm.gperf"
    {"lodsd",	onebyte_insn,	1,	NONE,	0xAD,	0x20,	0,	0,	CPU_386,	0,	0},
#line 708 "x86insn_nasm.gperf"
    {"pfmax",	now3d_insn,	1,	NONE,	0xA4,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 1277 "x86insn_nasm.gperf"
    {"vmovlps",	movhlp_insn,	3,	NONE,	0x00,	0x12,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 240 "x86insn_nasm.gperf"
    {"comunltsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 349 "x86insn_nasm.gperf"
    {"fmsubsd",	sse5arith64_insn,	8,	NONE,	0x0B,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 212 "x86insn_nasm.gperf"
    {"comulesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 1012 "x86insn_nasm.gperf"
    {"vandnps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x55,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 782 "x86insn_nasm.gperf"
    {"pmovsxwq",	sse4m32_insn,	2,	NONE,	0x24,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 453 "x86insn_nasm.gperf"
    {"jnge",	jcc_insn,	9,	NONE,	0x0C,	0,	0,	0,	0,	0,	0},
#line 1423 "x86insn_nasm.gperf"
    {"vpsubusw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xD9,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1158 "x86insn_nasm.gperf"
    {"vcvtpd2dq",	avx_cvt_xmm128_insn,	2,	NONE,	0xF2,	0xE6,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 226 "x86insn_nasm.gperf"
    {"comungepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 1393 "x86insn_nasm.gperf"
    {"vpmulhuw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE4,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 72 "x86insn_nasm.gperf"
    {"cmovna",	cmovcc_insn,	3,	NONE,	0x06,	0,	0,	0,	CPU_686,	0,	0},
#line 639 "x86insn_nasm.gperf"
    {"pcomgew",	sse5comcc_insn,	1,	NONE,	0x4D,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 969 "x86insn_nasm.gperf"
    {"sti",	onebyte_insn,	1,	NONE,	0xFB,	0,	0,	0,	0,	0,	0},
#line 611 "x86insn_nasm.gperf"
    {"pcmpgtw",	mmxsse2_insn,	2,	NONE,	0x65,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1022 "x86insn_nasm.gperf"
    {"vcmpeq_ospd",	ssecmp_128_insn,	3,	NONE,	0x10,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 853 "x86insn_nasm.gperf"
    {"psubq",	mmxsse2_insn,	2,	NONE,	0xFB,	0,	0,	0,	CPU_MMX,	0,	0},
#line 307 "x86insn_nasm.gperf"
    {"fcomip",	fcom2_insn,	2,	NONE,	0xDF,	0xF0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1384 "x86insn_nasm.gperf"
    {"vpmovsxwq",	sse4m32_insn,	2,	NONE,	0x24,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 668 "x86insn_nasm.gperf"
    {"pcomneqd",	sse5comcc_insn,	1,	NONE,	0x4E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 55 "x86insn_nasm.gperf"
    {"clc",	onebyte_insn,	1,	NONE,	0xF8,	0,	0,	0,	0,	0,	0},
#line 1453 "x86insn_nasm.gperf"
    {"vsubss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 719 "x86insn_nasm.gperf"
    {"pfsubr",	now3d_insn,	1,	NONE,	0xAA,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 286 "x86insn_nasm.gperf"
    {"enter",	enter_insn,	3,	NONE,	0,	0,	0,	0,	CPU_186,	0,	0},
#line 278 "x86insn_nasm.gperf"
    {"div",	div_insn,	8,	NONE,	0x06,	0,	0,	0,	0,	0,	0},
#line 205 "x86insn_nasm.gperf"
    {"comugess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 206 "x86insn_nasm.gperf"
    {"comugtpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 120 "x86insn_nasm.gperf"
    {"cmpordss",	ssecmp_32_insn,	4,	NONE,	0x07,	0xF3,	0,	0,	CPU_SSE,	0,	0},
#line 1135 "x86insn_nasm.gperf"
    {"vcmpps",	xmm_xmm128_imm_256_insn,	3,	NONE,	0x00,	0xC2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 614 "x86insn_nasm.gperf"
    {"pcomb",	sse5com_insn,	1,	NONE,	0x4C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1422 "x86insn_nasm.gperf"
    {"vpsubusb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xD8,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1437 "x86insn_nasm.gperf"
    {"vroundpd",	avx_sse4imm_insn,	3,	NONE,	0x09,	0,	0,	ONLY_AVX,	CPU_SSE41,	0,	0},
#line 811 "x86insn_nasm.gperf"
    {"popfw",	onebyte_insn,	1,	NONE,	0x9D,	0x10,	0x40,	0,	0,	0,	0},
#line 1336 "x86insn_nasm.gperf"
    {"vpcmpeqq",	ssse3_insn,	3,	NONE,	0x29,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1333 "x86insn_nasm.gperf"
    {"vpclmulqdq",	pclmulqdq_insn,	2,	NONE,	0x3A,	0x44,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 905 "x86insn_nasm.gperf"
    {"rsqrtps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x52,	0,	0,	CPU_SSE,	0,	0},
#line 870 "x86insn_nasm.gperf"
    {"push",	push_insn,	33,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 650 "x86insn_nasm.gperf"
    {"pcomleq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 313 "x86insn_nasm.gperf"
    {"fdivp",	farithp_insn,	3,	NONE,	0xF8,	0,	0,	0,	CPU_FPU,	0,	0},
#line 1261 "x86insn_nasm.gperf"
    {"vminsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5D,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1347 "x86insn_nasm.gperf"
    {"vpermilpd",	vpermil_insn,	4,	NONE,	0x05,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 886 "x86insn_nasm.gperf"
    {"rdtsc",	twobyte_insn,	1,	NONE,	0x0F,	0x31,	0,	0,	CPU_586,	0,	0},
#line 797 "x86insn_nasm.gperf"
    {"pmullw",	mmxsse2_insn,	2,	NONE,	0xD5,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1205 "x86insn_nasm.gperf"
    {"vfmsub213ps",	vfma_ps_insn,	2,	NONE,	0xAA,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 593 "x86insn_nasm.gperf"
    {"pavgw",	mmxsse2_insn,	2,	NONE,	0xE3,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 360 "x86insn_nasm.gperf"
    {"fnmsubps",	sse5arith_insn,	4,	NONE,	0x18,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 625 "x86insn_nasm.gperf"
    {"pcomfalsed",	sse5comcc_insn,	1,	NONE,	0x4E,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 971 "x86insn_nasm.gperf"
    {"stosb",	onebyte_insn,	1,	NONE,	0xAA,	0x00,	0,	0,	0,	0,	0},
#line 846 "x86insn_nasm.gperf"
    {"psraw",	pshift_insn,	4,	NONE,	0xE1,	0x71,	0x04,	0,	CPU_MMX,	0,	0},
#line 228 "x86insn_nasm.gperf"
    {"comungesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 1382 "x86insn_nasm.gperf"
    {"vpmovsxdq",	sse4m64_insn,	2,	NONE,	0x25,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 465 "x86insn_nasm.gperf"
    {"js",	jcc_insn,	9,	NONE,	0x08,	0,	0,	0,	0,	0,	0},
#line 213 "x86insn_nasm.gperf"
    {"comuless",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 296 "x86insn_nasm.gperf"
    {"fclex",	threebyte_insn,	1,	NONE,	0x9B,	0xDB,	0xE2,	0,	CPU_FPU,	0,	0},
#line 143 "x86insn_nasm.gperf"
    {"comfalsess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0B,	0,	0,	CPU_SSE5,	0,	0},
#line 871 "x86insn_nasm.gperf"
    {"pusha",	onebyte_insn,	1,	NONE,	0x60,	0x00,	0,	NOT_64,	CPU_186,	0,	0},
#line 586 "x86insn_nasm.gperf"
    {"palignr",	ssse3imm_insn,	2,	NONE,	0x0F,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1199 "x86insn_nasm.gperf"
    {"vfmaddsub231ps",	vfma_ps_insn,	2,	NONE,	0xB6,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 697 "x86insn_nasm.gperf"
    {"pextrb",	pextrb_insn,	3,	NONE,	0,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1401 "x86insn_nasm.gperf"
    {"vpshufd",	xmm_xmm128_imm_insn,	1,	NONE,	0x66,	0x70,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 47 "x86insn_nasm.gperf"
    {"bt",	bittest_insn,	6,	NONE,	0xA3,	0x04,	0,	0,	CPU_386,	0,	0},
#line 927 "x86insn_nasm.gperf"
    {"setna",	setcc_insn,	1,	NONE,	0x06,	0,	0,	0,	CPU_386,	0,	0},
#line 70 "x86insn_nasm.gperf"
    {"cmovl",	cmovcc_insn,	3,	NONE,	0x0C,	0,	0,	0,	CPU_686,	0,	0},
#line 39 "x86insn_nasm.gperf"
    {"blendpd",	sse4imm_insn,	2,	NONE,	0x0D,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 545 "x86insn_nasm.gperf"
    {"movsx",	movszx_insn,	5,	NONE,	0xBE,	0,	0,	0,	CPU_386,	0,	0},
#line 881 "x86insn_nasm.gperf"
    {"rcpss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x53,	0,	0,	CPU_SSE,	0,	0},
#line 783 "x86insn_nasm.gperf"
    {"pmovzxbd",	sse4m32_insn,	2,	NONE,	0x31,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1357 "x86insn_nasm.gperf"
    {"vphsubd",	ssse3_insn,	3,	NONE,	0x06,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 754 "x86insn_nasm.gperf"
    {"pmacssdql",	sse5pmacs_insn,	1,	NONE,	0x87,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 263 "x86insn_nasm.gperf"
    {"cvtsi2sd",	cvt_xmm_rmx_insn,	6,	NONE,	0xF2,	0x2A,	0,	0,	CPU_SSE2,	0,	0},
#line 1472 "x86insn_nasm.gperf"
    {"xchg",	xchg_insn,	16,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 728 "x86insn_nasm.gperf"
    {"phaddubw",	sse5two_insn,	1,	NONE,	0x51,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1109 "x86insn_nasm.gperf"
    {"vcmpngtss",	ssecmp_32_insn,	4,	NONE,	0x0A,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 632 "x86insn_nasm.gperf"
    {"pcomgeb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 1473 "x86insn_nasm.gperf"
    {"xcryptcbc",	padlock_insn,	1,	NONE,	0xD0,	0xF3,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 918 "x86insn_nasm.gperf"
    {"setae",	setcc_insn,	1,	NONE,	0x03,	0,	0,	0,	CPU_386,	0,	0},
#line 467 "x86insn_nasm.gperf"
    {"lahf",	onebyte_insn,	1,	NONE,	0x9F,	0,	0,	0,	0,	0,	0},
#line 893 "x86insn_nasm.gperf"
    {"ret",	retnf_insn,	6,	NONE,	0xC2,	0,	0,	0,	0,	0,	0},
#line 118 "x86insn_nasm.gperf"
    {"cmpordps",	ssecmp_128_insn,	3,	NONE,	0x07,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1098 "x86insn_nasm.gperf"
    {"vcmpngepd",	ssecmp_128_insn,	3,	NONE,	0x09,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 529 "x86insn_nasm.gperf"
    {"movntdqa",	movntdqa_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 978 "x86insn_nasm.gperf"
    {"subps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x5C,	0,	0,	CPU_SSE,	0,	0},
#line 252 "x86insn_nasm.gperf"
    {"cvtpd2pi",	cvt_mm_xmm_insn,	1,	NONE,	0x66,	0x2D,	0,	0,	CPU_SSE2,	0,	0},
#line 315 "x86insn_nasm.gperf"
    {"fdivrp",	farithp_insn,	3,	NONE,	0xF0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 433 "x86insn_nasm.gperf"
    {"ja",	jcc_insn,	9,	NONE,	0x07,	0,	0,	0,	0,	0,	0},
#line 1416 "x86insn_nasm.gperf"
    {"vpsrlw",	vpshift_insn,	4,	NONE,	0xD1,	0x71,	0x02,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 820 "x86insn_nasm.gperf"
    {"protb",	sse5prot_insn,	3,	NONE,	0x00,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 46 "x86insn_nasm.gperf"
    {"bswap",	bswap_insn,	2,	NONE,	0,	0,	0,	0,	CPU_486,	0,	0},
#line 164 "x86insn_nasm.gperf"
    {"comneqpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 19 "x86insn_nasm.gperf"
    {"adc",	arith_insn,	22,	NONE,	0x10,	0x02,	0,	0,	0,	0,	0},
#line 365 "x86insn_nasm.gperf"
    {"fnstcw",	fldnstcw_insn,	1,	NONE,	0x07,	0,	0,	0,	CPU_FPU,	0,	0},
#line 196 "x86insn_nasm.gperf"
    {"comtruesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0F,	0,	0,	CPU_SSE5,	0,	0},
#line 220 "x86insn_nasm.gperf"
    {"comuneqpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 508 "x86insn_nasm.gperf"
    {"minss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5D,	0,	0,	CPU_SSE,	0,	0},
#line 851 "x86insn_nasm.gperf"
    {"psubb",	mmxsse2_insn,	2,	NONE,	0xF8,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1015 "x86insn_nasm.gperf"
    {"vblendpd",	sse4imm_256_insn,	3,	NONE,	0x0D,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 419 "x86insn_nasm.gperf"
    {"insertps",	insertps_insn,	4,	NONE,	0,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 707 "x86insn_nasm.gperf"
    {"pfcmpgt",	now3d_insn,	1,	NONE,	0xA0,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 836 "x86insn_nasm.gperf"
    {"pshuflw",	xmm_xmm128_imm_insn,	1,	NONE,	0xF2,	0x70,	0,	0,	CPU_SSE2,	0,	0},
#line 124 "x86insn_nasm.gperf"
    {"cmpsd",	cmpsd_insn,	5,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 849 "x86insn_nasm.gperf"
    {"psrlq",	pshift_insn,	4,	NONE,	0xD3,	0x73,	0x02,	0,	CPU_MMX,	0,	0},
#line 943 "x86insn_nasm.gperf"
    {"setpe",	setcc_insn,	1,	NONE,	0x0A,	0,	0,	0,	CPU_386,	0,	0},
#line 516 "x86insn_nasm.gperf"
    {"movddup",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x12,	0,	0,	CPU_SSE3,	0,	0},
#line 1144 "x86insn_nasm.gperf"
    {"vcmptruesd",	ssecmp_64_insn,	4,	NONE,	0x0F,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1403 "x86insn_nasm.gperf"
    {"vpshuflw",	xmm_xmm128_imm_insn,	1,	NONE,	0xF2,	0x70,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 806 "x86insn_nasm.gperf"
    {"popaw",	onebyte_insn,	1,	NONE,	0x61,	0x10,	0,	NOT_64,	CPU_186,	0,	0},
#line 258 "x86insn_nasm.gperf"
    {"cvtps2pd",	xmm_xmm64_insn,	4,	NONE,	0x00,	0x5A,	0,	0,	CPU_SSE2,	0,	0},
#line 818 "x86insn_nasm.gperf"
    {"prefetcht2",	twobytemem_insn,	1,	NONE,	0x03,	0x0F,	0x18,	0,	CPU_P3,	0,	0},
#line 1278 "x86insn_nasm.gperf"
    {"vmovmskpd",	movmsk_insn,	4,	NONE,	0x66,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 928 "x86insn_nasm.gperf"
    {"setnae",	setcc_insn,	1,	NONE,	0x02,	0,	0,	0,	CPU_386,	0,	0},
#line 1451 "x86insn_nasm.gperf"
    {"vsubps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x5C,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 233 "x86insn_nasm.gperf"
    {"comungtss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1133 "x86insn_nasm.gperf"
    {"vcmpordss",	ssecmp_32_insn,	4,	NONE,	0x07,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1269 "x86insn_nasm.gperf"
    {"vmovddup",	vmovddup_insn,	3,	NONE,	0xF2,	0x12,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1147 "x86insn_nasm.gperf"
    {"vcmpunord_sps",	ssecmp_128_insn,	3,	NONE,	0x13,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 759 "x86insn_nasm.gperf"
    {"pmadcsswd",	sse5pmacs_insn,	1,	NONE,	0xA6,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1316 "x86insn_nasm.gperf"
    {"vpaddq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xD4,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 238 "x86insn_nasm.gperf"
    {"comunltpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 817 "x86insn_nasm.gperf"
    {"prefetcht1",	twobytemem_insn,	1,	NONE,	0x02,	0x0F,	0x18,	0,	CPU_P3,	0,	0},
#line 532 "x86insn_nasm.gperf"
    {"movntps",	movnt_insn,	2,	NONE,	0x00,	0x2B,	0,	0,	CPU_SSE,	0,	0},
#line 176 "x86insn_nasm.gperf"
    {"comngtsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 133 "x86insn_nasm.gperf"
    {"cmpxchg16b",	cmpxchg16b_insn,	1,	NONE,	0,	0,	0,	ONLY_64,	0,	0,	0},
#line 397 "x86insn_nasm.gperf"
    {"fucomp",	fcom2_insn,	2,	NONE,	0xDD,	0xE8,	0,	0,	CPU_286,	CPU_FPU,	0},
#line 830 "x86insn_nasm.gperf"
    {"pshld",	sse5psh_insn,	2,	NONE,	0x02,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1213 "x86insn_nasm.gperf"
    {"vfmsubadd132ps",	vfma_ps_insn,	2,	NONE,	0x97,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 910 "x86insn_nasm.gperf"
    {"salc",	onebyte_insn,	1,	NONE,	0xD6,	0,	0,	NOT_64,	CPU_Undoc,	0,	0},
#line 825 "x86insn_nasm.gperf"
    {"pshab",	sse5psh_insn,	2,	NONE,	0x04,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 288 "x86insn_nasm.gperf"
    {"extrq",	extrq_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SSE4a,	0,	0},
#line 321 "x86insn_nasm.gperf"
    {"ficomp",	fiarith_insn,	2,	NONE,	0x03,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 1436 "x86insn_nasm.gperf"
    {"vrcpss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x53,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 496 "x86insn_nasm.gperf"
    {"ltr",	prot286_insn,	1,	NONE,	0x03,	0x00,	0,	0,	CPU_286,	CPU_Priv,	CPU_Prot},
#line 1097 "x86insn_nasm.gperf"
    {"vcmpnge_uqss",	ssecmp_32_insn,	4,	NONE,	0x19,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 594 "x86insn_nasm.gperf"
    {"pblendvb",	sse4xmm0_insn,	2,	NONE,	0x10,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 325 "x86insn_nasm.gperf"
    {"fimul",	fiarith_insn,	2,	NONE,	0x01,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 660 "x86insn_nasm.gperf"
    {"pcomltud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 348 "x86insn_nasm.gperf"
    {"fmsubps",	sse5arith_insn,	4,	NONE,	0x08,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 94 "x86insn_nasm.gperf"
    {"cmpeqps",	ssecmp_128_insn,	3,	NONE,	0x00,	0,	0,	0,	CPU_SSE,	0,	0},
#line 1373 "x86insn_nasm.gperf"
    {"vpminsd",	ssse3_insn,	3,	NONE,	0x39,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 279 "x86insn_nasm.gperf"
    {"divpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5E,	0,	0,	CPU_SSE2,	0,	0},
#line 218 "x86insn_nasm.gperf"
    {"comunepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 272 "x86insn_nasm.gperf"
    {"cvttss2si",	cvt_rx_xmm32_insn,	4,	NONE,	0xF3,	0x2C,	0,	0,	CPU_386,	CPU_SSE,	0},
#line 641 "x86insn_nasm.gperf"
    {"pcomgtd",	sse5comcc_insn,	1,	NONE,	0x4E,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1317 "x86insn_nasm.gperf"
    {"vpaddsb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xEC,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 769 "x86insn_nasm.gperf"
    {"pmaxuw",	sse4_insn,	2,	NONE,	0x3E,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1129 "x86insn_nasm.gperf"
    {"vcmpord_sss",	ssecmp_32_insn,	4,	NONE,	0x17,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1479 "x86insn_nasm.gperf"
    {"xlatb",	onebyte_insn,	1,	NONE,	0xD7,	0x00,	0,	0,	0,	0,	0},
#line 216 "x86insn_nasm.gperf"
    {"comultsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 1168 "x86insn_nasm.gperf"
    {"vcvttpd2dq",	avx_cvt_xmm128_insn,	2,	NONE,	0x66,	0xE6,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 804 "x86insn_nasm.gperf"
    {"popa",	onebyte_insn,	1,	NONE,	0x61,	0x00,	0,	NOT_64,	CPU_186,	0,	0},
#line 197 "x86insn_nasm.gperf"
    {"comtruess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0F,	0,	0,	CPU_SSE5,	0,	0},
#line 740 "x86insn_nasm.gperf"
    {"phsubw",	ssse3_insn,	3,	NONE,	0x05,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 723 "x86insn_nasm.gperf"
    {"phaddd",	ssse3_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1418 "x86insn_nasm.gperf"
    {"vpsubd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xFA,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 798 "x86insn_nasm.gperf"
    {"pmuludq",	mmxsse2_insn,	2,	NONE,	0xF4,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 1340 "x86insn_nasm.gperf"
    {"vpcmpgtb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x64,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 269 "x86insn_nasm.gperf"
    {"cvttps2dq",	xmm_xmm128_insn,	2,	NONE,	0xF3,	0x5B,	0,	0,	CPU_SSE2,	0,	0},
#line 595 "x86insn_nasm.gperf"
    {"pblendw",	sse4imm_insn,	2,	NONE,	0x0E,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 505 "x86insn_nasm.gperf"
    {"minpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x5D,	0,	0,	CPU_SSE2,	0,	0},
#line 1030 "x86insn_nasm.gperf"
    {"vcmpeq_uspd",	ssecmp_128_insn,	3,	NONE,	0x18,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 497 "x86insn_nasm.gperf"
    {"lzcnt",	cnt_insn,	3,	NONE,	0xBD,	0,	0,	0,	CPU_686,	CPU_AMD,	0},
#line 562 "x86insn_nasm.gperf"
    {"o64",	NULL,	X86_OPERSIZE>>8,	0x40,	0,	0,	0,	ONLY_64,	0,	0,	0},
#line 1086 "x86insn_nasm.gperf"
    {"vcmpneq_uspd",	ssecmp_128_insn,	3,	NONE,	0x14,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 362 "x86insn_nasm.gperf"
    {"fnmsubss",	sse5arith32_insn,	8,	NONE,	0x1A,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 574 "x86insn_nasm.gperf"
    {"packsswb",	mmxsse2_insn,	2,	NONE,	0x63,	0,	0,	0,	CPU_MMX,	0,	0},
#line 683 "x86insn_nasm.gperf"
    {"pcomtrueq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 304 "x86insn_nasm.gperf"
    {"fcmovu",	fcmovcc_insn,	1,	NONE,	0xDA,	0xD8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1090 "x86insn_nasm.gperf"
    {"vcmpneqpd",	ssecmp_128_insn,	3,	NONE,	0x04,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1024 "x86insn_nasm.gperf"
    {"vcmpeq_ossd",	ssecmp_64_insn,	4,	NONE,	0x10,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1094 "x86insn_nasm.gperf"
    {"vcmpnge_uqpd",	ssecmp_128_insn,	3,	NONE,	0x19,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 57 "x86insn_nasm.gperf"
    {"clflush",	clflush_insn,	1,	NONE,	0,	0,	0,	0,	CPU_P3,	0,	0},
#line 635 "x86insn_nasm.gperf"
    {"pcomgeub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 1413 "x86insn_nasm.gperf"
    {"vpsrld",	vpshift_insn,	4,	NONE,	0xD2,	0x72,	0x02,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 445 "x86insn_nasm.gperf"
    {"jmp",	jmp_insn,	27,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 1475 "x86insn_nasm.gperf"
    {"xcryptctr",	padlock_insn,	1,	NONE,	0xD8,	0xF3,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 887 "x86insn_nasm.gperf"
    {"rdtscp",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xF9,	0,	CPU_686,	CPU_AMD,	CPU_Priv},
#line 711 "x86insn_nasm.gperf"
    {"pfnacc",	now3d_insn,	1,	NONE,	0x8A,	0,	0,	0,	CPU_3DNow,	CPU_Athlon,	0},
#line 322 "x86insn_nasm.gperf"
    {"fidiv",	fiarith_insn,	2,	NONE,	0x06,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 1342 "x86insn_nasm.gperf"
    {"vpcmpgtq",	ssse3_insn,	3,	NONE,	0x37,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1157 "x86insn_nasm.gperf"
    {"vcvtdq2ps",	avx_xmm_xmm128_insn,	2,	NONE,	0x00,	0x5B,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1443 "x86insn_nasm.gperf"
    {"vshufpd",	xmm_xmm128_imm_256_insn,	3,	NONE,	0x66,	0xC6,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 291 "x86insn_nasm.gperf"
    {"fadd",	farith_insn,	7,	NONE,	0xC0,	0xC0,	0x00,	0,	CPU_FPU,	0,	0},
#line 837 "x86insn_nasm.gperf"
    {"pshufw",	pshufw_insn,	1,	NONE,	0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 1237 "x86insn_nasm.gperf"
    {"vfnmsub213ss",	vfma_ss_insn,	2,	NONE,	0xAF,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1003 "x86insn_nasm.gperf"
    {"vaddsubpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0xD0,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 173 "x86insn_nasm.gperf"
    {"comngess",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 1083 "x86insn_nasm.gperf"
    {"vcmpneq_osps",	ssecmp_128_insn,	3,	NONE,	0x1C,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 447 "x86insn_nasm.gperf"
    {"jnae",	jcc_insn,	9,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 1118 "x86insn_nasm.gperf"
    {"vcmpnlt_uqpd",	ssecmp_128_insn,	3,	NONE,	0x15,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 139 "x86insn_nasm.gperf"
    {"comeqss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 234 "x86insn_nasm.gperf"
    {"comunlepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 561 "x86insn_nasm.gperf"
    {"o32",	NULL,	X86_OPERSIZE>>8,	0x20,	0,	0,	0,	0,	0,	0,	0},
#line 180 "x86insn_nasm.gperf"
    {"comnlesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 672 "x86insn_nasm.gperf"
    {"pcomnequq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1308 "x86insn_nasm.gperf"
    {"vpabsd",	avx_ssse3_2op_insn,	1,	NONE,	0x1E,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1471 "x86insn_nasm.gperf"
    {"xbts",	xbts_insn,	2,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_Obs,	CPU_Undoc},
#line 337 "x86insn_nasm.gperf"
    {"fldl2e",	twobyte_insn,	1,	NONE,	0xD9,	0xEA,	0,	0,	CPU_FPU,	0,	0},
#line 378 "x86insn_nasm.gperf"
    {"fsave",	twobytemem_insn,	1,	NONE,	0x06,	0x9B,	0xDD,	0,	CPU_FPU,	0,	0},
#line 1374 "x86insn_nasm.gperf"
    {"vpminsw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xEA,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 663 "x86insn_nasm.gperf"
    {"pcomltw",	sse5comcc_insn,	1,	NONE,	0x4D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1169 "x86insn_nasm.gperf"
    {"vcvttps2dq",	avx_xmm_xmm128_insn,	2,	NONE,	0xF3,	0x5B,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1455 "x86insn_nasm.gperf"
    {"vtestps",	sse4_insn,	2,	NONE,	0x0E,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 53 "x86insn_nasm.gperf"
    {"cdq",	onebyte_insn,	1,	NONE,	0x99,	0x20,	0,	0,	CPU_386,	0,	0},
#line 693 "x86insn_nasm.gperf"
    {"pcomw",	sse5com_insn,	1,	NONE,	0x4D,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 270 "x86insn_nasm.gperf"
    {"cvttps2pi",	cvt_mm_xmm64_insn,	2,	NONE,	0x2C,	0,	0,	0,	CPU_SSE,	0,	0},
#line 152 "x86insn_nasm.gperf"
    {"comisd",	xmm_xmm64_insn,	4,	NONE,	0x66,	0x2F,	0,	0,	CPU_SSE2,	0,	0},
#line 667 "x86insn_nasm.gperf"
    {"pcomneqb",	sse5comcc_insn,	1,	NONE,	0x4C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 546 "x86insn_nasm.gperf"
    {"movsxd",	movsxd_insn,	1,	NONE,	0,	0,	0,	ONLY_64,	0,	0,	0},
#line 1315 "x86insn_nasm.gperf"
    {"vpaddd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xFE,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1189 "x86insn_nasm.gperf"
    {"vfmadd213ss",	vfma_ss_insn,	2,	NONE,	0xA9,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1105 "x86insn_nasm.gperf"
    {"vcmpngt_uqss",	ssecmp_32_insn,	4,	NONE,	0x1A,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 854 "x86insn_nasm.gperf"
    {"psubsb",	mmxsse2_insn,	2,	NONE,	0xE8,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1066 "x86insn_nasm.gperf"
    {"vcmplepd",	ssecmp_128_insn,	3,	NONE,	0x02,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 85 "x86insn_nasm.gperf"
    {"cmovnz",	cmovcc_insn,	3,	NONE,	0x05,	0,	0,	0,	CPU_686,	0,	0},
#line 1412 "x86insn_nasm.gperf"
    {"vpsraw",	vpshift_insn,	4,	NONE,	0xE1,	0x71,	0x04,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1252 "x86insn_nasm.gperf"
    {"vmaskmovps",	vmaskmov_insn,	4,	NONE,	0x2C,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 268 "x86insn_nasm.gperf"
    {"cvttpd2pi",	cvt_mm_xmm_insn,	1,	NONE,	0x66,	0x2C,	0,	0,	CPU_SSE2,	0,	0},
#line 244 "x86insn_nasm.gperf"
    {"comunordsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 509 "x86insn_nasm.gperf"
    {"monitor",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xC8,	0,	CPU_SSE3,	0,	0},
#line 316 "x86insn_nasm.gperf"
    {"femms",	twobyte_insn,	1,	NONE,	0x0F,	0x0E,	0,	0,	CPU_3DNow,	0,	0},
#line 1486 "x86insn_nasm.gperf"
    {"xsha1",	padlock_insn,	1,	NONE,	0xC8,	0xF3,	0xA6,	0,	CPU_PadLock,	0,	0},
#line 188 "x86insn_nasm.gperf"
    {"comordsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 1029 "x86insn_nasm.gperf"
    {"vcmpeq_uqss",	ssecmp_32_insn,	4,	NONE,	0x08,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 786 "x86insn_nasm.gperf"
    {"pmovzxdq",	sse4m64_insn,	2,	NONE,	0x35,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 201 "x86insn_nasm.gperf"
    {"comueqss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x08,	0,	0,	CPU_SSE5,	0,	0},
#line 443 "x86insn_nasm.gperf"
    {"jl",	jcc_insn,	9,	NONE,	0x0C,	0,	0,	0,	0,	0,	0},
#line 267 "x86insn_nasm.gperf"
    {"cvttpd2dq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE6,	0,	0,	CPU_SSE2,	0,	0},
#line 367 "x86insn_nasm.gperf"
    {"fnstsw",	fnstsw_insn,	2,	NONE,	0,	0,	0,	0,	CPU_FPU,	0,	0},
#line 171 "x86insn_nasm.gperf"
    {"comngeps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 935 "x86insn_nasm.gperf"
    {"setnl",	setcc_insn,	1,	NONE,	0x0D,	0,	0,	0,	CPU_386,	0,	0},
#line 570 "x86insn_nasm.gperf"
    {"pabsb",	ssse3_insn,	3,	NONE,	0x1C,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1300 "x86insn_nasm.gperf"
    {"vmulsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x59,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 123 "x86insn_nasm.gperf"
    {"cmpsb",	onebyte_insn,	1,	NONE,	0xA6,	0x00,	0,	0,	0,	0,	0},
#line 1075 "x86insn_nasm.gperf"
    {"vcmpltps",	ssecmp_128_insn,	3,	NONE,	0x01,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 209 "x86insn_nasm.gperf"
    {"comugtss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 351 "x86insn_nasm.gperf"
    {"fmul",	farith_insn,	7,	NONE,	0xC8,	0xC8,	0x01,	0,	CPU_FPU,	0,	0},
#line 328 "x86insn_nasm.gperf"
    {"fist",	fiarith_insn,	2,	NONE,	0x02,	0xDB,	0,	0,	CPU_FPU,	0,	0},
#line 255 "x86insn_nasm.gperf"
    {"cvtpi2pd",	cvt_xmm_mm_ss_insn,	1,	NONE,	0x66,	0x2A,	0,	0,	CPU_SSE2,	0,	0},
#line 488 "x86insn_nasm.gperf"
    {"lodsw",	onebyte_insn,	1,	NONE,	0xAD,	0x10,	0,	0,	0,	0,	0},
#line 364 "x86insn_nasm.gperf"
    {"fnsave",	onebytemem_insn,	1,	NONE,	0x06,	0xDD,	0,	0,	CPU_FPU,	0,	0},
#line 659 "x86insn_nasm.gperf"
    {"pcomltub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1045 "x86insn_nasm.gperf"
    {"vcmpfalsess",	ssecmp_32_insn,	4,	NONE,	0x0B,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1480 "x86insn_nasm.gperf"
    {"xor",	arith_insn,	22,	NONE,	0x30,	0x06,	0,	0,	0,	0,	0},
#line 744 "x86insn_nasm.gperf"
    {"pinsrb",	pinsrb_insn,	4,	NONE,	0,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 856 "x86insn_nasm.gperf"
    {"psubsw",	mmxsse2_insn,	2,	NONE,	0xE9,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1281 "x86insn_nasm.gperf"
    {"vmovntdqa",	movntdqa_insn,	1,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1209 "x86insn_nasm.gperf"
    {"vfmsub231ps",	vfma_ps_insn,	2,	NONE,	0xBA,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 553 "x86insn_nasm.gperf"
    {"mulps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x59,	0,	0,	CPU_SSE,	0,	0},
#line 106 "x86insn_nasm.gperf"
    {"cmpneqps",	ssecmp_128_insn,	3,	NONE,	0x04,	0,	0,	0,	CPU_SSE,	0,	0},
#line 873 "x86insn_nasm.gperf"
    {"pushaw",	onebyte_insn,	1,	NONE,	0x60,	0x10,	0,	NOT_64,	CPU_186,	0,	0},
#line 163 "x86insn_nasm.gperf"
    {"comneps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1052 "x86insn_nasm.gperf"
    {"vcmpgesd",	ssecmp_64_insn,	4,	NONE,	0x0D,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1458 "x86insn_nasm.gperf"
    {"vunpckhpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x15,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 665 "x86insn_nasm.gperf"
    {"pcomned",	sse5comcc_insn,	1,	NONE,	0x4E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1026 "x86insn_nasm.gperf"
    {"vcmpeq_uqpd",	ssecmp_128_insn,	3,	NONE,	0x08,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 231 "x86insn_nasm.gperf"
    {"comungtps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1302 "x86insn_nasm.gperf"
    {"vmwrite",	vmxmemwr_insn,	2,	NONE,	0,	0,	0,	0,	CPU_P4,	0,	0},
#line 919 "x86insn_nasm.gperf"
    {"setb",	setcc_insn,	1,	NONE,	0x02,	0,	0,	0,	CPU_386,	0,	0},
#line 774 "x86insn_nasm.gperf"
    {"pminud",	sse4_insn,	2,	NONE,	0x3B,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1442 "x86insn_nasm.gperf"
    {"vrsqrtss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x52,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 653 "x86insn_nasm.gperf"
    {"pcomleuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 1063 "x86insn_nasm.gperf"
    {"vcmple_oqps",	ssecmp_128_insn,	3,	NONE,	0x12,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 822 "x86insn_nasm.gperf"
    {"protq",	sse5prot_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 605 "x86insn_nasm.gperf"
    {"pcmpeqw",	mmxsse2_insn,	2,	NONE,	0x75,	0,	0,	0,	CPU_MMX,	0,	0},
#line 790 "x86insn_nasm.gperf"
    {"pmulhriw",	cyrixmmx_insn,	1,	NONE,	0x5D,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 1380 "x86insn_nasm.gperf"
    {"vpmovsxbq",	sse4m16_insn,	2,	NONE,	0x22,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 827 "x86insn_nasm.gperf"
    {"pshaq",	sse5psh_insn,	2,	NONE,	0x07,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 236 "x86insn_nasm.gperf"
    {"comunlesd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 1156 "x86insn_nasm.gperf"
    {"vcvtdq2pd",	avx_cvt_xmm64_insn,	3,	NONE,	0xF3,	0xE6,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 803 "x86insn_nasm.gperf"
    {"pop",	pop_insn,	21,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 800 "x86insn_nasm.gperf"
    {"pmvlzb",	cyrixmmx_insn,	1,	NONE,	0x5B,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 65 "x86insn_nasm.gperf"
    {"cmovbe",	cmovcc_insn,	3,	NONE,	0x06,	0,	0,	0,	CPU_686,	0,	0},
#line 1096 "x86insn_nasm.gperf"
    {"vcmpnge_uqsd",	ssecmp_64_insn,	4,	NONE,	0x19,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1201 "x86insn_nasm.gperf"
    {"vfmsub132ps",	vfma_ps_insn,	2,	NONE,	0x9A,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 710 "x86insn_nasm.gperf"
    {"pfmul",	now3d_insn,	1,	NONE,	0xB4,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 458 "x86insn_nasm.gperf"
    {"jns",	jcc_insn,	9,	NONE,	0x09,	0,	0,	0,	0,	0,	0},
#line 1195 "x86insn_nasm.gperf"
    {"vfmaddsub132ps",	vfma_ps_insn,	2,	NONE,	0x96,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 217 "x86insn_nasm.gperf"
    {"comultss",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 73 "x86insn_nasm.gperf"
    {"cmovnae",	cmovcc_insn,	3,	NONE,	0x02,	0,	0,	0,	CPU_686,	0,	0},
#line 430 "x86insn_nasm.gperf"
    {"iretd",	onebyte_insn,	1,	NONE,	0xCF,	0x20,	0,	0,	CPU_386,	0,	0},
#line 373 "x86insn_nasm.gperf"
    {"frczps",	sse5two_insn,	1,	NONE,	0x10,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 297 "x86insn_nasm.gperf"
    {"fcmovb",	fcmovcc_insn,	1,	NONE,	0xDA,	0xC0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 1364 "x86insn_nasm.gperf"
    {"vpmaddubsw",	ssse3_insn,	3,	NONE,	0x04,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 61 "x86insn_nasm.gperf"
    {"cmc",	onebyte_insn,	1,	NONE,	0xF5,	0,	0,	0,	0,	0,	0},
#line 565 "x86insn_nasm.gperf"
    {"orps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x56,	0,	0,	CPU_SSE,	0,	0},
#line 843 "x86insn_nasm.gperf"
    {"psllq",	pshift_insn,	4,	NONE,	0xF3,	0x73,	0x06,	0,	CPU_MMX,	0,	0},
#line 484 "x86insn_nasm.gperf"
    {"lock",	NULL,	X86_LOCKREP>>8,	0xF0,	0,	0,	0,	0,	0,	0,	0},
#line 1227 "x86insn_nasm.gperf"
    {"vfnmadd231ps",	vfma_ps_insn,	2,	NONE,	0xBC,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 583 "x86insn_nasm.gperf"
    {"paddusb",	mmxsse2_insn,	2,	NONE,	0xDC,	0,	0,	0,	CPU_MMX,	0,	0},
#line 983 "x86insn_nasm.gperf"
    {"svts",	cyrixsmm_insn,	1,	NONE,	0x7C,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 195 "x86insn_nasm.gperf"
    {"comtrueps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0F,	0,	0,	CPU_SSE5,	0,	0},
#line 878 "x86insn_nasm.gperf"
    {"pxor",	mmxsse2_insn,	2,	NONE,	0xEF,	0,	0,	0,	CPU_MMX,	0,	0},
#line 1005 "x86insn_nasm.gperf"
    {"vaesdec",	aes_insn,	2,	NONE,	0x38,	0xDE,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 875 "x86insn_nasm.gperf"
    {"pushfd",	onebyte_insn,	1,	NONE,	0x9C,	0x20,	0,	NOT_64,	CPU_386,	0,	0},
#line 896 "x86insn_nasm.gperf"
    {"rol",	shift_insn,	16,	NONE,	0x00,	0,	0,	0,	0,	0,	0},
#line 24 "x86insn_nasm.gperf"
    {"addss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x58,	0,	0,	CPU_SSE,	0,	0},
#line 852 "x86insn_nasm.gperf"
    {"psubd",	mmxsse2_insn,	2,	NONE,	0xFA,	0,	0,	0,	CPU_MMX,	0,	0},
#line 103 "x86insn_nasm.gperf"
    {"cmpltsd",	ssecmp_64_insn,	4,	NONE,	0x01,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 1313 "x86insn_nasm.gperf"
    {"vpackuswb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x67,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 643 "x86insn_nasm.gperf"
    {"pcomgtub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 1255 "x86insn_nasm.gperf"
    {"vmaxsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5F,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1339 "x86insn_nasm.gperf"
    {"vpcmpestrm",	sse4pcmpstr_insn,	1,	NONE,	0x60,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1058 "x86insn_nasm.gperf"
    {"vcmpgtpd",	ssecmp_128_insn,	3,	NONE,	0x0E,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 867 "x86insn_nasm.gperf"
    {"punpckldq",	mmxsse2_insn,	2,	NONE,	0x62,	0,	0,	0,	CPU_MMX,	0,	0},
#line 879 "x86insn_nasm.gperf"
    {"rcl",	shift_insn,	16,	NONE,	0x02,	0,	0,	0,	0,	0,	0},
#line 1326 "x86insn_nasm.gperf"
    {"vpavgw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 675 "x86insn_nasm.gperf"
    {"pcomneub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1163 "x86insn_nasm.gperf"
    {"vcvtsd2ss",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x5A,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1220 "x86insn_nasm.gperf"
    {"vfnmadd132sd",	vfma_sd_insn,	2,	NONE,	0x9D,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1185 "x86insn_nasm.gperf"
    {"vfmadd132ss",	vfma_ss_insn,	2,	NONE,	0x99,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1344 "x86insn_nasm.gperf"
    {"vpcmpistri",	sse4pcmpstr_insn,	1,	NONE,	0x63,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1123 "x86insn_nasm.gperf"
    {"vcmpnltps",	ssecmp_128_insn,	3,	NONE,	0x05,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 275 "x86insn_nasm.gperf"
    {"daa",	onebyte_insn,	1,	NONE,	0x27,	0,	0,	NOT_64,	0,	0,	0},
#line 1467 "x86insn_nasm.gperf"
    {"wbinvd",	twobyte_insn,	1,	NONE,	0x0F,	0x09,	0,	0,	CPU_486,	CPU_Priv,	0},
#line 1396 "x86insn_nasm.gperf"
    {"vpmullw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xD5,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 999 "x86insn_nasm.gperf"
    {"vaddpd",	xmm_xmm128_256_insn,	3,	NONE,	0x66,	0x58,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 449 "x86insn_nasm.gperf"
    {"jnbe",	jcc_insn,	9,	NONE,	0x07,	0,	0,	0,	0,	0,	0},
#line 175 "x86insn_nasm.gperf"
    {"comngtps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 495 "x86insn_nasm.gperf"
    {"lss",	lfgss_insn,	2,	NONE,	0xB2,	0,	0,	0,	CPU_386,	0,	0},
#line 294 "x86insn_nasm.gperf"
    {"fbstp",	fbldstp_insn,	1,	NONE,	0x06,	0,	0,	0,	CPU_FPU,	0,	0},
#line 766 "x86insn_nasm.gperf"
    {"pmaxsw",	mmxsse2_insn,	2,	NONE,	0xEE,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 247 "x86insn_nasm.gperf"
    {"cqo",	onebyte_insn,	1,	NONE,	0x99,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 300 "x86insn_nasm.gperf"
    {"fcmovnb",	fcmovcc_insn,	1,	NONE,	0xDB,	0xC0,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 490 "x86insn_nasm.gperf"
    {"loope",	loop_insn,	8,	NONE,	0x01,	0,	0,	0,	0,	0,	0},
#line 1379 "x86insn_nasm.gperf"
    {"vpmovsxbd",	sse4m32_insn,	2,	NONE,	0x21,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 36 "x86insn_nasm.gperf"
    {"andpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x54,	0,	0,	CPU_SSE2,	0,	0},
#line 549 "x86insn_nasm.gperf"
    {"movzx",	movszx_insn,	5,	NONE,	0xB6,	0,	0,	0,	CPU_386,	0,	0},
#line 1018 "x86insn_nasm.gperf"
    {"vblendvps",	avx_sse4xmm0_insn,	2,	NONE,	0x4A,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 722 "x86insn_nasm.gperf"
    {"phaddbw",	sse5two_insn,	1,	NONE,	0x41,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 314 "x86insn_nasm.gperf"
    {"fdivr",	farith_insn,	7,	NONE,	0xF0,	0xF8,	0x07,	0,	CPU_FPU,	0,	0},
#line 633 "x86insn_nasm.gperf"
    {"pcomged",	sse5comcc_insn,	1,	NONE,	0x4E,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 90 "x86insn_nasm.gperf"
    {"cmovs",	cmovcc_insn,	3,	NONE,	0x08,	0,	0,	0,	CPU_686,	0,	0},
#line 1368 "x86insn_nasm.gperf"
    {"vpmaxsw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xEE,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 473 "x86insn_nasm.gperf"
    {"leave",	onebyte_insn,	1,	NONE,	0xC9,	0x00,	0x40,	0,	CPU_186,	0,	0},
#line 1430 "x86insn_nasm.gperf"
    {"vpunpcklbw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x60,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 248 "x86insn_nasm.gperf"
    {"crc32",	crc32_insn,	5,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_SSE42,	0},
#line 677 "x86insn_nasm.gperf"
    {"pcomneuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 838 "x86insn_nasm.gperf"
    {"psignb",	ssse3_insn,	3,	NONE,	0x08,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1335 "x86insn_nasm.gperf"
    {"vpcmpeqd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x76,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1427 "x86insn_nasm.gperf"
    {"vpunpckhdq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x6A,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 481 "x86insn_nasm.gperf"
    {"lmsw",	prot286_insn,	1,	NONE,	0x06,	0x01,	0,	0,	CPU_286,	CPU_Priv,	0},
#line 1367 "x86insn_nasm.gperf"
    {"vpmaxsd",	ssse3_insn,	3,	NONE,	0x3D,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1137 "x86insn_nasm.gperf"
    {"vcmpss",	xmm_xmm32_imm_insn,	4,	NONE,	0xF3,	0xC2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 456 "x86insn_nasm.gperf"
    {"jno",	jcc_insn,	9,	NONE,	0x01,	0,	0,	0,	0,	0,	0},
#line 966 "x86insn_nasm.gperf"
    {"stc",	onebyte_insn,	1,	NONE,	0xF9,	0,	0,	0,	0,	0,	0},
#line 413 "x86insn_nasm.gperf"
    {"idiv",	div_insn,	8,	NONE,	0x07,	0,	0,	0,	0,	0,	0},
#line 162 "x86insn_nasm.gperf"
    {"comnepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 1394 "x86insn_nasm.gperf"
    {"vpmulhw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE5,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 775 "x86insn_nasm.gperf"
    {"pminuw",	sse4_insn,	2,	NONE,	0x3A,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1232 "x86insn_nasm.gperf"
    {"vfnmsub132sd",	vfma_sd_insn,	2,	NONE,	0x9F,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 720 "x86insn_nasm.gperf"
    {"phaddbd",	sse5two_insn,	1,	NONE,	0x42,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 232 "x86insn_nasm.gperf"
    {"comungtsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 606 "x86insn_nasm.gperf"
    {"pcmpestri",	sse4pcmpstr_insn,	1,	NONE,	0x61,	0,	0,	0,	CPU_SSE42,	0,	0},
#line 1125 "x86insn_nasm.gperf"
    {"vcmpnltss",	ssecmp_32_insn,	4,	NONE,	0x05,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 758 "x86insn_nasm.gperf"
    {"pmacsww",	sse5pmacs_insn,	1,	NONE,	0x95,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 584 "x86insn_nasm.gperf"
    {"paddusw",	mmxsse2_insn,	2,	NONE,	0xDD,	0,	0,	0,	CPU_MMX,	0,	0},
#line 82 "x86insn_nasm.gperf"
    {"cmovno",	cmovcc_insn,	3,	NONE,	0x01,	0,	0,	0,	CPU_686,	0,	0},
#line 336 "x86insn_nasm.gperf"
    {"fldenv",	onebytemem_insn,	1,	NONE,	0x04,	0xD9,	0,	0,	CPU_FPU,	0,	0},
#line 1463 "x86insn_nasm.gperf"
    {"vxorps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x57,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 764 "x86insn_nasm.gperf"
    {"pmaxsb",	sse4_insn,	2,	NONE,	0x3C,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 914 "x86insn_nasm.gperf"
    {"scasd",	onebyte_insn,	1,	NONE,	0xAF,	0x20,	0,	0,	CPU_386,	0,	0},
#line 1176 "x86insn_nasm.gperf"
    {"vdppd",	sse4imm_insn,	2,	NONE,	0x41,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1160 "x86insn_nasm.gperf"
    {"vcvtps2dq",	avx_xmm_xmm128_insn,	2,	NONE,	0x66,	0x5B,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1343 "x86insn_nasm.gperf"
    {"vpcmpgtw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x65,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 642 "x86insn_nasm.gperf"
    {"pcomgtq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x02,	0,	0,	CPU_SSE5,	0,	0},
#line 334 "x86insn_nasm.gperf"
    {"fld1",	twobyte_insn,	1,	NONE,	0xD9,	0xE8,	0,	0,	CPU_FPU,	0,	0},
#line 415 "x86insn_nasm.gperf"
    {"in",	in_insn,	12,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 1069 "x86insn_nasm.gperf"
    {"vcmpless",	ssecmp_32_insn,	4,	NONE,	0x02,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 876 "x86insn_nasm.gperf"
    {"pushfq",	onebyte_insn,	1,	NONE,	0x9C,	0x40,	0x40,	ONLY_64,	0,	0,	0},
#line 1142 "x86insn_nasm.gperf"
    {"vcmptruepd",	ssecmp_128_insn,	3,	NONE,	0x0F,	0x66,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 855 "x86insn_nasm.gperf"
    {"psubsiw",	cyrixmmx_insn,	1,	NONE,	0x55,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 1149 "x86insn_nasm.gperf"
    {"vcmpunord_sss",	ssecmp_32_insn,	4,	NONE,	0x13,	0xF3,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1470 "x86insn_nasm.gperf"
    {"xadd",	cmpxchgxadd_insn,	4,	NONE,	0xC0,	0,	0,	0,	CPU_486,	0,	0},
#line 95 "x86insn_nasm.gperf"
    {"cmpeqsd",	ssecmp_64_insn,	4,	NONE,	0x00,	0xF2,	0,	0,	CPU_SSE2,	0,	0},
#line 554 "x86insn_nasm.gperf"
    {"mulsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x59,	0,	0,	CPU_SSE2,	0,	0},
#line 1459 "x86insn_nasm.gperf"
    {"vunpckhps",	xmm_xmm128_256_insn,	3,	NONE,	0x00,	0x15,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1488 "x86insn_nasm.gperf"
    {"xstore",	padlock_insn,	1,	NONE,	0xC0,	0x00,	0xA7,	0,	CPU_PadLock,	0,	0},
#line 259 "x86insn_nasm.gperf"
    {"cvtps2ph",	cvtps2ph_insn,	2,	NONE,	0,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1100 "x86insn_nasm.gperf"
    {"vcmpngesd",	ssecmp_64_insn,	4,	NONE,	0x09,	0xF2,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 717 "x86insn_nasm.gperf"
    {"pfrsqrt",	now3d_insn,	1,	NONE,	0x97,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 431 "x86insn_nasm.gperf"
    {"iretq",	onebyte_insn,	1,	NONE,	0xCF,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 730 "x86insn_nasm.gperf"
    {"phadduwd",	sse5two_insn,	1,	NONE,	0x56,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 44 "x86insn_nasm.gperf"
    {"bsf",	bsfr_insn,	3,	NONE,	0xBC,	0,	0,	0,	CPU_386,	0,	0},
#line 1439 "x86insn_nasm.gperf"
    {"vroundsd",	sse4m64imm_insn,	4,	NONE,	0x0B,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 480 "x86insn_nasm.gperf"
    {"lldt",	prot286_insn,	1,	NONE,	0x02,	0x00,	0,	0,	CPU_286,	CPU_Priv,	CPU_Prot},
#line 1314 "x86insn_nasm.gperf"
    {"vpaddb",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xFC,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1355 "x86insn_nasm.gperf"
    {"vphaddw",	ssse3_insn,	3,	NONE,	0x01,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 446 "x86insn_nasm.gperf"
    {"jna",	jcc_insn,	9,	NONE,	0x06,	0,	0,	0,	0,	0,	0},
#line 341 "x86insn_nasm.gperf"
    {"fldpi",	twobyte_insn,	1,	NONE,	0xD9,	0xEB,	0,	0,	CPU_FPU,	0,	0},
#line 747 "x86insn_nasm.gperf"
    {"pinsrw",	pinsrw_insn,	9,	NONE,	0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 756 "x86insn_nasm.gperf"
    {"pmacssww",	sse5pmacs_insn,	1,	NONE,	0x85,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 317 "x86insn_nasm.gperf"
    {"ffree",	ffree_insn,	1,	NONE,	0xDD,	0,	0,	0,	CPU_FPU,	0,	0},
#line 671 "x86insn_nasm.gperf"
    {"pcomnequd",	sse5comcc_insn,	1,	NONE,	0x6E,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 916 "x86insn_nasm.gperf"
    {"scasw",	onebyte_insn,	1,	NONE,	0xAF,	0x10,	0,	0,	0,	0,	0},
#line 265 "x86insn_nasm.gperf"
    {"cvtss2sd",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5A,	0,	0,	CPU_SSE2,	0,	0},
#line 211 "x86insn_nasm.gperf"
    {"comuleps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0A,	0,	0,	CPU_SSE5,	0,	0},
#line 476 "x86insn_nasm.gperf"
    {"lfs",	lfgss_insn,	2,	NONE,	0xB4,	0,	0,	0,	CPU_386,	0,	0},
#line 1478 "x86insn_nasm.gperf"
    {"xgetbv",	threebyte_insn,	1,	NONE,	0x0F,	0x01,	0xD0,	0,	CPU_386,	CPU_XSAVE,	0},
#line 628 "x86insn_nasm.gperf"
    {"pcomfalseud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 1004 "x86insn_nasm.gperf"
    {"vaddsubps",	xmm_xmm128_256_insn,	3,	NONE,	0xF2,	0xD0,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 733 "x86insn_nasm.gperf"
    {"phaddwd",	sse5two_insn,	1,	NONE,	0x46,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1279 "x86insn_nasm.gperf"
    {"vmovmskps",	movmsk_insn,	4,	NONE,	0x00,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 778 "x86insn_nasm.gperf"
    {"pmovsxbq",	sse4m16_insn,	2,	NONE,	0x22,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 909 "x86insn_nasm.gperf"
    {"sal",	shift_insn,	16,	NONE,	0x04,	0,	0,	0,	0,	0,	0},
#line 42 "x86insn_nasm.gperf"
    {"blendvps",	sse4xmm0_insn,	2,	NONE,	0x14,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 1007 "x86insn_nasm.gperf"
    {"vaesenc",	aes_insn,	2,	NONE,	0x38,	0xDC,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 839 "x86insn_nasm.gperf"
    {"psignd",	ssse3_insn,	3,	NONE,	0x0A,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 537 "x86insn_nasm.gperf"
    {"movq2dq",	movq2dq_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 638 "x86insn_nasm.gperf"
    {"pcomgeuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 148 "x86insn_nasm.gperf"
    {"comgtpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x0E,	0,	0,	CPU_SSE5,	0,	0},
#line 989 "x86insn_nasm.gperf"
    {"test",	test_insn,	20,	NONE,	0,	0,	0,	0,	0,	0,	0},
#line 23 "x86insn_nasm.gperf"
    {"addsd",	xmm_xmm64_insn,	4,	NONE,	0xF2,	0x58,	0,	0,	CPU_SSE2,	0,	0},
#line 808 "x86insn_nasm.gperf"
    {"popf",	onebyte_insn,	1,	NONE,	0x9D,	0x00,	0x40,	0,	0,	0,	0},
#line 550 "x86insn_nasm.gperf"
    {"mpsadbw",	sse4imm_insn,	2,	NONE,	0x42,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 225 "x86insn_nasm.gperf"
    {"comuness",	sse5comcc32_insn,	2,	NONE,	0x2E,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 973 "x86insn_nasm.gperf"
    {"stosq",	onebyte_insn,	1,	NONE,	0xAB,	0x40,	0,	ONLY_64,	0,	0,	0},
#line 686 "x86insn_nasm.gperf"
    {"pcomtrueuq",	sse5comcc_insn,	1,	NONE,	0x6F,	0x07,	0,	0,	CPU_SSE5,	0,	0},
#line 634 "x86insn_nasm.gperf"
    {"pcomgeq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 383 "x86insn_nasm.gperf"
    {"fsqrt",	twobyte_insn,	1,	NONE,	0xD9,	0xFA,	0,	0,	CPU_FPU,	0,	0},
#line 404 "x86insn_nasm.gperf"
    {"fxtract",	twobyte_insn,	1,	NONE,	0xD9,	0xF4,	0,	0,	CPU_FPU,	0,	0},
#line 609 "x86insn_nasm.gperf"
    {"pcmpgtd",	mmxsse2_insn,	2,	NONE,	0x66,	0,	0,	0,	CPU_MMX,	0,	0},
#line 346 "x86insn_nasm.gperf"
    {"fmaddss",	sse5arith32_insn,	8,	NONE,	0x02,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1484 "x86insn_nasm.gperf"
    {"xsave",	twobytemem_insn,	1,	NONE,	0x04,	0x0F,	0xAE,	0,	CPU_386,	CPU_XSAVE,	0},
#line 1389 "x86insn_nasm.gperf"
    {"vpmovzxwd",	sse4m64_insn,	2,	NONE,	0x33,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 222 "x86insn_nasm.gperf"
    {"comuneqsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x0C,	0,	0,	CPU_SSE5,	0,	0},
#line 170 "x86insn_nasm.gperf"
    {"comngepd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x09,	0,	0,	CPU_SSE5,	0,	0},
#line 750 "x86insn_nasm.gperf"
    {"pmacsdqh",	sse5pmacs_insn,	1,	NONE,	0x9F,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 462 "x86insn_nasm.gperf"
    {"jpe",	jcc_insn,	9,	NONE,	0x0A,	0,	0,	0,	0,	0,	0},
#line 776 "x86insn_nasm.gperf"
    {"pmovmskb",	pmovmskb_insn,	4,	NONE,	0,	0,	0,	0,	CPU_MMX,	CPU_P3,	0},
#line 980 "x86insn_nasm.gperf"
    {"subss",	xmm_xmm32_insn,	4,	NONE,	0xF3,	0x5C,	0,	0,	CPU_SSE,	0,	0},
#line 145 "x86insn_nasm.gperf"
    {"comgeps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x0D,	0,	0,	CPU_SSE5,	0,	0},
#line 988 "x86insn_nasm.gperf"
    {"sysret",	twobyte_insn,	1,	NONE,	0x0F,	0x07,	0,	0,	CPU_686,	CPU_AMD,	CPU_Priv},
#line 122 "x86insn_nasm.gperf"
    {"cmpps",	xmm_xmm128_imm_insn,	1,	NONE,	0x00,	0xC2,	0,	0,	CPU_SSE,	0,	0},
#line 921 "x86insn_nasm.gperf"
    {"setc",	setcc_insn,	1,	NONE,	0x02,	0,	0,	0,	CPU_386,	0,	0},
#line 802 "x86insn_nasm.gperf"
    {"pmvzb",	cyrixmmx_insn,	1,	NONE,	0x58,	0,	0,	0,	CPU_Cyrix,	CPU_MMX,	0},
#line 933 "x86insn_nasm.gperf"
    {"setng",	setcc_insn,	1,	NONE,	0x0E,	0,	0,	0,	CPU_386,	0,	0},
#line 517 "x86insn_nasm.gperf"
    {"movdq2q",	movdq2q_insn,	1,	NONE,	0,	0,	0,	0,	CPU_SSE2,	0,	0},
#line 166 "x86insn_nasm.gperf"
    {"comneqsd",	sse5comcc64_insn,	2,	NONE,	0x2F,	0x04,	0,	0,	CPU_SSE5,	0,	0},
#line 815 "x86insn_nasm.gperf"
    {"prefetchnta",	twobytemem_insn,	1,	NONE,	0x00,	0x0F,	0x18,	0,	CPU_P3,	0,	0},
#line 56 "x86insn_nasm.gperf"
    {"cld",	onebyte_insn,	1,	NONE,	0xFC,	0,	0,	0,	0,	0,	0},
#line 920 "x86insn_nasm.gperf"
    {"setbe",	setcc_insn,	1,	NONE,	0x06,	0,	0,	0,	CPU_386,	0,	0},
#line 1247 "x86insn_nasm.gperf"
    {"vinsertps",	insertps_insn,	4,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1235 "x86insn_nasm.gperf"
    {"vfnmsub213ps",	vfma_ps_insn,	2,	NONE,	0xAE,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1360 "x86insn_nasm.gperf"
    {"vpinsrb",	pinsrb_insn,	4,	NONE,	0xC0,	0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 80 "x86insn_nasm.gperf"
    {"cmovnl",	cmovcc_insn,	3,	NONE,	0x0D,	0,	0,	0,	CPU_686,	0,	0},
#line 592 "x86insn_nasm.gperf"
    {"pavgusb",	now3d_insn,	1,	NONE,	0xBF,	0,	0,	0,	CPU_3DNow,	0,	0},
#line 1219 "x86insn_nasm.gperf"
    {"vfnmadd132ps",	vfma_ps_insn,	2,	NONE,	0x9C,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1115 "x86insn_nasm.gperf"
    {"vcmpnleps",	ssecmp_128_insn,	3,	NONE,	0x06,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1234 "x86insn_nasm.gperf"
    {"vfnmsub213pd",	vfma_pd_insn,	2,	NONE,	0xAE,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 657 "x86insn_nasm.gperf"
    {"pcomltd",	sse5comcc_insn,	1,	NONE,	0x4E,	0x00,	0,	0,	CPU_SSE5,	0,	0},
#line 1164 "x86insn_nasm.gperf"
    {"vcvtsi2sd",	cvt_xmm_rmx_insn,	6,	NONE,	0xF2,	0x2A,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 303 "x86insn_nasm.gperf"
    {"fcmovnu",	fcmovcc_insn,	1,	NONE,	0xDB,	0xD8,	0,	0,	CPU_686,	CPU_FPU,	0},
#line 630 "x86insn_nasm.gperf"
    {"pcomfalseuw",	sse5comcc_insn,	1,	NONE,	0x6D,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 698 "x86insn_nasm.gperf"
    {"pextrd",	pextrd_insn,	1,	NONE,	0,	0,	0,	0,	CPU_386,	CPU_SSE41,	0},
#line 126 "x86insn_nasm.gperf"
    {"cmpss",	xmm_xmm32_imm_insn,	4,	NONE,	0xF3,	0xC2,	0,	0,	CPU_SSE,	0,	0},
#line 393 "x86insn_nasm.gperf"
    {"ftst",	twobyte_insn,	1,	NONE,	0xD9,	0xE4,	0,	0,	CPU_FPU,	0,	0},
#line 1431 "x86insn_nasm.gperf"
    {"vpunpckldq",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x62,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 386 "x86insn_nasm.gperf"
    {"fstenv",	twobytemem_insn,	1,	NONE,	0x06,	0x9B,	0xD9,	0,	CPU_FPU,	0,	0},
#line 21 "x86insn_nasm.gperf"
    {"addpd",	xmm_xmm128_insn,	2,	NONE,	0x66,	0x58,	0,	0,	CPU_SSE2,	0,	0},
#line 179 "x86insn_nasm.gperf"
    {"comnleps",	sse5comcc_insn,	1,	NONE,	0x2C,	0x06,	0,	0,	CPU_SSE5,	0,	0},
#line 15 "x86insn_nasm.gperf"
    {"aaa",	onebyte_insn,	1,	NONE,	0x37,	0,	0,	NOT_64,	0,	0,	0},
#line 1182 "x86insn_nasm.gperf"
    {"vfmadd132pd",	vfma_pd_insn,	2,	NONE,	0x98,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1192 "x86insn_nasm.gperf"
    {"vfmadd231sd",	vfma_sd_insn,	2,	NONE,	0xB9,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 35 "x86insn_nasm.gperf"
    {"andnps",	xmm_xmm128_insn,	2,	NONE,	0x00,	0x55,	0,	0,	CPU_SSE,	0,	0},
#line 1421 "x86insn_nasm.gperf"
    {"vpsubsw",	xmm_xmm128_insn,	2,	NONE,	0x66,	0xE9,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 1392 "x86insn_nasm.gperf"
    {"vpmulhrsw",	ssse3_insn,	3,	NONE,	0x0B,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 182 "x86insn_nasm.gperf"
    {"comnltpd",	sse5comcc_insn,	1,	NONE,	0x2D,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 432 "x86insn_nasm.gperf"
    {"iretw",	onebyte_insn,	1,	NONE,	0xCF,	0x10,	0,	0,	0,	0,	0},
#line 59 "x86insn_nasm.gperf"
    {"cli",	onebyte_insn,	1,	NONE,	0xFA,	0,	0,	0,	0,	0,	0},
#line 666 "x86insn_nasm.gperf"
    {"pcomneq",	sse5comcc_insn,	1,	NONE,	0x4F,	0x05,	0,	0,	CPU_SSE5,	0,	0},
#line 1203 "x86insn_nasm.gperf"
    {"vfmsub132ss",	vfma_ss_insn,	2,	NONE,	0x9B,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 110 "x86insn_nasm.gperf"
    {"cmpnleps",	ssecmp_128_insn,	3,	NONE,	0x06,	0,	0,	0,	CPU_SSE,	0,	0},
#line 981 "x86insn_nasm.gperf"
    {"svdc",	svdc_insn,	1,	NONE,	0,	0,	0,	0,	CPU_486,	CPU_Cyrix,	CPU_SMM},
#line 917 "x86insn_nasm.gperf"
    {"seta",	setcc_insn,	1,	NONE,	0x07,	0,	0,	0,	CPU_386,	0,	0},
#line 1240 "x86insn_nasm.gperf"
    {"vfnmsub231sd",	vfma_sd_insn,	2,	NONE,	0xBF,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 1229 "x86insn_nasm.gperf"
    {"vfnmadd231ss",	vfma_ss_insn,	2,	NONE,	0xBD,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 757 "x86insn_nasm.gperf"
    {"pmacswd",	sse5pmacs_insn,	1,	NONE,	0x96,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 1207 "x86insn_nasm.gperf"
    {"vfmsub213ss",	vfma_ss_insn,	2,	NONE,	0xAB,	0,	0,	ONLY_AVX,	CPU_FMA,	0,	0},
#line 371 "x86insn_nasm.gperf"
    {"fptan",	twobyte_insn,	1,	NONE,	0xD9,	0xF2,	0,	0,	CPU_FPU,	0,	0},
#line 725 "x86insn_nasm.gperf"
    {"phaddsw",	ssse3_insn,	3,	NONE,	0x03,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 536 "x86insn_nasm.gperf"
    {"movq",	movq_insn,	9,	NONE,	0,	0,	0,	0,	CPU_MMX,	0,	0},
#line 332 "x86insn_nasm.gperf"
    {"fisubr",	fiarith_insn,	2,	NONE,	0x05,	0xDA,	0,	0,	CPU_FPU,	0,	0},
#line 779 "x86insn_nasm.gperf"
    {"pmovsxbw",	sse4m64_insn,	2,	NONE,	0x20,	0,	0,	0,	CPU_SSE41,	0,	0},
#line 652 "x86insn_nasm.gperf"
    {"pcomleud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 1151 "x86insn_nasm.gperf"
    {"vcmpunordps",	ssecmp_128_insn,	3,	NONE,	0x03,	0x00,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 636 "x86insn_nasm.gperf"
    {"pcomgeud",	sse5comcc_insn,	1,	NONE,	0x6E,	0x03,	0,	0,	CPU_SSE5,	0,	0},
#line 1425 "x86insn_nasm.gperf"
    {"vptest",	sse4_insn,	2,	NONE,	0x17,	0xC0,	0,	ONLY_AVX,	CPU_AVX,	0,	0},
#line 752 "x86insn_nasm.gperf"
    {"pmacssdd",	sse5pmacs_insn,	1,	NONE,	0x8E,	0,	0,	0,	CPU_SSE5,	0,	0},
#line 651 "x86insn_nasm.gperf"
    {"pcomleub",	sse5comcc_insn,	1,	NONE,	0x6C,	0x01,	0,	0,	CPU_SSE5,	0,	0},
#line 732 "x86insn_nasm.gperf"
    {"phaddw",	ssse3_insn,	3,	NONE,	0x01,	0,	0,	0,	CPU_SSSE3,	0,	0},
#line 1402 "x86insn_nasm.gperf"
    {"vpshufhw",	xmm_xmm128_imm_insn,	1,	NONE,	0xF3,	0x70,	0xC0,	ONLY_AVX,	CPU_AVX,	0,	0}
  };
  static const unsigned short tab[] = {
    0,11,323,0,0,323,1213,83,1203,281,281,1203,1312,323,764,0,
    0,1116,0,829,399,665,508,1140,0,0,1042,281,323,0,281,764,
    1481,1140,605,83,0,665,0,1140,630,1044,517,665,11,83,0,702,
    0,83,517,1213,0,1116,517,702,1042,1116,605,281,1074,605,333,665,
    0,1116,1020,47,0,567,11,83,1140,0,665,83,1312,1312,0,1212,
    764,1095,1140,11,764,83,1042,702,988,1312,1530,1044,0,83,764,0,
    764,323,0,83,1312,11,1140,323,0,864,764,0,323,894,0,1312,
    508,665,1042,0,764,629,1312,702,605,83,1338,1432,823,1116,1020,0,
    517,0,281,0,824,1432,1140,0,1000,702,1140,0,83,518,0,1095,
    605,323,0,1140,630,665,0,1140,824,764,506,305,1140,824,1116,323,
    336,1140,1140,83,1042,1338,517,1509,1529,1232,764,781,1295,1044,399,702,
    1338,1140,1042,630,1432,629,1109,764,665,1203,11,764,1338,0,665,764,
    1213,1042,1140,764,83,1109,824,399,348,197,721,988,0,1312,1203,508,
    0,1140,11,1432,721,665,323,323,0,1312,665,1312,197,1453,793,1338,
    567,0,323,10,0,1044,323,1044,1042,517,1140,1140,1059,281,764,427,
    746,517,215,1105,1212,0,6,403,1140,764,629,1338,1338,864,0,323,
    0,517,83,0,1140,807,0,605,764,721,605,605,883,688,1044,1044,
    764,1020,517,605,1312,0,72,764,517,323,764,1042,1000,427,764,1140,
    1135,665,762,1140,762,1212,665,64,1338,1232,1213,1213,1000,83,0,323,
    333,1312,230,395,764,506,764,1481,1232,83,83,11,0,11,891,764,
    1169,1213,323,0,1042,1477,83,764,348,1042,517,1399,338,824,656,812,
    1312,0,546,1140,1074,307,288,630,665,0,988,702,1140,1312,11,1432,
    0,665,793,1213,1278,702,83,1217,1312,0,1217,864,0,11,1481,1212,
    1529,0,733,972,567,473,508,829,630,1116,764,83,1059,924,824,124,
    764,665,733,1186,323,399,1020,281,733,665,242,1243,737,1446,1338,1135,
    764,1383,348,665,0,665,1218,64,621,323,399,1140,864,665,83,546,
    702,764,0,764,629,793,1429,1107,1109,1295,620,988,0,1312,215,1092,
    288,522,83,1513,1140,440,629,342,6,517,0,1140,884,0,146,829,
    823,1042,721,51,111,0,856,630,332,600,94,622,823,829,702,605,
    630,787,323,781,1312,1432,1042,1142,488,665,558,0,1455,874,0,83,
    1044,387,1186,323,518,239,512,1141,0,286,83,807,1213,764,988,1109,
    281,605,1156,208,195,0,1218,0,988,0,11,1116,1109,864,346,1387,
  };

  const struct insnprefix_parse_data *ret;
  unsigned long rsl, val = phash_lookup(key, len, 0xbe1e08bbUL);
  rsl = ((val>>23)^tab[val&0x1ff]);
  if (rsl >= 1478) return NULL;
  ret = &pd[rsl];
  if (strcmp(key, ret->name) != 0) return NULL;
  return ret;
}

