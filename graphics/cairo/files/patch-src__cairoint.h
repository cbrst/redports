--- src/cairoint.h.orig	2012-10-25 14:50:57.578525167 +0800
+++ src/cairoint.h	2012-10-25 14:50:57.606523056 +0800
@@ -54,6 +54,11 @@
 #define cairo_public __declspec(dllexport)
 #endif
 
+#ifndef INT16_MAX
+#define INT16_MAX 0x7fff
+#define INT16_MIN (-0x7fff-1)
+#endif
+
 #include <assert.h>
 #include <stdlib.h>
 #include <string.h>
