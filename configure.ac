dnl
dnl Copyright (c) 2004-2009 os-cillation.
dnl Copyright (c) 2009-2024 Xfce Development Team <xfce4-dev@xfce.org>
dnl All rights reserved.
dnl
dnl Written by Benedikt Meurer <benny@xfce.org>.
dnl

dnl ***************************
dnl *** Version information ***
dnl ***************************
m4_define([libexo_verinfo], [1:0:1])
m4_define([libexo_version_api], [2])
XDT_VERSION_INIT([4.21.0], [git])

dnl ***************************
dnl *** Initialize autoconf ***
dnl ***************************
AC_COPYRIGHT([Copyright (c) 2004-2010 os-cillation.
Copyright (c) 2004-2025 Xfce Development Team])
AC_INIT([exo], [xdt_version], [https://gitlab.xfce.org/xfce/exo])
AC_PREREQ([2.69])
AC_CONFIG_MACRO_DIRS([m4])
AC_REVISION([xdt_version_build])
AC_CANONICAL_TARGET()

dnl ***************************
dnl *** Initialize automake ***
dnl ***************************
AM_INIT_AUTOMAKE([1.11 dist-bzip2 tar-ustar no-dist-gzip foreign])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE()
AM_SILENT_RULES([yes])

dnl **************************
dnl *** Libtool versioning ***
dnl **************************
LIBEXO_VERINFO=libexo_verinfo()
AC_SUBST([LIBEXO_VERINFO])

dnl ****************************
dnl *** Subst libexo version ***
dnl ****************************
LIBEXO_VERSION_API=libexo_version_api()
LIBEXO_VERSION_MAJOR=xdt_version_major()
LIBEXO_VERSION_MINOR=xdt_version_minor()
LIBEXO_VERSION_MICRO=xdt_version_micro()
AC_SUBST([LIBEXO_VERSION_API])
AC_SUBST([LIBEXO_VERSION_MAJOR])
AC_SUBST([LIBEXO_VERSION_MINOR])
AC_SUBST([LIBEXO_VERSION_MICRO])

dnl ******************************
dnl *** Set helper path prefix ***
dnl ******************************
AC_ARG_WITH([helper-path-prefix],
            [AS_HELP_STRING([--with-helper-path-prefix=PATH],
                            [Path prefix under which helper executables will be installed (default: $libdir)])],
            [HELPER_PATH_PREFIX="$withval"],
            [HELPER_PATH_PREFIX="$libdir"])
AC_SUBST([HELPER_PATH_PREFIX])

dnl ********************************
dnl *** Check for Win32 variants ***
dnl ********************************
AC_MSG_CHECKING([if building for some Win32 platform])
case "$host" in
*-*-mingw*|*-*-cygwin*)
  ac_bm_platform_win32=yes
  ;;
*)
  ac_bm_platform_win32=no
  ;;
esac
AC_MSG_RESULT([$ac_bm_platform_win32])
AM_CONDITIONAL([PLATFORM_WIN32], [test x"$ac_bm_platform_win32" = x"yes"])

dnl **************************
dnl *** Initialize libtool ***
dnl **************************
LT_PREREQ([2.4])
LT_INIT([disable-static])

dnl ********************************
dnl *** Check for basic programs ***
dnl ********************************
AM_PROG_AS()
AC_PROG_CC()
AM_PROG_CC_C_O()
AC_PROG_INSTALL()
AC_CHECK_PROGS([PERL], [perl5 perl])

dnl ***************************************
dnl *** Check for standard header files ***
dnl ***************************************
AC_CHECK_HEADERS([assert.h errno.h fcntl.h fnmatch.h libintl.h \
                  locale.h math.h mmintrin.h paths.h regex.h \
                  signal.h stdarg.h string.h sys/mman.h \
                  sys/stat.h sys/time.h sys/types.h sys/wait.h time.h])

dnl ************************************
dnl *** Check for standard functions ***
dnl ************************************
AC_FUNC_MMAP()
AC_CHECK_FUNCS([realpath])

dnl ***************************************************************************
dnl *** Check for strftime() extensions                                     ***
dnl ***                                                                     ***
dnl *** AC_RUN_IFELSE must be able to build and execute programs natively on   ***
dnl *** the build system, so it fails when cross-compiling unless given an  ***
dnl *** action in the fourth parameter.  This sets its results in a cache   ***
dnl *** variable that users can override to specify target system behavior. ***
dnl ***************************************************************************
AC_CACHE_CHECK([for strftime %E and %O modifiers],
  [ac_cv_have_strftime_extension], [AC_RUN_IFELSE([AC_LANG_SOURCE([[
  #include <string.h>
  #include <time.h>
  int
  main (int argc, char **argv)
  {
    struct tm tm;
    char      buffer[16];
    tm.tm_year = 81;
    if (strftime (buffer, 16, "%EY", &tm) == 4 && strcmp (buffer, "1981") == 0)
      return 0;
    return 1;
  }
    ]])],
    [ac_cv_have_strftime_extension=yes],
    [ac_cv_have_strftime_extension=no],
    [ac_cv_have_strftime_extension=cross])])
