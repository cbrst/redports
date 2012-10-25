--- src/truetype/ttobjs.h	2012-10-25 13:49:35.514523485 +0800
+++ src/truetype/ttobjs.h	2012-10-25 13:52:36.754527185 +0800
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
 
