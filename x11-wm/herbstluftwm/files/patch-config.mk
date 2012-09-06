--- config.mk.orig	2012-09-06 15:30:57.839022355 +0800
+++ config.mk	2012-09-06 15:40:29.367301107 +0800
@@ -7,8 +7,8 @@
 XINERAMALIBS = -L${X11LIB} -lXinerama
 XINERAMAFLAGS = -DXINERAMA
 
-INCS = -Isrc/ -I/usr/include -I${X11INC}  `pkg-config --cflags glib-2.0`
-LIBS = -L/usr/lib -lc -L${X11LIB} -lX11 $(XINERAMALIBS) `pkg-config --libs glib-2.0`
+INCS = -Isrc/ -I/usr/include -I/usr/local/include -I${X11INC}  `pkg-config --cflags glib-2.0`
+LIBS = -L/usr/lib -L/usr/local/lib -lc -L${X11LIB} -lX11 $(XINERAMALIBS) `pkg-config --libs glib-2.0`
 
 ifeq ($(shell uname),Linux)
 LIBS += -lrt
@@ -36,7 +36,7 @@
 DOCDIR = $(DATADIR)/doc/herbstluftwm
 EXAMPLESDIR = $(DOCDIR)/examples
 LICENSEDIR = $(DOCDIR)
-SYSCONFDIR = /etc
+SYSCONFDIR = $(PREFIX)/etc
 CONFIGDIR = $(SYSCONFDIR)/xdg/herbstluftwm
 XSESSIONSDIR = $(DATADIR)/xsessions
 ZSHCOMPLETIONDIR = $(DATADIR)/zsh/functions/Completion/X/
