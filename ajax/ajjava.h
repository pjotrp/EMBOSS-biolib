#ifndef ajjava_h
#define ajjava_h

#ifdef HAVE_JAVA

#include <jni.h>


#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqType 
    (JNIEnv *, jobject, jstring);
JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_seqsetType
    (JNIEnv *env, jobject obj, jstring usa);

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_userInfo
    (JNIEnv *env, jobject obj, jstring door, jstring key);

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setuid
    (JNIEnv *env, jclass j, jint uid);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_seteuid
    (JNIEnv *env, jclass j, jint uid);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setgid
    (JNIEnv *env, jclass j, jint gid);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_setegid
    (JNIEnv *env, jclass j, jint gid);

JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getuid
    (JNIEnv *env, jclass j);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_geteuid
    (JNIEnv *env, jclass j);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getgid
    (JNIEnv *env, jclass j);
JNIEXPORT jint JNICALL Java_org_emboss_jemboss_parser_Ajax_getegid
    (JNIEnv *env, jclass j);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_fork
(JNIEnv *env, jobject obj, jstring commandline,
 jstring environment, jstring directory, jint uid, jint gid);



#ifdef __cplusplus
}
#endif
#endif
#endif
