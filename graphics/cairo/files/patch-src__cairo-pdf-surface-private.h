--- src/cairo-pdf-surface-private.h.orig	2012-10-25 14:50:57.585523539 +0800
+++ src/cairo-pdf-surface-private.h	2012-10-25 14:51:11.526555954 +0800
@@ -68,6 +68,7 @@
     cairo_hash_entry_t base;
     unsigned int id;
     cairo_bool_t interpolate;
+    cairo_bool_t mask;
     cairo_pdf_resource_t surface_res;
     int width;
     int height;
