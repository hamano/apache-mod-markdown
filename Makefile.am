## This is the shared library to be built                                       
lib_LTLIBRARIES = libmodmarkdown.la

apache_CFLAGS=`@apxs@ -q CFLAGS` -I`@apxs@ -q INCLUDEDIR`
apache_LIBEXECDIR=`@apxs@ -q LIBEXECDIR`

## Define the source file for the module
libmodmarkdown_la_SOURCES = mod_markdown.c
libmodmarkdown_la_CFLAGS = $(apache_CFLAGS) -I@discount_dir@/include
libmodmarkdown_la_LDFLAGS = -L@discount_dir@/lib -lmarkdown

## Define that an include directory is required.
INCLUDES = -D_LARGEFILE64_SOURCE
#DEFS =

install: .libs/libmodmarkdown.so
	install $< $(apache_LIBEXECDIR)/mod_markdown.so

