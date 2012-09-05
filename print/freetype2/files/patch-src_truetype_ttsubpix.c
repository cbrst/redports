--- src/truetype/ttsubpix.c	1970-01-01 07:30:00.000000000 +0730
+++ src/truetype/ttsubpix.c	2012-09-05 22:25:23.305021348 +0800
@@ -0,0 +1,227 @@
+/***************************************************************************/
+/*                                                                         */
+/*  ttsubpix.c                                                             */
+/*                                                                         */
+/*    TrueType Subpixel Hinting.                                           */
+/*                                                                         */
+/*  Copyright 2010-2011 by                                                 */
+/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
+/*                                                                         */
+/*  This file is part of the FreeType project, and may only be used,       */
+/*  modified, and distributed under the terms of the FreeType project      */
+/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
+/*  this file you indicate that you have read the license and              */
+/*  understand and accept it fully.                                        */
+/*                                                                         */
+/***************************************************************************/
+
+#include <ft2build.h>
+#include FT_INTERNAL_DEBUG_H
+#include FT_INTERNAL_CALC_H
+#include FT_INTERNAL_STREAM_H
+#include FT_INTERNAL_SFNT_H
+#include FT_TRUETYPE_TAGS_H
+#include FT_OUTLINE_H
+
+#include "ttsubpix.h"
+
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+  FT_LOCAL_DEF( FT_Bool )
+  sph_test_tweak( TT_Face         face,
+                  FT_String*      family,
+                  int             ppem,
+                  FT_String*      style,
+                  FT_UInt         glyph_index,
+                  SPH_TweakRule*  rule,
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
+        return TRUE;
+      }
+    }
+    return FALSE;
+  }
+
+
+#define TWEAK_RULES( x )                                       \
+  if ( sph_test_tweak( face, family, ppem, style, glyph_index, \
+                       x##_Rules, x##_RULES_SIZE ) )           \
+    loader->exec->sph_tweak_flags |= SPH_TWEAK_##x;
+
+#define TWEAK_RULES_EXCEPTIONS( x )                                        \
+  if ( sph_test_tweak( face, family, ppem, style, glyph_index,             \
+                       x##_Rules_Exceptions, x##_RULES_EXCEPTIONS_SIZE ) ) \
+    loader->exec->sph_tweak_flags &= ~SPH_TWEAK_##x;
+
+
+  FT_LOCAL_DEF( void )
+  sph_set_tweaks( TT_Loader  loader,
+                  FT_UInt    glyph_index )
+  {
+    TT_Face     face   = (TT_Face)loader->face;
+    FT_String*  family = face->root.family_name;
+    int         ppem   = loader->size->metrics.x_ppem;
+    FT_String*  style  = face->root.style_name;
+
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    int checked_use_known_settings_on_selected_fonts_env = 0;
+    FT_Bool use_known_settings_on_selected_fonts = FALSE;
+
+    int          fitting_strength = 0;
+    FT_UInt      checked_fitting_strength = 0;
+
+    if ( checked_use_known_settings_on_selected_fonts_env == 0 )
+    {
+      char *use_known_settings_on_selected_fonts_env = getenv( "INFINALITY_FT_USE_KNOWN_SETTINGS_ON_SELECTED_FONTS" );
+      if ( use_known_settings_on_selected_fonts_env != NULL )
+      {
+        if ( strcasecmp(use_known_settings_on_selected_fonts_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(use_known_settings_on_selected_fonts_env, "true") == 0)
+            use_known_settings_on_selected_fonts = TRUE;
+          else if ( strcasecmp(use_known_settings_on_selected_fonts_env, "1") == 0)
+            use_known_settings_on_selected_fonts = TRUE;
+          else if ( strcasecmp(use_known_settings_on_selected_fonts_env, "on") == 0)
+            use_known_settings_on_selected_fonts = TRUE;
+          else if ( strcasecmp(use_known_settings_on_selected_fonts_env, "yes") == 0)
+            use_known_settings_on_selected_fonts = TRUE;
+        }
+      }
+      checked_use_known_settings_on_selected_fonts_env = 1;
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
+
+#endif /* FT_CONFIG_OPTION_INFINALITY_PATCHSET */
+
+    /* Don't apply rules if style isn't set */
+    if ( !face->root.style_name ) return;
+
+    /* loader->exec->sph_tweak_flags = 0; */
+
+    /* printf( "%s,%d,%s,%c=%d ", family, ppem, style, glyph_index, glyph_index ); */
+    TWEAK_RULES( PIXEL_HINTING );
+
+    if ( loader->exec->sph_tweak_flags & SPH_TWEAK_PIXEL_HINTING )
+    {
+      loader->exec->enhanced_mode = FALSE;
+      return;
+    }
+
+    TWEAK_RULES( NO_ALIGNRP_AFTER_IUP );
+    TWEAK_RULES( NO_CALL_AFTER_IUP );
+    TWEAK_RULES( DEEMBOLDEN );
+    TWEAK_RULES( EMBOLDEN );
+    TWEAK_RULES( SKIP_INLINE_DELTAS );
+
+    TWEAK_RULES( NORMAL_ROUND_MIRP );
+    TWEAK_RULES( NORMAL_ROUND_MDRP );
+    TWEAK_RULES( NORMAL_ROUND_MDAP );
+    TWEAK_RULES( NORMAL_ROUND_MIAP );
+
+    TWEAK_RULES( SKIP_IUP );
+
+    TWEAK_RULES( ALWAYS_SKIP_DELTAP );
+    TWEAK_RULES( ALWAYS_DO_DELTAP );
+    TWEAK_RULES( DELTAP_RTG );
+    TWEAK_RULES( DELTAP_SKIP_EXAGGERATED_VALUES );
+    TWEAK_RULES( DELTAP_RDTG );
+
+    TWEAK_RULES( ALLOW_DMOVEX_FREEV );
+    TWEAK_RULES( ALLOW_DMOVE_FREEV );
+    TWEAK_RULES_EXCEPTIONS( ALLOW_DMOVEX_FREEV );
+    TWEAK_RULES_EXCEPTIONS( ALLOW_DMOVE_FREEV );
+
+    TWEAK_RULES( RASTERIZER_35 );
+
+    TWEAK_RULES( ALLOW_MOVEZP2_FREEV );
+    TWEAK_RULES_EXCEPTIONS( ALLOW_MOVEZP2_FREEV );
+
+    TWEAK_RULES( DO_RS );
+
+    TWEAK_RULES( DO_SHPIX );
+
+    TWEAK_RULES( SKIP_NONPIXEL_INLINE_MOVES );
+    TWEAK_RULES_EXCEPTIONS( SKIP_NONPIXEL_INLINE_MOVES );
+
+    TWEAK_RULES( ROUND_NONPIXEL_INLINE_MOVES );
+    TWEAK_RULES_EXCEPTIONS( ROUND_NONPIXEL_INLINE_MOVES );
+
+    TWEAK_RULES( SHPIX_EITHER_TOUCHED );
+    TWEAK_RULES_EXCEPTIONS( SHPIX_EITHER_TOUCHED );
+
+
+    if ( loader->exec->sph_tweak_flags & SPH_TWEAK_RASTERIZER_35 )
+    {
+      if ( loader->exec->rasterizer_version != 35 )
+      {
+        loader->exec->rasterizer_version = 35;
+        /* must re-execute fpgm */
+        loader->exec->size->cvt_ready      = FALSE;
+        tt_size_ready_bytecode( loader->exec->size, FT_BOOL( loader->load_flags & FT_LOAD_PEDANTIC ) );
+      }
+    }
+    else
+    {
+      if ( loader->exec->rasterizer_version == 35 )
+      {
+        loader->exec->rasterizer_version = 37;
+        /* must re-execute fpgm */
+        loader->exec->size->cvt_ready      = FALSE;
+        tt_size_ready_bytecode( loader->exec->size, FT_BOOL( loader->load_flags & FT_LOAD_PEDANTIC ) );
+      }
+    }
+
+    if ( IS_HINTED( loader->load_flags ) )
+    {
+      TWEAK_RULES( TIMES_NEW_ROMAN_HACK );
+      TWEAK_RULES( COURIER_NEW_2_HACK );
+    }
+
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    /* These tweaks require the TT interpreter to be on for this glyph */
+    if ( IS_HINTED( loader->load_flags ) && use_known_settings_on_selected_fonts )
+    {
+      if (sph_test_tweak( face, family, ppem, style, glyph_index,
+           COMPATIBLE_WIDTHS_Rules, COMPATIBLE_WIDTHS_RULES_SIZE ) )
+        loader->exec->compatible_widths |= TRUE;
+    }
+#endif /* FT_CONFIG_OPTION_INFINALITY_PATCHSET */
+  }
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+
+/* END */
