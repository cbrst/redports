--- src/cairo-xlib-display.c.orig	2012-10-25 14:50:57.597523363 +0800
+++ src/cairo-xlib-display.c	2012-10-25 14:50:57.606523056 +0800
@@ -353,11 +353,7 @@
     /* Prior to Render 0.10, there is no protocol support for gradients and
      * we call function stubs instead, which would silently consume the drawing.
      */
-#if RENDER_MAJOR == 0 && RENDER_MINOR < 10
     display->buggy_gradients = TRUE;
-#else
-    display->buggy_gradients = FALSE;
-#endif
     display->buggy_pad_reflect = FALSE;
     display->buggy_repeat = FALSE;
 
