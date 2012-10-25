--- src/base/ftsynth.c	2012-10-25 13:49:35.465524656 +0800
+++ src/base/ftsynth.c	2012-10-25 13:52:45.977527120 +0800
@@ -88,7 +88,26 @@
     FT_Face     face    = slot->face;
     FT_Error    error;
     FT_Pos      xstr, ystr;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+    int checked_use_various_tweaks_env = 0;
+    FT_Bool use_various_tweaks = FALSE;
 
+    if ( checked_use_various_tweaks_env == 0 )
+    {
+      char *use_various_tweaks_env = getenv( "INFINALITY_FT_USE_VARIOUS_TWEAKS" );
+      if ( use_various_tweaks_env != NULL )
+      {
+        if ( strcasecmp(use_various_tweaks_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(use_various_tweaks_env, "true") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "1") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "on") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "yes") == 0) use_various_tweaks = TRUE;
+        }
+      }
+      checked_use_various_tweaks_env = 1;
+    }
+#endif
 
     if ( slot->format != FT_GLYPH_FORMAT_OUTLINE &&
          slot->format != FT_GLYPH_FORMAT_BITMAP  )
@@ -101,6 +120,11 @@
 
     if ( slot->format == FT_GLYPH_FORMAT_OUTLINE )
     {
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+      if ( use_various_tweaks )
+        (void)FT_Outline_EmboldenXY( &slot->outline, xstr, FT_PIX_FLOOR( ystr ) );
+      else
+#endif
       /* ignore error */
       (void)FT_Outline_EmboldenXY( &slot->outline, xstr, ystr );
     }
@@ -141,6 +165,9 @@
 
     slot->metrics.width       += xstr;
     slot->metrics.height      += ystr;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    /*if ( !use_various_tweaks ) */
+#endif
     slot->metrics.horiAdvance += xstr;
     slot->metrics.vertAdvance += ystr;
 
