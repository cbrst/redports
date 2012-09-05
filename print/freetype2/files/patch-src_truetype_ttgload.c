--- src/truetype/ttgload.c	2012-09-05 21:25:10.146017934 +0800
+++ src/truetype/ttgload.c	2012-09-05 21:25:37.837025529 +0800
@@ -32,6 +32,7 @@
 #endif
 
 #include "tterrors.h"
+#include "ttsubpix.h"
 
 
   /*************************************************************************/
@@ -149,6 +150,15 @@
     loader->top_bearing  = top_bearing;
     loader->vadvance     = advance_height;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( loader->exec )
+      loader->exec->sph_tweak_flags = 0;
+
+    /* this may not be the right place for this, but it works */
+    if ( loader->exec && loader->exec->enhanced_mode )
+      sph_set_tweaks( loader, glyph_index );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     if ( !loader->linear_def )
     {
       loader->linear_def = 1;
@@ -813,9 +823,51 @@
       loader->pp4 = zone->cur[zone->n_points - 1];
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if  ( loader->exec->sph_tweak_flags & SPH_TWEAK_DEEMBOLDEN )
+      FT_Outline_Embolden(&loader->gloader->current.outline, -24);
+
+    else if  ( loader->exec->sph_tweak_flags & SPH_TWEAK_EMBOLDEN )
+      FT_Outline_Embolden(&loader->gloader->current.outline, 24);
+#endif
     return TT_Err_Ok;
   }
 
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+  FT_LOCAL_DEF( float )
+  scale_test_tweak( TT_Face         face,
+                  FT_String*      family,
+                  int             ppem,
+                  FT_String*      style,
+                  FT_UInt         glyph_index,
+                  SPH_ScaleRule*  rule,
+                  FT_UInt         num_rules )
+  {
+    FT_UInt  i;
+
+
+    /* rule checks may be able to be optimized further */
+    for ( i = 0; i < num_rules; i++ )
+    {
+      if ( family                                    &&
+           ( strcmp( rule[i].family, "" ) == 0     ||
+             strcmp( rule[i].family, family ) == 0 ) )
+        if ( rule[i].ppem == 0    ||
+             rule[i].ppem == ppem )
+          if ( ( style                            &&
+                 strcmp( rule[i].style, "" ) == 0 ) ||
+               strcmp( rule[i].style, style ) == 0  )
+            if ( rule[i].glyph == 0                                ||
+                 FT_Get_Char_Index( (FT_Face)face,
+                                    rule[i].glyph ) == glyph_index )
+      {
+        /* printf( "%s,%d,%s,%c ", family, ppem, style, rule[i].glyph ); */
+        return rule[i].scale;
+      }
+    }
+    return 1.0;
+  }
+#endif
 
   /*************************************************************************/
   /*                                                                       */
@@ -835,6 +887,54 @@
     FT_Outline*     outline;
     FT_Int          n_points;
 
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    int checked_use_various_tweaks_env = 0;
+    FT_Bool use_various_tweaks = FALSE;
+
+    TT_Face     face   = (TT_Face)loader->face;
+    FT_String*  family = face->root.family_name;
+    int         ppem   = loader->size->metrics.x_ppem;
+    FT_String*  style  = face->root.style_name;
+    float       x_scale_factor = 1.0;
+    float       y_scale_factor = 1.0;
+
+    int          fitting_strength = 0;
+    FT_UInt      checked_fitting_strength = 0;
+
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
+
+    if ( checked_fitting_strength == 0)
+    {
+      char *fitting_strength_env = getenv( "INFINALITY_FT_STEM_FITTING_STRENGTH" );
+      if ( fitting_strength_env != NULL )
+      {
+        sscanf ( fitting_strength_env, "%d", &fitting_strength );
+        if      (fitting_strength > 100 ) fitting_strength = 100;
+        else if (fitting_strength < 0 ) fitting_strength = 0;
+      }
+        if      (fitting_strength > 100 ) fitting_strength = 100;
+      checked_fitting_strength = 1;
+    }
+
+#endif /* FT_CONFIG_OPTION_INFINALITY_PATCHSET */
 
     outline  = &gloader->current.outline;
     n_points = outline->n_points;
@@ -888,14 +988,36 @@
       FT_ARRAY_COPY( loader->zone.orus, loader->zone.cur,
                      loader->zone.n_points + 4 );
     }
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    /* scale, but only if enabled and only if TT hinting is being used */
+    if ( fitting_strength > 0 && use_various_tweaks && IS_HINTED( loader->load_flags ) )
+    {
+      x_scale_factor = scale_test_tweak( face, family, ppem, style, loader->glyph_index, X_SCALING_Rules, X_SCALING_RULES_SIZE );
+      /* Enabling this causes problems even though supposedly not being used in rules - need to figure out */
+      /*y_scale_factor = scale_test_tweak( face, family, ppem, style, loader->glyph_index, Y_SCALING_Rules, Y_SCALING_RULES_SIZE );*/
+    }
 
     /* scale the glyph */
