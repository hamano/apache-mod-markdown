ACLOCAL_AMFLAGS=-I m4

## This is the shared library to be built                                       
lib_LTLIBRARIES = libmodmarkdown.la

apache_CFLAGS=`@apxs@ -q CFLAGS` `@apxs@ -q EXTRA_INCLUDES`
apache_LIBEXECDIR=`@apxs@ -q LIBEXECDIR`

## Define the source file for the module
libmodmarkdown_la_SOURCES = mod_markdown.c
libmodmarkdown_la_CFLAGS = $(apache_CFLAGS) -I@discount_dir@/include
libmodmarkdown_la_LDFLAGS = -L@discount_dir@/lib -lmarkdown

## Define that an include directory is required.
AM_CPPFLAGS = -D_LARGEFILE64_SOURCE
#DEFS =

install: libmodmarkdown.la
	@apxs@ -n markdown -i $^

