dnl $Id$
dnl
dnl Copyright (c) 2004-2005 os-cillation.
dnl All rights reserved.
dnl
dnl Written by  Stephane Bortzmeyer <bortzmeyer@pasteur.fr> and Benedikt
dnl Meurer <benny@xfce.org>.
dnl


AC_DEFUN([AC_PROG_JAVAC],
[
  AC_REQUIRE([AC_EXEEXT])dnl
  if test "x$JAVAPREFIX" = x; then
    test "x$JAVAC" = x && AC_CHECK_PROGS([JAVAC], "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT)
  else
    test "x$JAVAC" = x && AC_CHECK_PROGS([JAVAC], "gcj$EXEEXT -C" guavac$EXEEXT jikes$EXEEXT javac$EXEEXT, $JAVAPREFIX)
  fi
  test "x$JAVAC" = x && AC_MSG_ERROR([no acceptable Java compiler found in \$PATH])
  AC_PROG_JAVAC_WORKS()
  AC_PROVIDE([$0])dnl
])


AC_DEFUN([AC_PROG_JAVAC_WORKS],
[
  AC_CACHE_CHECK([if $JAVAC works], ac_cv_prog_javac_works, [
JAVA_TEST=Test.java
CLASS_TEST=Test.class
cat << \EOF > $JAVA_TEST
/* [#]line __oline__ "configure" */
public class Test {
}
EOF
if AC_TRY_COMMAND($JAVAC $JAVACFLAGS $JAVA_TEST) >/dev/null 2>&1; then
  ac_cv_prog_javac_works=yes
else
  AC_MSG_ERROR([The Java compiler $JAVAC failed (see config.log, check the CLASSPATH?)])
  echo "configure: failed program was:" >&AC_FD_CC
  cat $JAVA_TEST >&AC_FD_CC
fi
rm -f $JAVA_TEST $CLASS_TEST
  ])
  AC_PROVIDE([$0])dnl
])


AC_DEFUN([AC_PROG_JAVAH],
[
  AC_REQUIRE([AC_PROG_CPP])dnl
  AC_PATH_PROG([JAVAH], javah)
  if test x"`eval 'echo $ac_cv_path_JAVAH'`" != x ; then
    AC_TRY_CPP([#include <jni.h>],,[
      ac_save_CPPFLAGS="$CPPFLAGS"
changequote(, )dnl
      ac_dir=`echo $ac_cv_path_JAVAH | sed 's,\(.*\)/[^/]*/[^/]*$,\1/include,'`
      ac_machdep=`echo $build_os | sed 's,[-0-9].*,,' | sed 's,cygwin,win32,'`
changequote([, ])dnl
      CPPFLAGS="$ac_save_CPPFLAGS -I$ac_dir -I$ac_dir/$ac_machdep"
      AC_TRY_CPP([#include <jni.h>],
                 ac_save_CPPFLAGS="$CPPFLAGS",
                 AC_MSG_WARN([unable to include <jni.h>]))
      CPPFLAGS="$ac_save_CPPFLAGS"])
  fi
])


AC_DEFUN([AC_PROG_JAR],
[
  AC_REQUIRE([AC_EXEEXT])dnl
  if test "x$JAVAPREFIX" = x; then
    test "x$JAR" = x && AC_CHECK_PROGS([JAR], fastjar$EXEEXT jar$EXEEXT)
  else
    test "x$JAR" = x && AC_CHECK_PROGS([JAR], jar, $JAVAPREFIX)
  fi
  test "x$JAR" = x && AC_MSG_ERROR([no acceptable jar program found in \$PATH])
  AC_PROVIDE([$0])dnl
])
