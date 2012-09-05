--- include/freetype/internal/ftobjs.h	2012-09-05 21:25:10.090016118 +0800
+++ include/freetype/internal/ftobjs.h	2012-09-05 21:25:27.907019528 +0800
@@ -81,6 +81,14 @@
 #define FT_PIX_ROUND( x )     FT_PIX_FLOOR( (x) + 32 )
 #define FT_PIX_CEIL( x )      FT_PIX_FLOOR( (x) + 63 )
 
+  /*
+   *  These are used in ttinterp.c for subpixel hinting with an
+   *  adjustable grids-per-pixel value.
+   */
+#define FT_PIX_FLOOR_GRID( x, n )  ( (x) & ~( 64 / n - 1 ) )
+#define FT_PIX_ROUND_GRID( x, n )  FT_PIX_FLOOR_GRID( (x) + 32 / n, n )
+#define FT_PIX_CEIL_GRID( x, n )   FT_PIX_FLOOR_GRID( (x) + 63 / n, n )
+
 
   /*
    *  Return the highest power of 2 that is <= value; this correspond to
