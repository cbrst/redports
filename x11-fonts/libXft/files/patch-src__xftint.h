--- src/xftint.h	2009-10-16 23:27:08.000000000 +0200
+++ src/xftint.h	2010-11-03 11:56:52.780577002 +0100
@@ -145,6 +145,7 @@
     FcBool		antialias;	/* doing antialiasing */
     FcBool		embolden;	/* force emboldening */
     int			rgba;		/* subpixel order */
+    int			lcd_filter;	/* lcd filter */
     FT_Matrix		matrix;		/* glyph transformation matrix */
     FcBool		transform;	/* non-identify matrix? */
     FT_Int		load_flags;	/* glyph load flags */
