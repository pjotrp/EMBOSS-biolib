#ifdef __cplusplus
extern "C"
{
#endif

#ifndef ajmem_h
#define ajmem_h

#include "ajdefine.h"
#include "ajexcept.h"

#ifndef HAVE_MEMMOVE
#define ajMemMove(d,s,l) bcopy (s,d,l)
#else
#define ajMemMove(d,s,l) memmove(d,s,l)
#endif

void *ajMemAlloc (ajlong nbytes,
	const char *file, ajint line, AjBool nofail);
void *ajMemCalloc(ajlong count, ajlong nbytes,
	const char *file, ajint line, AjBool nofail);
void *ajMemCalloc0(ajlong count, ajlong nbytes,
	const char *file, ajint line, AjBool nofail);
void ajMemFree(void *ptr,
	const char *file, ajint line);
void *ajMemResize(void *ptr, ajlong nbytes,
	const char *file, ajint line, AjBool nofail);
ajint *ajMemArrB (size_t size);
ajint *ajMemArrI (size_t size);
float *ajMemArrF (size_t size);
void ajMemStat (const char* title);
void ajMemExit (void);

#define AJALLOC(nbytes) \
	ajMemAlloc((nbytes), __FILE__, __LINE__, AJFALSE)
#define AJALLOC0(nbytes) \
	ajMemCalloc0(1, (nbytes), __FILE__, __LINE__, AJFALSE)
#define AJCALLOC(count, nbytes) \
	ajMemCalloc((count), (nbytes), __FILE__, __LINE__, AJFALSE)
#define AJCALLOC0(count, nbytes) \
	ajMemCalloc0((count), (nbytes), __FILE__, __LINE__, AJFALSE)

#define AJNEW(p) ((p) = AJALLOC((ajlong)sizeof *(p)))
#define AJCNEW(p,c) ((p) = AJCALLOC(c, (ajlong)sizeof *(p)))
#define AJNEW0(p) ((p) = AJCALLOC0(1, (ajlong)sizeof *(p)))
#define AJCNEW0(p,c) ((p) = AJCALLOC0(c, (ajlong)sizeof *(p)))

#define AJFREE(ptr) ((void)(ajMemFree((ptr), \
	__FILE__, __LINE__), (ptr) = 0))
#define AJRESIZE(ptr, nbytes) 	((ptr) = ajMemResize((ptr), \
	(nbytes), __FILE__, __LINE__, AJFALSE))
#define AJCRESIZE(ptr, nbytes) 	((ptr) = ajMemResize((ptr), \
	(nbytes)*(ajlong)sizeof *(ptr), __FILE__, __LINE__, AJFALSE))
#define AJCRESIZETRY(ptr, nbytes) 	((ptr) = ajMemResize((ptr), \
	(nbytes)*(ajlong)sizeof *(ptr), __FILE__, __LINE__, AJTRUE))
#endif

#ifdef __cplusplus
}
#endif
