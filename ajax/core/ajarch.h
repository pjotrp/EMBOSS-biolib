#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajarch_h
#define ajarch_h


#include <sys/types.h>
#include <stdio.h>

#if !defined(AJ_LinuxLF) && !defined(AJ_SolarisLF) && !defined(AJ_IRIXLF)
typedef int ajint;
typedef long ajlong;
typedef unsigned int ajuint;
typedef short ajshort;
typedef unsigned short ajushort;
typedef unsigned long ajulong;
#endif


#ifdef AJ_LinuxLF
typedef int ajint;
typedef long long ajlong;
typedef unsigned int ajuint;
typedef short ajshort;
typedef unsigned short ajushort;
typedef unsigned long long ajulong;
#endif


#ifdef AJ_SolarisLF
typedef int ajint;
typedef off_t ajlong;
typedef unsigned int ajuint;
typedef short ajshort;
typedef unsigned short ajushort;
typedef unsigned long ajulong;
#define ftell(a) ftello(a)
#define fseek(a,b,c) fseeko(a,b,c)
#endif

#ifdef AJ_IRIXLF
typedef int ajint;
typedef off64_t ajlong;
typedef unsigned int ajuint;
typedef short ajshort;
typedef unsigned short ajushort;
typedef unsigned long ajulong;
#define ftell(a) ftell64(a)
#define fseek(a,b,c) fseek64(a,b,c)
#endif



#endif

#ifdef __cplusplus
}
#endif
