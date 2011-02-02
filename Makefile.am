## This is the shared library to be built                                       
lib_LTLIBRARIES = libmodmarkdown.la

## Define the source file for the module
libmodmarkdown_la_SOURCES = mod_markdown.c
libmodmarkdown_la_CFLAGS = -DAPACHE_DIR=\"@apache_dir@\"
libmodmarkdown_la_LDFLAGS = -L@discount_dir@/ -lmarkdown

## Define that an include directory is required.
INCLUDES = -I@apache_dir@/include -I@discount_dir@/ -D_LARGEFILE64_SOURCE
#DEFS =

install: .libs/libmodmarkdown.so
	install $< @apache_dir@/modules/mod_markdown.so

