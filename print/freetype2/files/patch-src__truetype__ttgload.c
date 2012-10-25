--- src/truetype/ttgload.c	2012-10-25 13:49:35.513523625 +0800
+++ src/truetype/ttgload.c	2012-10-25 13:52:36.743525591 +0800
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
+    if ( loader->exec && loader->exec->ignore_x_mode )
+      sph_set_tweaks( loader, glyph_index );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     if ( !loader->linear_def )
     {
       loader->linear_def = 1;
@@ -813,6 +823,13 @@
       loader->pp4 = zone->cur[zone->n_points - 1];
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if  ( loader->exec->sph_tweak_flags & SPH_TWEAK_DEEMBOLDEN )
+      FT_Outline_EmboldenXY( &loader->gloader->current.outline, -24, 0 );
+
+    else if  ( loader->exec->sph_tweak_flags & SPH_TWEAK_EMBOLDEN )
+      FT_Outline_EmboldenXY( &loader->gloader->current.outline, 24, 0 );
+#endif
     return TT_Err_Ok;
   }
 
@@ -835,6 +852,13 @@
     FT_Outline*     outline;
     FT_Int          n_points;
 
+    TT_Face     face   = (TT_Face)loader->face;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_String*  family = face->root.family_name;
+    int         ppem   = loader->size->metrics.x_ppem;
+    FT_String*  style  = face->root.style_name;
+    float       x_scale_factor = 1.0;
+#endif
 
     outline  = &gloader->current.outline;
     n_points = outline->n_points;
@@ -889,6 +913,26 @@
                      loader->zone.n_points + 4 );
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* scale, but only if enabled and only if TT hinting is being used */
+    if ( IS_HINTED( loader->load_flags ) )
+      x_scale_factor = scale_test_tweak( face, family, ppem, style,
+                                         loader->glyph_index, X_SCALING_Rules,
+                                         X_SCALING_RULES_SIZE );
+    /* scale the glyph */
+    if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0                  ||
+         x_scale_factor != 1.0 )
+    {
+      FT_Vector*  vec     = outline->points;
+      FT_Vector*  limit   = outline->points + n_points;
+      FT_Fixed    x_scale = ((TT_Size)loader->size)->metrics.x_scale * x_scale_factor;
+      FT_Fixed    y_scale = ((TT_Size)loader->size)->metrics.y_scale;
+
+      /* compensate for any scaling by de/emboldening */
+      if ( x_scale_factor != 1.0 && ppem > 11 )
+        FT_Outline_EmboldenXY( outline,
+                (FT_Int) ( 16.0 * (float)ppem * ( 1.0 - x_scale_factor) ), 0 );
+#else
     /* scale the glyph */
     if ( ( loader->load_flags & FT_LOAD_NO_SCALE ) == 0 )
     {
@@ -896,7 +940,7 @@
       FT_Vector*  limit   = outline->points + n_points;
       FT_Fixed    x_scale = ((TT_Size)loader->size)->metrics.x_scale;
       FT_Fixed    y_scale = ((TT_Size)loader->size)->metrics.y_scale;
-
+#endif
 
       for ( ; vec < limit; vec++ )
       {
@@ -1648,12 +1692,26 @@
     {
       FT_Byte*  widthp;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      FT_Bool   ignore_x_mode;
+
+
+      ignore_x_mode = FT_BOOL( FT_LOAD_TARGET_MODE( loader->load_flags ) !=
+                          FT_RENDER_MODE_MONO );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
       widthp = tt_face_get_device_metrics( face,
                                            size->root.metrics.x_ppem,
                                            glyph_index );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( widthp &&
+           ( ( ignore_x_mode && loader->exec->compatible_widths ) ||
+              !ignore_x_mode                                      ||
+              SPH_OPTION_BITMAP_WIDTHS ) )
+#else
       if ( widthp )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
         glyph->metrics.horiAdvance = *widthp << 6;
     }
 
@@ -1848,6 +1906,15 @@
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
@@ -1865,11 +1932,88 @@
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
+      exec->ignore_x_mode      = subpixel_hinting || grayscale_hinting;
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
@@ -1887,6 +2031,8 @@
         tt_size_run_prep( size, pedantic );
       }
 
+#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       /* see whether the cvt program has disabled hinting */
       if ( exec->GS.instruct_control & 1 )
         load_flags |= FT_LOAD_NO_HINTING;
