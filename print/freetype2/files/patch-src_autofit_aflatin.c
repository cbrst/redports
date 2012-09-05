--- src/autofit/aflatin.c	2012-09-05 21:25:10.216018946 +0800
+++ src/autofit/aflatin.c	2012-09-05 21:25:37.789024142 +0800
@@ -22,6 +22,7 @@
 
 #include "aflatin.h"
 #include "aferrors.h"
+#include "strings.h"
 
 
 #ifdef AF_CONFIG_OPTION_USE_WARPER
@@ -528,7 +529,30 @@
     FT_Pos        delta;
     AF_LatinAxis  axis;
     FT_UInt       nn;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+    int checked_adjust_heights_env = 0;
+    FT_Bool adjust_heights = FALSE;
 
+    if ( checked_adjust_heights_env == 0 )
+    {
+      char *adjust_heights_env = getenv( "INFINALITY_FT_AUTOHINT_INCREASE_GLYPH_HEIGHTS" );
+      if ( adjust_heights_env != NULL )
+      {
+        if ( strcasecmp(adjust_heights_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(adjust_heights_env, "true") == 0)
+            adjust_heights = TRUE;
+          else if ( strcasecmp(adjust_heights_env, "1") == 0)
+            adjust_heights = TRUE;
+          else if ( strcasecmp(adjust_heights_env, "on") == 0)
+            adjust_heights = TRUE;
+          else if ( strcasecmp(adjust_heights_env, "yes") == 0)
+            adjust_heights = TRUE;
+        }
+      }
+      checked_adjust_heights_env = 1;
+    }
+#endif
 
     if ( dim == AF_DIMENSION_HORZ )
     {
@@ -556,7 +580,7 @@
     {
       AF_LatinAxis  Axis = &metrics->axis[AF_DIMENSION_VERT];
       AF_LatinBlue  blue = NULL;
-
+      int threshold = 40;
 
       for ( nn = 0; nn < Axis->blue_count; nn++ )
       {
@@ -566,12 +590,16 @@
           break;
         }
       }
-
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+      if ( adjust_heights
+        && metrics->root.scaler.face->size->metrics.x_ppem < 15
+        && metrics->root.scaler.face->size->metrics.x_ppem > 5 )
+        threshold = 52;
+#endif
       if ( blue )
       {
         FT_Pos  scaled = FT_MulFix( blue->shoot.org, scaler->y_scale );
-        FT_Pos  fitted = ( scaled + 40 ) & ~63;
-
+        FT_Pos  fitted = ( scaled + threshold ) & ~63;
 
         if ( scaled != fitted )
         {
@@ -626,7 +654,6 @@
         AF_LatinBlue  blue = &axis->blues[nn];
         FT_Pos        dist;
 
-
         blue->ref.cur   = FT_MulFix( blue->ref.org, scale ) + delta;
         blue->ref.fit   = blue->ref.cur;
         blue->shoot.cur = FT_MulFix( blue->shoot.org, scale ) + delta;
@@ -635,7 +662,12 @@
 
         /* a blue zone is only active if it is less than 3/4 pixels tall */
         dist = FT_MulFix( blue->ref.org - blue->shoot.org, scale );
+
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+        /* Do always, in order to prevent fringes */ 
+#else        
         if ( dist <= 48 && dist >= -48 )
+#endif          
         {
 #if 0
           FT_Pos  delta1;
@@ -686,7 +718,12 @@
             delta2 = -delta2;
 
           blue->ref.fit   = FT_PIX_ROUND( blue->ref.cur );
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET           
+          /* Round to prevent fringes */
+          blue->shoot.fit = FT_PIX_ROUND( blue->ref.fit - delta2 );
+#else
           blue->shoot.fit = blue->ref.fit - delta2;
+#endif          
 
 #endif
 
@@ -1432,6 +1469,8 @@
               if ( dist < 0 )
                 dist = -dist;
 
+              /* round down to pixels */
+              /*dist = FT_MulFix( dist, scale ) & ~63;*/
               dist = FT_MulFix( dist, scale );
               if ( dist < best_dist )
               {
@@ -1594,20 +1633,95 @@
     FT_Pos           dist     = width;
     FT_Int           sign     = 0;
     FT_Int           vertical = ( dim == AF_DIMENSION_VERT );
-
-
-    if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) ||
-         axis->extra_light                       )
-      return width;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
+    FT_Int           infinality_dist = 0;
+    
+    
+
+    FT_UInt autohint_snap_stem_height = 0;
+    FT_UInt checked_autohint_snap_stem_height = 0;
+    
+    if ( checked_autohint_snap_stem_height == 0)
+    {
+      char *autohint_snap_stem_height_env = getenv( "INFINALITY_FT_AUTOHINT_SNAP_STEM_HEIGHT" );
+      if ( autohint_snap_stem_height_env != NULL )
+      {
+        sscanf ( autohint_snap_stem_height_env, "%u", &autohint_snap_stem_height );
+        if      (autohint_snap_stem_height > 100 ) autohint_snap_stem_height = 100;
+        else if (autohint_snap_stem_height < 0 ) autohint_snap_stem_height = 0;
+      }
+      checked_autohint_snap_stem_height = 1;
+    }    
+
+    if ( autohint_snap_stem_height == 0 )
+#endif      
+      if ( !AF_LATIN_HINTS_DO_STEM_ADJUST( hints ) ||
+            axis->extra_light                      )
+        return width;
 
     if ( dist < 0 )
     {
       dist = -width;
       sign = 1;
     }
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
+    /* Calculate snap value differently than standard freetype */
+    if ( /* stem_snap_light*/ autohint_snap_stem_height > 0 
+      && (
+           ( vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) )
+      ||   ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) ) )
+    {
+      infinality_dist = af_latin_snap_width( axis->widths, axis->width_count, dist );
+
+      if ( metrics->root.scaler.face->size->metrics.x_ppem > 9
+        && axis->width_count > 0
+        && abs ( axis->widths[0].cur - infinality_dist ) < 32
+        && axis->widths[0].cur > 52 )
+      {
+        if ( strstr(metrics->root.scaler.face->style_name, "Regular")
+          || strstr(metrics->root.scaler.face->style_name, "Book")
+          || strstr(metrics->root.scaler.face->style_name, "Medium")
+          || strcmp(metrics->root.scaler.face->style_name, "Italic") == 0
+          || strcmp(metrics->root.scaler.face->style_name, "Oblique") == 0 )
+        {
+          /* regular weight */
+          if ( axis->widths[0].cur < 64 ) infinality_dist = 64 ;
+          else if (axis->widths[0].cur  < 88) infinality_dist = 64;
+          else if (axis->widths[0].cur  < 160) infinality_dist = 128;
+          else if (axis->widths[0].cur  < 240) infinality_dist = 190;
+          else infinality_dist = ( infinality_dist ) & ~63;
+        }
+        else
+        {
+          /* bold gets a different threshold */
+          if ( axis->widths[0].cur < 64 ) infinality_dist = 64 ;
+          else if (axis->widths[0].cur  < 108) infinality_dist = 64;
+          else if (axis->widths[0].cur  < 160) infinality_dist = 128;
+          else if (axis->widths[0].cur  < 222) infinality_dist = 190;
+          else if (axis->widths[0].cur  < 288) infinality_dist = 254;
+          else infinality_dist = ( infinality_dist + 16 ) & ~63;
+        }
+
+      }
+      if (infinality_dist < 52)
+      {
+        if (metrics->root.scaler.face->size->metrics.x_ppem < 9 )
+        {
 
-    if ( (  vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
-         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) )
+          if (infinality_dist < 32) infinality_dist = 32;
+        }
+        else
+          infinality_dist = 64;
+      }
+    }
+    else if ( autohint_snap_stem_height < 100 
+      && (( vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
+         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) ) )
+#else
+      
+    if ( ( vertical && !AF_LATIN_HINTS_DO_VERT_SNAP( hints ) ) ||
+         ( !vertical && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) ) ) 
+#endif      
     {
       /* smooth hinting process: very lightly quantize the stem width */
 
@@ -1666,7 +1780,10 @@
           dist = ( dist + 32 ) & ~63;
       }
     }
