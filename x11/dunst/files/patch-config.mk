--- config.mk.orig	2012-09-11 19:04:10.134104466 +0800
+++ config.mk	2012-09-11 19:04:54.018214365 +0800
@@ -1,14 +1,14 @@
 # paths
-PREFIX = /usr/local
-MANPREFIX = ${PREFIX}/share/man
+PREFIX = %%PREFIX%%
+MANPREFIX = %%MANPREFIX%%
 
 VERSION="0.3.1"
 
-X11INC = /usr/X11R6/include
-X11LIB = /usr/X11R6/lib
+X11INC = %%LOCALBASE%%/include
+X11LIB = %%LOCALBASE%%/lib
 
 # Xft, comment if you don't want it
-XFTINC = -I/usr/include/freetype2
+XFTINC = -I%%LOCALBASE%%/include/freetype2
 XFTLIBS  = -lXft
 
 # Xinerama, comment if you don't want it
@@ -24,5 +24,5 @@
 
 # flags
 CPPFLAGS = -D_BSD_SOURCE -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS} ${INIFLAGS}
-CFLAGS   = -g --std=c99 -pedantic -Wall -Wno-overlength-strings -Os ${INCS} ${CPPFLAGS}
+CFLAGS   = -g --std=c99 -Wno-overlength-strings ${INCS} ${CPPFLAGS}
 LDFLAGS  = ${LIBS}
