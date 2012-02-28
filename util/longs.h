#pragma once

#include "fput.h"

#include <cstdio>

#include <util/system/defaults.h> // _BIDSCLASS _EXPCLASS

int in_long(i64 &longVal, FILE *file);
int in_long(i64 &longVal, const char *ptrBuf);

int out_long(const i64 &longVal, FILE *file);
int out_long(const i64 &longVal, char *ptrBuf);

int len_long(const i64 &longVal);

int in_long(i32 &longVal, FILE *file);
int in_long(i32 &longVal, const char *ptrBuf);

int out_long(const i32 &longVal, FILE *file);
int out_long(const i32 &longVal, char *ptrBuf);

int len_long(const i32 &longVal);

#if 1 /* new stuff */

#define PUT_8(x, buf)  *(buf)++ = (x)
#define GET_8(x, buf)  (x) = *(buf)++

#if   !defined (_must_align2_)
#   define PUT_16(x, buf) *(ui16*)(buf) = (x), (buf) += 2
#   define GET_16(x, buf) (x) = *(const ui16*)(buf), (buf) += 2
#elif defined (_big_endian_)
#   define PUT_16(x, buf) *(buf)++ = HI_8(x), *(buf)++ = LO_8(x)
#   define GET_16(x, buf) HI_8(x) = *(buf)++, LO_8(x) = *(buf)++
#elif defined(_little_endian_)
#   define PUT_16(x, buf) *(buf)++ = LO_8(x), *(buf)++ = HI_8(x)
#   define GET_16(x, buf) LO_8(x) = *(buf)++, HI_8(x) = *(buf)++
#endif

#if !defined (_must_align4_)
#   define PUT_32(x, buf) *(ui32*)(buf) = (x), (buf) += 4
#   define GET_32(x, buf) (x) = *(const ui32*)(buf), (buf) += 4
#elif defined(_big_endian_)
#   define PUT_32(x, buf) PUT_16(HI_16(x), buf), PUT_16(LO_16(x), buf)
#   define GET_32(x, buf) GET_16(HI_16(x), buf), GET_16(LO_16(x), buf)
#elif defined(_little_endian_)
#   define PUT_32(x, buf) PUT_16(LO_16(x), buf), PUT_16(HI_16(x), buf)
#   define GET_32(x, buf) GET_16(LO_16(x), buf), GET_16(HI_16(x), buf)
#endif

#if !defined (_must_align8_)
#   define PUT_64(x, buf) *(ui64*)(buf) = (x), (buf) += 8
#   define GET_64(x, buf) (x) = *(const ui64*)(buf), (buf) += 8
#elif defined(_big_endian_)
#   define PUT_64(x, buf) PUT_32(HI_32(x), buf), PUT_32(LO_32(x), buf)
#   define GET_64(x, buf) GET_32(HI_32(x), buf), GET_32(LO_32(x), buf)
#elif defined(_little_endian_)
#   define PUT_64(x, buf) PUT_32(LO_32(x), buf), PUT_32(HI_32(x), buf)
#   define GET_64(x, buf) GET_32(LO_32(x), buf), GET_32(HI_32(x), buf)
#endif

struct FILE_traits
{
    static void get_8 (ui8& x, FILE *f)  {fget(f, x);}
    static void get_16(ui16a& x, FILE *f) {fget_a(f, x);}
    static void get_32(ui32a& x, FILE *f) {fget_a(f, x);}
    static void put_8 (const ui8& x, FILE *f)  {fput(f, x);}
    static void put_16(const ui16a& x, FILE *f) {fput_a(f, x);}
    static void put_32(const ui32a& x, FILE *f) {fput_a(f, x);}
#if defined(__FreeBSD__) || defined(__linux__)
    static int  is_good(FILE *f)          {return !feof_unlocked(f) && !ferror_unlocked(f);}
#else
    static int  is_good(FILE *f)          {return !feof(f) && !ferror(f);}
#endif
};