-    else
+    else 
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET         
+      if ( autohint_snap_stem_height < 100 )
+#endif        
     {
       /* strong hinting process: snap the stem width to integer pixels */
 
@@ -1674,7 +1791,9 @@
 
 
       dist = af_latin_snap_width( axis->widths, axis->width_count, dist );
-
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+      if ( autohint_snap_stem_height > 0 ) goto Done_Width;
+#endif
       if ( vertical )
       {
         /* in the case of vertical hinting, always round */
@@ -1737,6 +1856,23 @@
     }
 
   Done_Width:
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET   
+    if (axis->widths[0].cur > 42 ) 
+      /* weighted average */
+      dist = (dist * (100 - autohint_snap_stem_height) + infinality_dist * autohint_snap_stem_height ) / 100;  
+
+      {
+        int factor = 100;
+        if (axis->standard_width < 100)
+          factor = axis->standard_width;
+
+        if (metrics->root.scaler.face->size->metrics.x_ppem >=9  && dist < 52 ) dist += ((52 - dist) * factor) / 100;
+        if (metrics->root.scaler.face->size->metrics.x_ppem <9  && dist < 32 ) dist += ((32 - dist) * factor) / 100;
+        
+        if (axis->standard_width > 100 && metrics->root.scaler.face->size->metrics.x_ppem >=11  && dist < 64 ) dist = 64; 
+        if (axis->standard_width > 100 && metrics->root.scaler.face->size->metrics.x_ppem >=9  && dist < 52 ) dist = 52;       
+      }
+#endif    
     if ( sign )
       dist = -dist;
 
@@ -1759,6 +1895,8 @@
                              (AF_Edge_Flags)base_edge->flags,
                              (AF_Edge_Flags)stem_edge->flags );
 
