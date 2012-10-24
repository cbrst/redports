--- src/xftdpy.c	2009-10-16 23:27:08.000000000 +0200
+++ src/xftdpy.c	2010-11-03 11:56:52.780577002 +0100
@@ -369,6 +369,10 @@
 	goto bail1;
     if (!_XftDefaultInitInteger (dpy, pat, FC_RGBA))
 	goto bail1;
+#ifdef FC_LCD_FILTER
+    if (!_XftDefaultInitInteger (dpy, pat, FC_LCD_FILTER))
+	goto bail1;
+#endif
     if (!_XftDefaultInitBool (dpy, pat, FC_ANTIALIAS))
 	goto bail1;
 #ifdef FC_EMBOLDEN
@@ -521,6 +525,14 @@
 			      XftDefaultGetInteger (dpy, FC_RGBA, screen, 
 						    subpixel));
     }
+#ifdef FC_LCD_FILTER
+    if (FcPatternGet (pattern, FC_LCD_FILTER, 0, &v) == FcResultNoMatch)
+    {
+	FcPatternAddInteger (pattern, FC_LCD_FILTER,
+			     XftDefaultGetInteger (dpy, FC_LCD_FILTER, screen,
+						   FC_LCD_DEFAULT));
+    }
+#endif
     if (FcPatternGet (pattern, FC_MINSPACE, 0, &v) == FcResultNoMatch)
     {
 	FcPatternAddBool (pattern, FC_MINSPACE,
