#include "org_xfce_exo_MimeDatabase.h"
#include "org_xfce_exo_MimeInfo.h"

#include <exo/exo.h>


JNIEXPORT jint JNICALL
Java_org_xfce_exo_MimeDatabase_exo_1mime_1database_1get_1default (JNIEnv *env, jclass cls)
{
  return (jint) exo_mime_database_get_default();
}


JNIEXPORT jint JNICALL
Java_org_xfce_exo_MimeDatabase_exo_1mime_1database_1get_1info (JNIEnv *env, jclass cls, jint handle, jstring name)
{
  const gchar *name_g = (*env)->GetStringUTFChars (env, name, NULL);
  ExoMimeInfo *info = exo_mime_database_get_info (EXO_MIME_DATABASE (handle), name_g);
  (*env)->ReleaseStringUTFChars(env, name, name_g);
  return (jint) info;
}


JNIEXPORT jint JNICALL
Java_org_xfce_exo_MimeDatabase_exo_1mime_1database_1get_1info_1for_1file (JNIEnv *env, jclass cls, jint handle, jstring file_path)
{
  const gchar *file_path_g = (*env)->GetStringUTFChars (env, file_path, NULL);
  ExoMimeInfo *info = exo_mime_database_get_info_for_file (EXO_MIME_DATABASE (handle), file_path_g);
  (*env)->ReleaseStringUTFChars(env, file_path, file_path_g);
  return (jint) info;
}



JNIEXPORT jint JNICALL
Java_org_xfce_exo_MimeDatabase_exo_1mime_1database_1get_1info_1from_1file_1name (JNIEnv *env, jclass cls, jint handle, jstring file_name)
{
  const gchar *file_name_g = (*env)->GetStringUTFChars (env, file_name, NULL);
  ExoMimeInfo *info = exo_mime_database_get_info_from_file_name (EXO_MIME_DATABASE (handle), file_name_g);
  (*env)->ReleaseStringUTFChars(env, file_name, file_name_g);
  return (jint) info;
}


JNIEXPORT jstring JNICALL
Java_org_xfce_exo_MimeInfo_exo_1mime_1info_1get_1comment (JNIEnv *env,
                                                          jclass  cls,
                                                          jint    handle)
{
  const gchar *comment = exo_mime_info_get_comment (EXO_MIME_INFO (handle));
  return (*env)->NewStringUTF (env, comment);
}


JNIEXPORT jstring JNICALL
Java_org_xfce_exo_MimeInfo_exo_1mime_1info_1get_1name (JNIEnv *env,
                                                       jclass  cls,
                                                       jint    handle)
{
  const gchar *name = exo_mime_info_get_name (EXO_MIME_INFO (handle));
  return (*env)->NewStringUTF (env, name);
}


