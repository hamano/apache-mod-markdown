ACLOCAL_AMFLAGS=-I m4

## This is the shared library to be built                                       
#lib_LTLIBRARIES = libmodmarkdown.la

apache_CFLAGS=`@apxs@ -q CFLAGS` `@apxs@ -q EXTRA_INCLUDES`
apache_LIBEXECDIR=`@apxs@ -q LIBEXECDIR`

## Define the source file for the module
CFLAGS = $(apache_CFLAGS) -I@discount_dir@/include
LDFLAGS = -L@discount_dir@/lib -lmarkdown
CPPFLAGS = -D_LARGEFILE64_SOURCE
#DEFS =
CLEANFILES = *.la *.o *.slo

all: mod_markdown.la

mod_markdown.la: mod_markdown.c
	@apxs@ -c $(CFLAGS) $(LDFLAGS) $^

install: mod_markdown.la
	@apxs@ -n markdown -i $^

