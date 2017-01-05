ACLOCAL_AMFLAGS=-I m4

CFLAGS = -I@discount_dir@/include
LDFLAGS = -L@discount_dir@/lib -lmarkdown
CPPFLAGS = -D_LARGEFILE64_SOURCE
#DEFS =
CLEANFILES = *.la *.o *.slo

all: mod_markdown.la

mod_markdown.la: mod_markdown.c
	@apxs@ -c $(CFLAGS) $(LDFLAGS) $^

install: mod_markdown.la
	@apxs@ -n markdown -i $^

