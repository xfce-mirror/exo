# $Id$

INCLUDES =								\
	-I$(top_srcdir)							\
	-DEXO_API_SUBJECT_TO_CHANGE					\
	-DG_LOG_DOMAIN=\"exojava\"

JAVA_FILES =								\
	org/xfce/exo/MimeDatabase.java					\
	org/xfce/exo/MimeInfo.java

JAVAH_FILES =								\
	$(subst /,_,$(JAVA_FILES:.java=.h))

CLASS_FILES =								\
	$(JAVA_FILES:.java=.class)

exojava-$(LIBEXO_VERSION_API).jar: $(CLASS_FILES)
	$(JAR) cf $@ $(CLASS_FILES)

%.class: %.java
	$(JAVAC) $<

%.h: exojava-$(LIBEXO_VERSION_API).jar
	CLASSPATH=".:$(CLASSPATH)" $(JAVAH) -jni $(subst _,.,$(@:.h=))

jnidir = $(libdir)/jni
jni_LTLIBRARIES = libexojava-0.3.la

libexojava_0_3_la_SOURCES =						\
	$(JAVAH_FILES)							\
	org_xfce_exo.c

libexojava_0_3_la_DEPENDENCIES =					\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la

libexojava_0_3_la_CFLAGS =						\
	$(LIBXFCEGUI4_CFLAGS)

libexojava_0_3_la_LDFLAGS =						\
	-no-undefined

libexojava_0_3_la_LIBADD =						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_API).la

all-local: exojava-$(LIBEXO_VERSION_API).jar

CLEANFILES =								\
	$(CLASS_FILES)							\
	$(JAVAH_FILES)							\
	exojava-$(LIBEXO_VERSION_API).jar

BUILT_SOURCES =								\
	$(JAVAH_FILES)

.PHONY: all-local

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
