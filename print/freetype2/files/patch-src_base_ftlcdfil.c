--- src/base/ftlcdfil.c	2012-09-05 21:25:10.160021740 +0800
+++ src/base/ftlcdfil.c	2012-09-05 21:25:37.792026285 +0800
@@ -21,6 +21,9 @@
 #include FT_IMAGE_H
 #include FT_INTERNAL_OBJECTS_H
 
+#include <math.h>
+#include <string.h>
+#include <strings.h>
 
 #ifdef FT_CONFIG_OPTION_SUBPIXEL_RENDERING
 
@@ -287,10 +290,51 @@
                             { 0x00, 0x55, 0x56, 0x55, 0x00 };
     /* the values here sum up to a value larger than 256, */
     /* providing a cheap gamma correction                 */
-    static const FT_Byte  default_filter[5] =
+    static FT_Byte  default_filter[5] =
                             { 0x10, 0x40, 0x70, 0x40, 0x10 };
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+    int checked_filter_params_env = 0;
 
+    if ( checked_filter_params_env == 0 )
+    {
+      char *filter_params = getenv( "INFINALITY_FT_FILTER_PARAMS" );
+      if ( filter_params != NULL && strcmp(filter_params, "") != 0 )
+      {
+        float f1, f2, f3, f4, f5;
+
+        if ( strcasecmp(filter_params, "default" ) != 0)
+        {
+          int args_assigned = 0;
+          args_assigned = sscanf ( filter_params, "%f %f %f %f %f", &f1, &f2, &f3, &f4, &f5 );
 
+          if ( args_assigned == 5 )
+          {
+            if ( f1 + f2 + f3 + f4 + f5 > 5 )
+            {
+              /* Assume we were given integers instead of floats */
+              /* 0 to 100 */
+              default_filter[0] = (FT_Byte) (f1 * 2.55f + 0.5f);
+              default_filter[1] = (FT_Byte) (f2 * 2.55f + 0.5f);
+              default_filter[2] = (FT_Byte) (f3 * 2.55f + 0.5f);
+              default_filter[3] = (FT_Byte) (f4 * 2.55f + 0.5f);
+              default_filter[4] = (FT_Byte) (f5 * 2.55f + 0.5f);
+            }
+            else
+            {
+              /* Assume we were given floating point values */
+              /* 0 to 1.0 */
+              default_filter[0] = (FT_Byte) (f1 * 255.0f + 0.5f);
+              default_filter[1] = (FT_Byte) (f2 * 255.0f + 0.5f);
+              default_filter[2] = (FT_Byte) (f3 * 255.0f + 0.5f);
+              default_filter[3] = (FT_Byte) (f4 * 255.0f + 0.5f);
+              default_filter[4] = (FT_Byte) (f5 * 255.0f + 0.5f);
+            }
+          }
+        }
+      }
+      checked_filter_params_env = 1;
+    }
+#endif
     if ( !library )
       return FT_Err_Invalid_Argument;
 
