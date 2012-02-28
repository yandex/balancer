/*
 *	osdep.h
 *	Operating System and Hardware Abstraction Layer
 *	Release $Name: MATRIXSSL-3-2-1-OPEN $
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2011. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software
 *	into proprietary programs.  If you are unable to comply with the GPL, a
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *	See the GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#ifndef _h_PS_PLATFORM
#define _h_PS_PLATFORM

/******************************************************************************/
/*
    APIs that must be implemented on every platform
*/
extern int		osdepTraceOpen(void);
extern void		osdepTraceClose(void);
extern int		osdepTimeOpen(void);
extern void		osdepTimeClose(void);
extern int		osdepEntropyOpen(void);
extern void		osdepEntropyClose(void);
#ifdef HALT_ON_PS_ERROR
	extern void  osdepBreak(void);
#endif
#ifndef min
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif /* min */

/******************************************************************************/
/*
    If the Makefile specifies that PeerSec MatrixSSL does not currently have
    a layer for the given OS, or the port is to "bare metal" hardware,
    do basic defines here and include externally provided file "matrixos.h".
    In addition, if building for such a platform, a C file defining the above
    functions must be linked with the final executable.
*/
#ifdef PS_UNSUPPORTED_OS
    #define PSPUBLIC extern
    #include "matrixos.h"
#else
/******************************************************************************/
/*
    Supported Platforms below. The implementations of the apis are in
    platform specific directories, such as core/POSIX and core/ECOS

    POSIX define is used for Linux and Mac OS X
*/
#include <util/system/defaults.h>

typedef i32 int32;
typedef ui32 uint32;
typedef i16 int16;
typedef ui16 uint16;
typedef i64 int64;
typedef ui64 uint64;

/******************************************************************************/
/*
	Hardware Abstraction Layer
*/
#define halAlert()

/******************************************************************************/
/*
	OS-specific psTime_t types

	Make psTime_t an opaque time value.
*/

typedef uint64 psTime_t;

/******************************************************************************/
#define PSPUBLIC extern

/******************************************************************************/
/*
	Raw trace and error
*/
PSPUBLIC void _psTrace(char *msg);
PSPUBLIC void _psTraceInt(char *msg, int32 val);
PSPUBLIC void _psTraceStr(char *msg, char *val);
PSPUBLIC void _psTracePtr(char *message, void *value);
PSPUBLIC void psTraceBytes(char *tag, unsigned char *p, int l);

PSPUBLIC void _psError(char *msg);
PSPUBLIC void _psErrorInt(char *msg, int32 val);
PSPUBLIC void _psErrorStr(char *msg, char *val);

/******************************************************************************/
/*
	Core trace
*/
#ifndef USE_CORE_TRACE
#define psTraceCore(x)
#define psTraceStrCore(x, y)
#define psTraceIntCore(x, y)
#define psTracePtrCore(x, y)
#else
#define psTraceCore(x) _psTrace(x)
#define psTraceStrCore(x, y) _psTraceStr(x, y)
#define psTraceIntCore(x, y) _psTraceInt(x, y)
#define psTracePtrCore(x, y) _psTracePtr(x, y)
#endif /* USE_CORE_TRACE */

/******************************************************************************/
/*
	HALT_ON_PS_ERROR define at compile-time determines whether to halt on
	psAssert and psError calls
*/
#define psAssert(C)  if (C) ; else \
{halAlert();_psTraceStr("psAssert %s", __FILE__);_psTraceInt(":%d ", __LINE__);\
_psError(#C);}

#define psError(a) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psError(a);

#define psErrorStr(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorStr(a,b)

#define psErrorInt(a,b) \
 halAlert();_psTraceStr("psError %s", __FILE__);_psTraceInt(":%d ", __LINE__); \
 _psErrorInt(a,b)

/******************************************************************************/
/*
	OS specific file system apis
*/
#ifdef PS_USE_FILE_SYSTEM
	#ifdef POSIX
	#include <sys/stat.h>
	#endif /* POSIX */
#endif /* PS_USE_FILE_SYSTEM */

#ifdef USE_MULTITHREADING
/******************************************************************************/
/*
	Defines to make library multithreading safe
*/
extern int32 osdepMutexOpen(void);
extern int32 osdepMutexClose(void);

typedef void* psMutex_t;
#endif /* USE_MULTITHREADING */

/******************************************************************************/

#endif /* !PS_UNSUPPORTED_OS */
#endif /* _h_PS_PLATFORM */

