--- src/truetype/ttobjs.h	2012-09-05 21:25:10.147018974 +0800
+++ src/truetype/ttobjs.h	2012-09-05 21:25:27.943019537 +0800
@@ -178,6 +178,7 @@
     FT_Long  end;        /* where does it end?                 */
     FT_UInt  opc;        /* function #, or instruction code    */
     FT_Bool  active;     /* is it active?                      */
+    FT_Bool  inline_delta; /* is function that defines inline delta? */
 
   } TT_DefRecord, *TT_DefArray;
 
@@ -190,7 +191,7 @@
   {
     FT_Fixed    xx, xy;     /* transformation matrix coefficients */
     FT_Fixed    yx, yy;
-    FT_F26Dot6  ox, oy;     /* offsets        */
+    FT_F26Dot6  ox, oy;     /* offsets                            */
 
   } TT_Transform;
 
