--- include/freetype/ftoutln.h	2012-09-05 21:25:10.100018767 +0800
+++ include/freetype/ftoutln.h	2012-09-05 21:25:37.779023728 +0800
@@ -349,6 +349,10 @@
   FT_Outline_Embolden( FT_Outline*  outline,
                        FT_Pos       strength );
 
+  FT_EXPORT( FT_Error )
+  FT_Outline_Embolden_XY( FT_Outline*  outline,
+                       FT_Pos       strength_x,
+                       FT_Pos       strength_y  );  
 
   /*************************************************************************/
   /*                                                                       */