+/* if fitted_width causes stem_edge->pos to land basically on top of an existing
+ * stem_edge->pos, then add or remove 64.  Need to figure out a way to do this */
 
     stem_edge->pos = base_edge->pos + fitted_width;
 
@@ -2233,8 +2371,32 @@
   {
     FT_Error  error;
     int       dim;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET       
+    int       emboldening_strength = 0;
 
-
+    int checked_use_various_tweaks_env = 0;
+    FT_Bool use_various_tweaks = FALSE;
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
+#endif
     error = af_glyph_hints_reload( hints, outline );
     if ( error )
       goto Exit;
@@ -2291,7 +2453,54 @@
       }
     }
     af_glyph_hints_save( hints, outline );
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET    
+#if 0
+    if( metrics->root.scaler.face->style_name )
+    {
+      if ( strcasestr(metrics->root.scaler.face->style_name, "Bold")  
+        || strcasestr(metrics->root.scaler.face->style_name, "Black")          
+        || strcasestr(metrics->root.scaler.face->style_name, "Narrow") 
+               && metrics->root.scaler.face->size->metrics.x_ppem < 15  
+        || strcasestr(metrics->root.scaler.face->style_name, "Condensed") 
+               && metrics->root.scaler.face->size->metrics.x_ppem < 20 )         
+            goto Exit;
+    }
+    if( metrics->root.scaler.face->family_name )
+    {
+      if ( strcasestr(metrics->root.scaler.face->family_name, "Bold")  
+        || strcasestr(metrics->root.scaler.face->family_name, "Black")          
+        || strcasestr(metrics->root.scaler.face->family_name, "Narrow") 
+               && metrics->root.scaler.face->size->metrics.x_ppem < 15 
+        || strcasestr(metrics->root.scaler.face->family_name, "Condensed") 
+               && metrics->root.scaler.face->size->metrics.x_ppem < 20 )         
+            goto Exit;
+    }  
 
+    /* if the font is particularly thin, embolden it, up to 1 px */
+    if ( use_various_tweaks
+      && metrics->axis->widths[0].cur <= FT_MulDiv ( autohint_minimum_stem_width, 64, 100)
+      && !( dim == AF_DIMENSION_VERT )
+      && !AF_LATIN_HINTS_DO_HORZ_SNAP( hints ) )
+    {
+      if ( metrics->axis->widths[0].cur
+        / metrics->root.scaler.face->size->metrics.x_ppem < 5 )
+      {
+         emboldening_strength = FT_MulDiv ( autohint_minimum_stem_width, 64, 100) - metrics->axis->widths[0].cur;
+        if ( metrics->root.scaler.face->size->metrics.x_ppem < 9 )
+          emboldening_strength -= 10;
+        if ( metrics->root.scaler.face->size->metrics.x_ppem < 7 )
+          emboldening_strength -= 10;            
+      }
+      if ( emboldening_strength < 0 ) emboldening_strength = 0;
+      FT_Outline_Embolden(outline,emboldening_strength);
+    }
+#endif
+    /* Save this width for use in ftsmooth.c.  This is a shameful hack */
+    const char* c1 = "CUR_WIDTH";
+    char c2[8];
+    snprintf(c2,8,"%ld",metrics->axis->widths[0].cur);
+    setenv(c1, c2, 1);
+#endif
   Exit:
     return error;
   }
