--- include/freetype/internal/ftobjs.h	2012-10-25 13:49:35.566524405 +0800
+++ include/freetype/internal/ftobjs.h	2012-10-25 13:52:36.739525944 +0800
@@ -81,6 +81,14 @@
 #define FT_PIX_ROUND( x )     FT_PIX_FLOOR( (x) + 32 )
 #define FT_PIX_CEIL( x )      FT_PIX_FLOOR( (x) + 63 )
 
+  /*
+   *  These are used in ttinterp.c for subpixel hinting with an
+   *  adjustable grids-per-pixel value.
+   */
+#define FT_PIX_FLOOR_GRID( x, n )  ( (x) & ~( 64 / (n) - 1 ) )
+#define FT_PIX_ROUND_GRID( x, n )  FT_PIX_FLOOR_GRID( (x) + 32 / (n), (n) )
+#define FT_PIX_CEIL_GRID( x, n )   FT_PIX_FLOOR_GRID( (x) + 63 / (n), (n) )
+
 
   /*
    *  Return the highest power of 2 that is <= value; this correspond to
