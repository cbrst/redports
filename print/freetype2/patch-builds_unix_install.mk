--- builds/unix/install.mk	2012-09-05 21:25:10.271019092 +0800
+++ builds/unix/install.mk	2012-09-05 21:25:16.047018422 +0800
@@ -29,7 +29,7 @@
 #
 install: $(PROJECT_LIBRARY)
 	$(MKINSTALLDIRS) $(DESTDIR)$(libdir)                               \
-                         $(DESTDIR)$(libdir)/pkgconfig                     \
+                         $(DESTDIR)$(prefix)/libdata/pkgconfig                     \
                          $(DESTDIR)$(includedir)/freetype2/freetype/config \
                          $(DESTDIR)$(includedir)/freetype2/freetype/cache  \
                          $(DESTDIR)$(bindir)                               \
@@ -59,7 +59,7 @@
 	$(INSTALL_SCRIPT) -m 644 $(BUILD_DIR)/freetype2.m4 \
           $(DESTDIR)$(datadir)/aclocal/freetype2.m4
 	$(INSTALL_SCRIPT) -m 644 $(OBJ_BUILD)/freetype2.pc \
-          $(DESTDIR)$(libdir)/pkgconfig/freetype2.pc
+          $(DESTDIR)$(prefix)/libdata/pkgconfig/freetype2.pc
 
 
 uninstall:
@@ -72,7 +72,7 @@
 	-$(DELETE) $(DESTDIR)$(includedir)/ft2build.h
 	-$(DELETE) $(DESTDIR)$(bindir)/freetype-config
 	-$(DELETE) $(DESTDIR)$(datadir)/aclocal/freetype2.m4
-	-$(DELETE) $(DESTDIR)$(libdir)/pkgconfig/freetype2.pc
+	-$(DELETE) $(DESTDIR)$(prefix)/libdata/pkgconfig/freetype2.pc
 
 
 check:
