#ifndef ajjava_h
#define ajjava_h

#ifdef HAVE_JAVAE

#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_emboss_Jemboss_parser_Ajax_seqType 
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
#endif
