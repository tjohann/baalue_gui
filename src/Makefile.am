# Makefile.am

bin_PROGRAMS = baalue

baalue_SOURCES =             	\
	main.c    	        \
	gettext.h


baalue_CFLAGS =             	\
	$(LIBCONFIG_CFLAGS)     \
	$(LIBBAALUE_CFLAGS)     \
	-DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/../lib


baalue_LDADD =    	        \
	$(LIBCONFIG_LIBS)   	\
	$(LIBBAALUE_LIBS)      	\
	$(LIBINTL) ../lib/libservice.a