struct mem_traits
{
    static void get_8 (ui8& x, const char*& mem) {GET_8(x, mem);}
    static void get_16(ui16a& x, const char*& mem){GET_16(x, mem);}
    static void get_32(ui32a& x, const char*& mem){GET_32(x, mem);}
    static void put_8 (const ui8& x, char*& mem) {PUT_8(x, mem);}
    static void put_16(const ui16a& x, char*& mem){PUT_16(x, mem);}
    static void put_32(const ui32a& x, char*& mem){PUT_32(x, mem);}
    static int  is_good(char *&)              {return 1;}
};

/*
|____|____|____|____|____|____|____|____|____|____|____|____|____|____|8***|****
|____|____|____|____|____|____|____|____|____|____|____|____|i4**|****|****|****
|____|____|____|____|____|____|____|____|____|____|ii2*|****|****|****|****|****
|____|____|____|____|____|____|____|____|iii1|****|****|****|****|****|****|****
|____|____|____|____|____|____|iiii|8***|****|****|****|****|****|****|****|****
|____|____|____|____|iiii|i4**|****|****|****|****|****|****|****|****|****|****
|____|____|iiii|ii2*|****|****|****|****|****|****|****|****|****|****|****|****
|iiii|iii1|****|****|****|****|****|****|****|****|****|****|****|****|****|****
*/

#define MY_16(x) ((ui16)(x) < 0x80u ? 1 : 2)
#define MY_32(x) ((ui32a)(x) < 0x4000u ? MY_16(LO_16(x)) : ((ui32a)(x) < 0x200000ul ? 3 : 4))
#define MY_64(x) (HI_32(x) == 0 && LO_32(x) < 0x10000000ul ? MY_32(LO_32(x)) :\
        (HI_32(x) < 0x400u ? (HI_32(x)<8?5:6) :\
        (HI_32(x) < 0x20000ul ? 7 : (HI_32(x) < 0x1000000ul?8:9)) ))

#if !defined (MACRO_BEGIN)
#   define MACRO_BEGIN                 do {
#   define MACRO_END                   } while (0)
#endif

#define PACK_16(x,buf,how,ret)\
  MACRO_BEGIN\
      if ((ui16)(x) < 0x80u) {\
          how::put_8(LO_8(x), (buf));\
          (ret) = 1;\
      } else {\
          how::put_8((ui8)(0x80 | HI_8(x)), (buf));\
          how::put_8(LO_8(x), (buf));\
          (ret) = 2;\
      }\
  MACRO_END

#define PACK_32(x,buf,how,ret)\
  MACRO_BEGIN\
      if ((ui32a)(x) < 0x4000u) {\
          PACK_16(LO_16(x),(buf),how,(ret));\
      } else {\
          if ((ui32a)(x) < 0x200000ul) {\
              how::put_8((ui8)(0xC0 | LO_8(HI_16(x))), buf);\
              (ret) = 3;\
          } else {\
              how::put_8((ui8)(0xE0 | HI_8(HI_16(x))), buf);\
              how::put_8(LO_8(HI_16(x)), buf);\
              (ret) = 4;\
          }\
          how::put_16(LO_16(x), (buf));\
      }\
  MACRO_END

#define PACK_64(x,buf,how,ret)\
  MACRO_BEGIN\
      if (HI_32(x) == 0 && LO_32(x) < 0x10000000ul) {\
          PACK_32(LO_32(x),(buf),how,(ret));\
      } else {\
          if (HI_32(x) < 0x400u) {\
              if (HI_32(x) < 8) {\
                  how::put_8((ui8)(0xF0 | LO_8(LO_16(HI_32(x)))), buf);\
                  (ret) = 5;\
              } else {\
                  how::put_8((ui8)(0xF8 | HI_8(LO_16(HI_32(x)))), buf);\
                  how::put_8(LO_8(LO_16(HI_32(x))), buf);\
                  (ret) = 6;\
              }\
          } else {\
              if (HI_32(x) < 0x20000ul) {\
                  how::put_8((ui8)(0xFC | LO_8(HI_16(HI_32(x)))), buf);\
                  (ret) = 7;\
              } else {\
                  if (HI_32(x) < 0x1000000ul) {\
                      how::put_8((ui8)(0xFE | HI_8(HI_16(HI_32(x)))), buf);\
                      how::put_8(LO_8(HI_16(HI_32(x))), buf);\
                      (ret) = 8;\
                  } else {\
                      how::put_8((ui8)(0xFF), buf);\
                      how::put_16(HI_16(HI_32(x)), buf);\
                      (ret) = 9;\
                  }\
              }\
              how::put_16(LO_16(HI_32(x)), buf);\
          }\
          how::put_32(LO_32(x), buf);\
      }\
  MACRO_END