if test x"$ac_cv_have_strftime_extension" != x"no"; then
  AC_DEFINE([HAVE_STRFTIME_EXTENSION], 1, [Define if strftime supports %E and %O modifiers.])
fi

dnl ******************************
dnl *** Check for i18n support ***
dnl ******************************
GETTEXT_PACKAGE="$PACKAGE"
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$PACKAGE"], [Name of default gettext domain])
AC_SUBST([GETTEXT_PACKAGE])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])
AC_SEARCH_LIBS([bind_textdomain_codeset], [intl],
  [AC_DEFINE([HAVE_BIND_TEXTDOMAIN_CODESET], [1], [Define to 1 if you have the 'bind_textdomain_codeset' function.])],
  [])

dnl ***********************************
dnl *** Check for required packages ***
dnl ***********************************
XDT_CHECK_PACKAGE([GLIB], [glib-2.0], [2.72.0])
XDT_CHECK_PACKAGE([GIO], [gio-2.0], [2.72.0])
XDT_CHECK_PACKAGE([GTK], [gtk+-3.0], [3.24.0])
XDT_CHECK_PACKAGE([GTHREAD], [gthread-2.0], [2.72.0])
XDT_CHECK_PACKAGE([LIBXFCE4UTIL], [libxfce4util-1.0], [4.17.2])
XDT_CHECK_PACKAGE([LIBXFCE4UI], [libxfce4ui-2], [4.15.1])
XDT_CHECK_OPTIONAL_PACKAGE([GIO_UNIX], [gio-unix-2.0], [2.72.0], [gio-unix], [GIO-Unix features])

dnl *************************
dnl *** Check for gtk-doc ***
dnl *************************
GTK_DOC_CHECK([1.20])

dnl ***********************************
dnl *** Check for debugging support ***
dnl ***********************************
XDT_FEATURE_DEBUG([xdt_debug_default])

dnl **************************************
dnl *** Check for linker optimizations ***
dnl **************************************
XDT_FEATURE_LINKER_OPTS()

dnl ****************************************
dnl *** Check for ELF visibility support ***
dnl ****************************************
AC_ARG_ENABLE([visibility], AS_HELP_STRING([--disable-visibility],[Do not use ELF visibility attributes]), [], [enable_visibility=yes])
have_gnuc_visibility=no
if test x"$enable_visibility" != x"no"; then
  dnl Check whether the compiler supports the visibility attribute
  save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS -Wall -Werror"
  AC_MSG_CHECKING([whether $CC supports the GNUC visibility attribute])
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
  [
    void test_default (void);
    void test_hidden (void);

    void __attribute__ ((visibility("default"))) test_default (void) {}
    void __attribute__ ((visibility("hidden"))) test_hidden (void) {}

    int main (int argc, char **argv) { test_default (); test_hidden (); return 0; }
  ])],
  [
    have_gnuc_visibility=yes
    AC_MSG_RESULT([yes])
  ],
  [
    AC_MSG_RESULT([no])
  ])
  CFLAGS="$save_CFLAGS"
fi
if test x"$have_gnuc_visibility" = x"yes"; then
  CPPFLAGS="$CPPFLAGS -DHAVE_GNUC_VISIBILITY"
fi
AM_CONDITIONAL([HAVE_GNUC_VISIBILITY], [test x"$have_gnuc_visibility" = x"yes"])

dnl *************************************
dnl *** Use GSEAL if possible         ***
dnl *************************************
XDT_SUPPORTED_FLAGS([CFLAGS], [-DGSEAL_ENABLE])

AC_CONFIG_FILES([
Makefile
docs/Makefile
docs/reference/Makefile
docs/man/Makefile
exo/Makefile
exo/exo-2.pc
exo/exo-config.h
exo-desktop-item-edit/Makefile
exo-open/Makefile
pixmaps/Makefile
po/Makefile.in
tests/Makefile
])
AC_OUTPUT

dnl ***************************
dnl *** Print configuration ***
dnl ***************************
echo
echo "Build Configuration:"
echo
echo "* Debug Support:        $enable_debug"
echo "* GNU Visibility:       $have_gnuc_visibility"
echo
