## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = 1.6

# The name of the module.
DOC_MODULE=exo

# The top-level SGML file.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS=--deprecated-guards="EXO_DISABLE_DEPRECATED"

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../exo

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# Used for dependencies
HFILE_GLOB=$(top_srcdir)/exo/*.h
CFILE_GLOB=$(top_srcdir)/exo/*.c

# Header files to ignore when scanning
IGNORE_HFILES=

# Extra files to add when scanning (relative to $srcdir)
EXTRA_HFILES=

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by DOC_MAIN_SGML_FILE
content_files = 					\
	version.xml					\
	exo-csource.xml

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
INCLUDES =	 					\
	-DEXO_API_SUBJECT_TO_CHANGE			\
	-I$(top_srcdir) 				\
	-I$(top_builddir) 				\
	$(GTK_CFLAGS)					\
	$(LIBXFCEGUI4_CFLAGS)

GTKDOC_LIBS =  						\
	$(top_builddir)/exo/libexo-$(LIBEXO_VERSION_MAJOR).$(LIBEXO_VERSION_MINOR).la

include $(top_srcdir)/gtk-doc.make

# Other files to distribute
EXTRA_DIST += 						\
	exo-csource.1					\
	version.xml.in

# manual pages
man_MANS =						\
	exo-csource.1

if MAINTAINER_MODE
%.1: %.xml
	xsltproc -nonet http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl $<
endif

# vi:set ts=8 sw=8 noet ai nocindent syntax=automake:
