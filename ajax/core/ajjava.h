#ifndef ajjava_h
#define ajjava_h

#ifdef HAVE_JAVA



#ifdef __cplusplus
extern "C" {
#endif


#include <jni.h>



#define JBUFFLEN 10000

#define COMM_AUTH      1
#define EMBOSS_FORK    2
#define MAKE_DIRECTORY 3
#define DELETE_FILE    4
#define DELETE_DIR     5
#define LIST_FILES     6
#define LIST_DIRS      7
#define GET_FILE       8
#define PUT_FILE       9

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


/* Jembossctl functions */

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_userAuth
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment);

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_forkEmboss
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring cline, jstring direct);

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_makeDir
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring direct);

JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_delFile
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring filename);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_delDir
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring direct);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_listFiles
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring direct);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_listDirs
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring direct);


JNIEXPORT jbyteArray JNICALL Java_org_emboss_jemboss_parser_Ajax_getFile
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring filename);


JNIEXPORT jboolean JNICALL Java_org_emboss_jemboss_parser_Ajax_putFile
(JNIEnv *env, jobject obj, jstring door, jstring key,
 jstring environment, jstring filename, jbyteArray arr);


#ifdef __cplusplus
}
#endif
#endif
#endif