#define DO_UNPACK_16(x, buf, how, ret)\
  MACRO_BEGIN\
      if (LO_8(x) < 0x80) {\
          (ret) = 1;\
      } else {\
          HI_8(x) = (ui8)(0x7F & LO_8(x));\
          how::get_8(LO_8(x), buf);\
          (ret) = 2;\
      }\
  MACRO_END

#define UNPACK_16(x, buf, how, ret)\
  MACRO_BEGIN\
      (x) = 0U;\
      how::get_8(LO_8(x), (buf));\
      DO_UNPACK_16((x), (buf), how, (ret));\
  MACRO_END

#define DO_UNPACK_32(x, buf, how, ret)\
  MACRO_BEGIN\
      if (LO_8(LO_16(x)) < 0xC0) {\
          DO_UNPACK_16(LO_16(x), (buf), how, (ret));\
      } else {\
          if (LO_8(LO_16(x)) < 0xE0) {\
              LO_8(HI_16(x)) = (ui8)(0x3F & LO_8(LO_16(x)));\
              (ret) = 3;\
          } else {\
              HI_8(HI_16(x)) = (ui8)(0x1F & LO_8(LO_16(x)));\
              how::get_8(LO_8(HI_16(x)), (buf));\
              (ret) = 4;\
          }\
          how::get_16(LO_16(x), (buf));\
      }\
  MACRO_END

#define UNPACK_32(x, buf, how, ret)\
  MACRO_BEGIN\
      (x) = 0UL;\
      how::get_8(LO_8(LO_16(x)), (buf));\
      DO_UNPACK_32((x), buf, how, (ret));\
  MACRO_END

#define DO_UNPACK_64(x, buf, how, ret)\
  MACRO_BEGIN\
      if (LO_8(LO_16(LO_32(x))) < 0xF0) {\
          DO_UNPACK_32(LO_32(x), (buf), how, (ret));\
      } else {\
          if (LO_8(LO_16(LO_32(x))) < 0xFC) {\
              if (LO_8(LO_16(LO_32(x))) < 0xF8) {\
                  LO_8(LO_16(HI_32(x))) = (ui8)(0x0F & LO_8(LO_16(LO_32(x))));\
                  (ret) = 5;\
              } else {\
                  HI_8(LO_16(HI_32(x))) = (ui8)(0x07 & LO_8(LO_16(LO_32(x))));\
                  how::get_8(LO_8(LO_16(HI_32(x))), (buf));\
                  (ret) = 6;\
              }\
          } else {\
              if (LO_8(LO_16(LO_32(x))) < 0xFE) {\
                  LO_8(HI_16(HI_32(x))) = (ui8)(0x03 & LO_8(LO_16(LO_32(x))));\
                  (ret) = 7;\
              } else {\
                  if (LO_8(LO_16(LO_32(x))) < 0xFF) {\
                      HI_8(HI_16(HI_32(x))) = (ui8)(0x01 & LO_8(LO_16(LO_32(x))));\
                      how::get_8(LO_8(HI_16(HI_32(x))), (buf));\
                      (ret) = 8;\
                  } else {\
                      how::get_16(HI_16(HI_32(x)), (buf));\
                      (ret) = 9;\
                  }\
              }\
              how::get_16(LO_16(HI_32(x)), (buf));\
          }\
          how::get_32(LO_32(x), (buf));\
      }\
  MACRO_END

