#ifndef _MY_UNISTD_H
#define _MY_UNISTD_H

/* This file intended to serve as a drop-in replacement for 
 *  unistd.h on Windows
 *  Please add functionality as neeeded 
 */

#ifdef _MSC_VER

#include <stdlib.h>
#include <io.h>
#include <getopt.h> /* getopt from: http://www.pwilson.net/sample.html. */
#include <process.h> /* for getpid() and the exec..() family */

#define srandom srand
#define random rand

//const W_OK = 2;
//const R_OK = 4;

#define access _access
#define ftruncate _chsize

#define ssize_t int

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
/* should be in some equivalent to <sys/types.h> */
//typedef __int8            int8_t;
typedef __int16           int16_t; 
//typedef __int32           int32_t;
typedef __int64           int64_t;
typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
#else

//#include <bits/stdint-uintn.h>
#include <unistd.h>

#endif

#endif /* unistd.h  */
