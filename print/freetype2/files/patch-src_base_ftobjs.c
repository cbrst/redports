--- src/base/ftobjs.c	2012-09-05 21:25:10.156017509 +0800
+++ src/base/ftobjs.c	2012-09-05 21:25:37.805025768 +0800
@@ -513,6 +513,22 @@
   ft_lookup_glyph_renderer( FT_GlyphSlot  slot );
 
 
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
+  static void
+  ft_glyphslot_enlarge_metrics( FT_GlyphSlot slot,
+                                FT_Render_Mode mode )
+  {
+    FT_Glyph_Metrics*  metrics = &slot->metrics;
+    FT_Pos enlarge_cbox = 0;
+    /* enlarge for grayscale rendering */
+    if ( mode == FT_RENDER_MODE_NORMAL ) enlarge_cbox = 64;
+
+    metrics->horiBearingX -= enlarge_cbox;
+    metrics->width += 2*enlarge_cbox;
+  }
+#endif /* FT_CONFIG_OPTION_INFINALITY_PATCHSET */
+
+
 #ifdef GRID_FIT_METRICS
   static void
   ft_glyphslot_grid_fit_metrics( FT_GlyphSlot  slot,
@@ -571,8 +587,27 @@
     FT_Bool       autohint = FALSE;
     FT_Module     hinter;
     TT_Face       ttface = (TT_Face)face;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
 
+    int checked_use_various_tweaks_env = FALSE;
+    FT_Bool use_various_tweaks = FALSE;
 
+    if ( !checked_use_various_tweaks_env )
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
     if ( !face || !face->size || !face->glyph )
       return FT_Err_Invalid_Face_Handle;
 
@@ -652,8 +687,22 @@
     if ( autohint )
     {
       FT_AutoHinter_Service  hinting;
-
-
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
+      if ( use_various_tweaks )
+      {
+        load_flags = 66088;
+        /* attempt to force slight hinting here, but doesn't work */
+        /* the above hack does though, until I can figure out the below */
+
+        /*load_flags &= ~FT_RENDER_MODE_NORMAL;
+        load_flags &= ~FT_LOAD_TARGET_NORMAL;
+        load_flags &= ~FT_LOAD_NO_AUTOHINT;
+        load_flags |= FT_RENDER_MODE_LIGHT;
+        load_flags |= FT_LOAD_TARGET_LIGHT;
+        load_flags |= FT_LOAD_FORCE_AUTOHINT;*/
+        /*printf("%d ", load_flags);*/
+      }
+#endif
       /* try to load embedded bitmaps first if available            */
       /*                                                            */
       /* XXX: This is really a temporary hack that should disappear */
@@ -691,6 +740,10 @@
     }
     else
     {
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET        
+      char* c1 = "CUR_WIDTH";
+      char* c2 = "0";
+#endif      
       error = driver->clazz->load_glyph( slot,
                                          face->size,
                                          glyph_index,
@@ -698,6 +751,14 @@
       if ( error )
         goto Exit;
 
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET  
+      setenv(c1, c2, 1);
+
+      /* fix for sdl_ttf */
+      FT_Render_Mode  mode = FT_LOAD_TARGET_MODE( load_flags );
+      ft_glyphslot_enlarge_metrics( slot, mode );
+#endif
+
       if ( slot->format == FT_GLYPH_FORMAT_OUTLINE )
       {
         /* check that the loaded outline is correct */