#define UNPACK_64(x, buf, how, ret)\
  MACRO_BEGIN\
      (x) = 0UL;\
      how::get_8(LO_8(LO_16(LO_32(x))), buf);\
      DO_UNPACK_64(x, buf, how, ret);\
  MACRO_END

inline int in_long(i64 &longVal, FILE *file)
{
    int ret;
    UNPACK_64(longVal, file, FILE_traits, ret);
    return ret;
}
inline int in_long(i64 &longVal, const char *ptrBuf)
{
    int ret;
    UNPACK_64(longVal, ptrBuf, mem_traits, ret);
    return ret;
}

inline int out_long(const i64 &longVal, FILE *file)
{
    int ret;
    PACK_64(longVal, file, FILE_traits, ret);/*7*/
    return ret;
}
inline int out_long(const i64 &longVal, char *ptrBuf)
{
    int ret;
    PACK_64(longVal, ptrBuf, mem_traits, ret);/*7*/
    return ret;
}

inline int len_long(const i64 &longVal)
{
    return MY_64(longVal);
}

inline int in_long(i32 &longVal, FILE *file)
{
    int ret;
    UNPACK_32(longVal, file, FILE_traits, ret);
    return ret;
}
inline int in_long(i32 &longVal, const char *ptrBuf)
{
    int ret;
    UNPACK_32(longVal, ptrBuf, mem_traits, ret);
    return ret;
}

inline int out_long(const i32 &longVal, FILE *file)
{
    int ret;
    PACK_32(longVal, file, FILE_traits, ret);
    return ret;
}
inline int out_long(const i32 &longVal, char *ptrBuf)
{
    int ret;
    PACK_32(longVal, ptrBuf, mem_traits, ret);
    return ret;
}

inline int len_long(const i32 &longVal)
{
    return MY_32(longVal);
}

#endif /* new stuff */

extern char *i386unp_bufba[256];
#define ASMi386_UNPACK_ADD_64(Cur, Sum)     \
    asm (                                   \
        "movzbl (%0),%%eax; incl %0; \n"    \
        "call *i386unp_bufba(,%%eax,4) \n"  \
        "addl %%eax,%1; \n"                 \
        "adcl %%edx,%2; \n"                 \
        : "+c" (Cur), "=m" (LO_32(Sum)), "=m" (HI_32(Sum)) \
        : /* "0" (Cur), "m" (LO_32(Sum)), "m" (HI_32(Sum)) */ \
        : "cc", "eax", "edx")

#define ASMi386_UNPACK_64(Cur, Sum)         \
    asm (                                   \
        "movzbl (%0),%%eax; incl %0; \n"    \
        "call *i386unp_bufba(,%%eax,4) \n"  \
        "movl %%eax,%1; \n"                 \
        "movl %%edx,%2; \n"                 \
        : "+c" (Cur), "+m" (LO_32(Sum)), "+m" (HI_32(Sum)) \
        : /* "0" (Cur), "1" (LO_32(Sum)), "2" (HI_32(Sum))*/ \
        : "cc", "eax", "edx")

extern char *i64unp_bufba[256];
#define ASMi64_UNPACK_ADD_64(Cur, Sum)      \
    asm (                                   \
        "movzbl (%0),%%eax; incq %0; \n"    \
        "call *i64unp_bufba(,%%rax,8) \n"   \
        "addq %%rax,%1; \n"                 \
        : "+c" (Cur), "+g" (Sum) \
        :  \
        : "cc", "rax", "rdx")

#define ASMi64_UNPACK_64(Cur, Val)          \
    asm (                                   \
        "movzbl (%0),%%eax; incq %0; \n"    \
        "call *i64unp_bufba(,%%rax,8) \n"   \
        : "+c" (Cur), "=a" (Val) \
        : \
        : "cc", "rdx")