-    if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
+#endif
+    if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+      || x_scale_factor != 1.0 || y_scale_factor != 1.0
+#endif
+    )
     {
       FT_Vector*  vec     = outline->points;
       FT_Vector*  limit   = outline->points + n_points;
-      FT_Fixed    x_scale = ((TT_Size)loader->size)->metrics.x_scale;
-      FT_Fixed    y_scale = ((TT_Size)loader->size)->metrics.y_scale;
+      FT_Fixed    x_scale = ((TT_Size)loader->size)->metrics.x_scale
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+      * x_scale_factor
+#endif
+      ;
+
+      FT_Fixed    y_scale = ((TT_Size)loader->size)->metrics.y_scale
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+      * y_scale_factor
+#endif
+      ;
 
 
       for ( ; vec < limit; vec++ )
@@ -917,6 +1039,10 @@
       error = TT_Hint_Glyph( loader, 0 );
     }
 
+    /* crashy */
+    /*if  ( loader->exec->sph_tweak_flags & SPH_TWEAK_DEEMBOLDEN )
+      FT_Outline_Embolden(outline, 24);*/
+
     return error;
   }
 
@@ -1648,12 +1774,27 @@
     {
       FT_Byte*  widthp;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      FT_Bool   enhanced_mode;
+
+
+      enhanced_mode = FT_BOOL( FT_LOAD_TARGET_MODE( loader->load_flags ) !=
+                          FT_RENDER_MODE_MONO );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
       widthp = tt_face_get_device_metrics( face,
                                            size->root.metrics.x_ppem,
                                            glyph_index );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( (
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+        ( enhanced_mode && loader->exec->compatible_widths ) ||
+#endif
+        !enhanced_mode || SPH_OPTION_BITMAP_WIDTHS ) && widthp )
+#else
       if ( widthp )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
         glyph->metrics.horiAdvance = *widthp << 6;
     }
 
@@ -1848,6 +1989,15 @@
     {
       TT_ExecContext  exec;
       FT_Bool         grayscale;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      FT_Bool         subpixel_hinting;
+      FT_Bool         grayscale_hinting;
+#if 0
+      FT_Bool         compatible_widths;
+      FT_Bool         symmetrical_smoothing;
+      FT_Bool         bgr;
+#endif
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 
       if ( !size->cvt_ready )
@@ -1865,11 +2015,88 @@
       if ( !exec )
         return TT_Err_Could_Not_Find_Context;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+      subpixel_hinting = FT_BOOL( ( FT_LOAD_TARGET_MODE( load_flags )
+                                    != FT_RENDER_MODE_MONO )          &&
+                                  SPH_OPTION_SET_SUBPIXEL             );
+
+      if ( subpixel_hinting )
+        grayscale = grayscale_hinting = FALSE;
+
+      else if ( SPH_OPTION_SET_GRAYSCALE )
+      {
+        grayscale = grayscale_hinting = TRUE;
+        subpixel_hinting = FALSE;
+      }
+
+      if ( FT_IS_TRICKY( glyph->face ) )
+      {
+        subpixel_hinting = grayscale_hinting = FALSE;
+      }
+
+      exec->enhanced_mode      = subpixel_hinting || grayscale_hinting;
+      exec->rasterizer_version = SPH_OPTION_SET_RASTERIZER_VERSION;
+      if ( exec->sph_tweak_flags & SPH_TWEAK_RASTERIZER_35 )
+        exec->rasterizer_version = 35;    
+
+#if 1
+      exec->compatible_widths     = SPH_OPTION_SET_COMPATIBLE_WIDTHS;
+      exec->symmetrical_smoothing = FALSE;
+      exec->bgr                   = FALSE;
+#else /* 0 */
+      exec->compatible_widths =
+        FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
+                 TT_LOAD_COMPATIBLE_WIDTHS );
+      exec->symmetrical_smoothing =
+        FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
+                 TT_LOAD_SYMMETRICAL_SMOOTHING );
+      exec->bgr =
+        FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) !=
+                 TT_LOAD_BGR );
+#endif /* 0 */
+
+#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       grayscale =
         FT_BOOL( FT_LOAD_TARGET_MODE( load_flags ) != FT_RENDER_MODE_MONO );
 
+#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       TT_Load_Context( exec, face, size );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+      /* a change from mono to subpixel rendering (and vice versa) */
+      /* requires a re-execution of the CVT program                */
+      if ( subpixel_hinting != exec->subpixel_hinting )
+      {
+        FT_UInt  i;
+
+
+        exec->subpixel_hinting = subpixel_hinting;
+
+        for ( i = 0; i < size->cvt_size; i++ )
+          size->cvt[i] = FT_MulFix( face->cvt[i], size->ttmetrics.scale );
+        tt_size_run_prep( size, pedantic );
+      }
+
+      /* a change from mono to grayscale rendering (and vice versa) */
+      /* requires a re-execution of the CVT program                 */
+      if ( grayscale != exec->grayscale_hinting )
+      {
+        FT_UInt  i;
+
+
+        exec->grayscale_hinting = grayscale_hinting;
+
+        for ( i = 0; i < size->cvt_size; i++ )
+          size->cvt[i] = FT_MulFix( face->cvt[i], size->ttmetrics.scale );
+        tt_size_run_prep( size, pedantic );
+      }
+
+#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       /* a change from mono to grayscale rendering (and vice versa) */
       /* requires a re-execution of the CVT program                 */
       if ( grayscale != exec->grayscale )
@@ -1887,6 +2114,8 @@
         tt_size_run_prep( size, pedantic );
       }
 
+#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       /* see whether the cvt program has disabled hinting */
       if ( exec->GS.instruct_control & 1 )
         load_flags |= FT_LOAD_NO_HINTING;
