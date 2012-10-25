--- src/cairo-wideint-type-private.h.orig	2012-10-25 14:50:57.584526476 +0800
+++ src/cairo-wideint-type-private.h	2012-10-25 14:50:57.606523056 +0800
@@ -121,6 +121,11 @@
     cairo_int64_t	rem;
 } cairo_quorem64_t;
 
+/* Intrinsic __uint128_t support is broken on gcc 3.4/sparc. */
+#if __GNUC__ == 3 && __GNUC_MINOR__ == 4
+#undef HAVE___UINT128_T
+#endif
+
 /* gcc has a non-standard name. */
 #if HAVE___UINT128_T && !HAVE_UINT128_T
 typedef __uint128_t uint128_t;
