/*
    LZ4 HC - High Compression Mode of LZ4
    Copyright (C) 2011, Yann Collet.
    GPL v2 License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

	You can contact the author at :
	- LZ4 homepage : http://fastcompression.blogspot.com/p/lz4.html
	- LZ4-HC source repository : http://code.google.com/p/lz4hc/
*/


//**************************************
// Includes
//**************************************
#include <string.h>   // for memset, memcpy
#include "lz4hc.h"
#include "mmc.h"



//**************************************
// Basic Types
//**************************************
#if defined(_MSC_VER) 
#define BYTE	unsigned __int8
#define U16		unsigned __int16
#define U32		unsigned __int32
#define S32		__int32
#else
#include <stdint.h>
#define BYTE	uint8_t
#define U16		uint16_t
#define U32		uint32_t
#define S32		int32_t
#endif


//**************************************
// Constants
//**************************************
#define ML_BITS 4
#define ML_MASK ((1U<<ML_BITS)-1)
#define RUN_BITS (8-ML_BITS)
#define RUN_MASK ((1U<<RUN_BITS)-1)
#define MINMATCH 4
#define COPYLENGTH 8
#define LASTLITERALS 5
#define MFLIMIT (COPYLENGTH+MINMATCH)
#define MINLENGTH (MFLIMIT+1)


//**************************************
// Local structures
//**************************************
#ifdef __GNUC__
#  define _PACKED __attribute__ ((packed))
#else
#  define _PACKED
#endif

typedef struct _U32_S
{
	U32 v;
} _PACKED U32_S;

typedef struct _U16_S
{
	U16 v;
} _PACKED U16_S;

#define A32(x) (((U32_S *)(x))->v)
#define A16(x) (((U16_S *)(x))->v)


//**************************************
// Macros
//**************************************
#define LZ4_COPYPACKET(s,d)		A32(d) = A32(s); d+=4; s+=4; A32(d) = A32(s); d+=4; s+=4;
#define LZ4_WILDCOPY(s,d,e)		do { LZ4_COPYPACKET(s,d) } while (d<e);
#define LZ4_BLINDCOPY(s,d,l)	{ BYTE* e=d+l; LZ4_WILDCOPY(s,d,e); d=e; }



//****************************
// Compression CODE
//****************************

int LZ4_compressHCCtx(void* ctx,
				 const char* source,
				 char* dest,
				 int isize)
{	
	const BYTE* ip = (const BYTE*) source;
	const BYTE* anchor = ip;
	const BYTE* const iend = ip + isize;
	const BYTE* const mflimit = iend - MFLIMIT;
	const BYTE* const matchlimit = (iend - LASTLITERALS);
	const BYTE* ref;

	BYTE* op = (BYTE*) dest;
	BYTE* token;
	
	int	len, length;
	U32	ml;


	// Main Loop
	while (ip < mflimit-1)
	{
		ml = MMC_InsertAndFindBestMatch (ctx, (char*)ip, matchlimit-ip, (char**)(&ref));
		if (!ml) { ip++; continue; }

		// Lazy match
		do
		{
			U32 tmpml;
			char* tmpref;
			tmpml = MMC_InsertAndFindBestMatch (ctx, (char*)ip+1, matchlimit-ip-1, &tmpref);
			if (tmpml > ml)
			{
				ml = tmpml;
				ref = (BYTE*)tmpref;
				ip++;
			}
			else break;
		} while (1);

		// Encode Literal length
		length = ip - anchor;
		token = op++;
		if (length>=(int)RUN_MASK) { *token=(RUN_MASK<<ML_BITS); len = length-RUN_MASK; for(; len > 254 ; len-=255) *op++ = 255; *op++ = (BYTE)len; } 
		else *token = (length<<ML_BITS);

		// Copy Literals
		LZ4_BLINDCOPY(anchor, op, length);

		// Encode Offset
		A16(op) = (ip-ref); op+=2;

		// Encode MatchLength
		len = (int)(ml-MINMATCH);
		if (len>=(int)ML_MASK) { *token+=ML_MASK; len-=ML_MASK; for(; len > 509 ; len-=510) { *op++ = 255; *op++ = 255; } if (len > 254) { len-=255; *op++ = 255; } *op++ = (BYTE)len; } 
		else *token += len;	

		// Prepare next loop
		MMC_InsertMany (ctx, (char*)ip+2, ml-2);
		ip += ml;
		anchor = ip; 
	}

	// Encode Last Literals
	{
		int lastRun = iend - anchor;
		if (lastRun>=(int)RUN_MASK) { *op++=(RUN_MASK<<ML_BITS); lastRun-=RUN_MASK; for(; lastRun > 254 ; lastRun-=255) *op++ = 255; *op++ = (BYTE) lastRun; } 
		else *op++ = (lastRun<<ML_BITS);
		memcpy(op, anchor, iend - anchor);
		op += iend-anchor;
	} 

	// End
	return (int) (((char*)op)-dest);
}


int LZ4_compressHC(const char* source,
				 char* dest,
				 int isize)
{
	void* ctx = MMC_Create ((char*)source);
	int result = LZ4_compressHCCtx(ctx, source, dest, isize);
	MMC_Free (&ctx);

	return result;
}




