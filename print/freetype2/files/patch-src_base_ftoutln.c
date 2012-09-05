--- src/base/ftoutln.c	2012-09-05 21:25:10.159019303 +0800
+++ src/base/ftoutln.c	2012-09-05 21:25:37.809027974 +0800
@@ -887,8 +887,30 @@
     FT_Angle    rotate, angle_in, angle_out;
     FT_Int      c, n, first;
     FT_Int      orientation;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+    int checked_use_various_tweaks_env = 0;
+    FT_Bool use_various_tweaks = FALSE;
 
-
+    if ( checked_use_various_tweaks_env == 0 )
+    {
+      char *use_various_tweaks_env = getenv( "INFINALITY_FT_USE_VARIOUS_TWEAKS" );
+      if ( use_various_tweaks_env != NULL )
+      {
+        if ( strcasecmp(use_various_tweaks_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(use_various_tweaks_env, "true") == 0)
+            use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "1") == 0)
+            use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "on") == 0)
+            use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "yes") == 0)
+            use_various_tweaks = TRUE;
+        }
+      }
+      checked_use_various_tweaks_env = 1;
+    }
+#endif
     if ( !outline )
       return FT_Err_Invalid_Argument;
 
@@ -957,7 +979,13 @@
         }
 
         outline->points[n].x = v_cur.x + strength + in.x;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET         
+        if (!use_various_tweaks)
+#endif          
         outline->points[n].y = v_cur.y + strength + in.y;
+        
+        /* OSX ? */
+        /*  v_cur.y + FT_PIX_ROUND(strength) + in.y; */
 
         v_prev = v_cur;
         v_cur  = v_next;
@@ -969,6 +997,99 @@
     return FT_Err_Ok;
   }
 
+  
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET  
+  /* documentation is in ftoutln.h */
+
+  FT_EXPORT_DEF( FT_Error )
+  FT_Outline_Embolden_XY( FT_Outline*  outline,
+                       FT_Pos       strength_x,
+                       FT_Pos       strength_y  )
+  {
+    FT_Vector*  points;
+    FT_Vector   v_prev, v_first, v_next, v_cur;
+    FT_Angle    rotate, angle_in, angle_out;
+    FT_Int      c, n, first;
+    FT_Int      orientation;
+
+    if ( !outline )
+      return FT_Err_Invalid_Argument;
+
+    orientation = FT_Outline_Get_Orientation( outline );
+    if ( orientation == FT_ORIENTATION_NONE )
+    {
+      if ( outline->n_contours )
+        return FT_Err_Invalid_Argument;
+      else
+        return FT_Err_Ok;
+    }
+
+    if ( orientation == FT_ORIENTATION_TRUETYPE )
+      rotate = -FT_ANGLE_PI2;
+    else
+      rotate = FT_ANGLE_PI2;
+
+    points = outline->points;
+
+    first = 0;
+    for ( c = 0; c < outline->n_contours; c++ )
+    {
+      int  last = outline->contours[c];
+
+
+      v_first = points[first];
+      v_prev  = points[last];
+      v_cur   = v_first;
+
+      for ( n = first; n <= last; n++ )
+      {
+        FT_Vector  in, out;
+        FT_Angle   angle_diff;
+        FT_Pos     d, dy;
+        FT_Fixed   scale;
+
+
+        if ( n < last )
+          v_next = points[n + 1];
+        else
+          v_next = v_first;
+
+        /* compute the in and out vectors */
+        in.x = v_cur.x - v_prev.x;
+        in.y = v_cur.y - v_prev.y;
+
+        out.x = v_next.x - v_cur.x;
+        out.y = v_next.y - v_cur.y;
+
+        angle_in   = FT_Atan2( in.x, in.y );
+        angle_out  = FT_Atan2( out.x, out.y );
+        angle_diff = FT_Angle_Diff( angle_in, angle_out );
+        scale      = FT_Cos( angle_diff / 2 );
+
+        if ( scale < 0x4000L && scale > -0x4000L )
+          in.x = in.y = 0;
+        else
+        {
+          d = FT_DivFix( strength_x, scale );
+          dy = FT_DivFix( strength_y, scale );
+          
+          FT_Vector_From_Polar( &in, d, angle_in + angle_diff / 2 - rotate );
+          FT_Vector_From_Polar( &in, dy, angle_in + angle_diff / 2 - rotate );          
+        }
+
+        outline->points[n].x = v_cur.x + strength_x + in.x;
+        outline->points[n].y = v_cur.y + strength_y + in.y;
+
+        v_prev = v_cur;
+        v_cur  = v_next;
+      }
+
+      first = last + 1;
+    }
+
+    return FT_Err_Ok;
+  }  
+#endif  
 
   /* documentation is in ftoutln.h */
 
