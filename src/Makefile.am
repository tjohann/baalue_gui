#
# Makefile.am
#

#AM_LDFLAGS = -Wl,--gc-sections -Wl,--as-needed
AM_CFLAGS = ${my_CFLAGS}

bin_PROGRAMS = baalue_gui

baalue_gui_SOURCES =            \
	main.c    	        \
	gui.c			\
	threads.c		\
	messages.c		\
	common.h		\
	gettext.h

baalue_gui_CFLAGS = $(AM_CFLAGS)\
	$(LIBGTK_CFLAGS)	\
	$(LIBCONFIG_CFLAGS)     \
	$(LIBBAALUE_CFLAGS)     \
	-DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/../lib

#baalue_gui_LDFLAGS = $(AM_LDFLAGS)

baalue_gui_LDADD =  		\
	$(LIBGTK_LIBS)		\
	$(LIBCONFIG_LIBS)   	\
	$(LIBBAALUE_LIBS)      	\
	$(LIBINTL) ../lib/libservice.a
