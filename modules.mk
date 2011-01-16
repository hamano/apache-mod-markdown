mod_markdown.la: mod_markdown.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_markdown.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_markdown.la
