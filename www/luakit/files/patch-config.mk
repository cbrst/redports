--- config.mk	2012-10-14 14:33:40.324546666 +0800
+++ config.mk	2012-10-14 14:34:08.573539803 +0800
@@ -13,8 +13,8 @@
 # === Paths ==================================================================
 
 PREFIX     ?= /usr/local
-MANPREFIX  ?= $(PREFIX)/share/man
-DOCDIR     ?= $(PREFIX)/share/luakit/docs
+MANPREFIX  ?= $(PREFIX)/man
+DOCDIR     ?= $(PREFIX)/share/doc/luakit
 
 INSTALLDIR := $(DESTDIR)$(PREFIX)
 MANPREFIX  := $(DESTDIR)$(MANPREFIX)
