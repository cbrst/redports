--- src/truetype/ttsubpix.h	1970-01-01 07:30:00.000000000 +0730
+++ src/truetype/ttsubpix.h	2012-09-05 22:25:23.307017773 +0800
@@ -0,0 +1,672 @@
+/***************************************************************************/
+/*                                                                         */
+/*  ttsubpix.h                                                             */
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
+#ifndef __TTSUBPIX_H__
+#define __TTSUBPIX_H__
+
+#include <ft2build.h>
+#include "ttobjs.h"
+#include "ttinterp.h"
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+  /*************************************************************************/
+  /*                                                                       */
+  /* Tweak flags that are set for each glyph                               */
+  /*                                                                       */
+  /*                                                                       */
+#define SPH_TWEAK_NORMAL_ROUND_MIRP                0x0000001
+#define SPH_TWEAK_NORMAL_ROUND_MDRP                0x0000002
+#define SPH_TWEAK_DELTAP_RDTG                      0x0000004
+#define SPH_TWEAK_DELTAP_RTG                       0x0000008
+#define SPH_TWEAK_DELTAP_SKIP_EXAGGERATED_VALUES   0x0000010
+#define SPH_TWEAK_ALLOW_DMOVEX_FREEV               0x0000020
+#define SPH_TWEAK_ALLOW_DMOVE_FREEV                0x0000040
+#define SPH_TWEAK_ALLOW_MOVEZP2_FREEV              0x0000080
+#define SPH_TWEAK_ALWAYS_SKIP_DELTAP               0x0000100
+#define SPH_TWEAK_SKIP_IUP                         0x0000200
+#define SPH_TWEAK_NORMAL_ROUND_MIAP                0x0000400
+#define SPH_TWEAK_NORMAL_ROUND_MDAP                0x0000800
+#define SPH_TWEAK_DO_RS                            0x0001000
+#define SPH_TWEAK_DO_SHPIX                         0x0002000
+#define SPH_TWEAK_ALWAYS_DO_DELTAP                 0x0004000
+#define SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES       0x0008000
+#define SPH_TWEAK_SHPIX_EITHER_TOUCHED             0x0010000
+#define SPH_TWEAK_RASTERIZER_35                    0x0020000
+#define SPH_TWEAK_PIXEL_HINTING                    0x0040000
+#define SPH_TWEAK_NO_ALIGNRP_AFTER_IUP             0x0080000
+#define SPH_TWEAK_DEEMBOLDEN                       0x0100000
+#define SPH_TWEAK_EMBOLDEN                         0x0200000
+#define SPH_TWEAK_ROUND_NONPIXEL_INLINE_MOVES      0x0400000
+#define SPH_TWEAK_NO_CALL_AFTER_IUP                0x0800000
+#define SPH_TWEAK_SKIP_INLINE_DELTAS               0x1000000
+#define SPH_TWEAK_TIMES_NEW_ROMAN_HACK             0x4000000
+#define SPH_TWEAK_COURIER_NEW_2_HACK               0x8000000
+
+
+  FT_LOCAL( FT_Bool )
+  sph_test_tweak( TT_Face         face,
+                  FT_String*      family,
+                  int             ppem,
+                  FT_String*      style,
+                  FT_UInt         glyph_index,
+                  SPH_TweakRule*  rule,
+                  FT_UInt         num_rules );
+
+  FT_LOCAL( void )
+  sph_set_tweaks( TT_Loader  loader,
+                  FT_UInt    glyph_index );
+
+
+  /*************************************************************************/
+  /*                                                                       */
+  /* These are rules that affect how the TT Interpreter does               */
+  /* hinting.                                                              */
+  /*                                                                       */
+  /* "" string or 0 int/char indicates to apply to all.                    */
+  /* "-" used as dummy placeholders, but any non-matching string works.    */
+  /*                                                                       */
+  /* Remaining rules are tweaks for various fonts / glyphs.                */
+  /* Some of this could arguably be implemented in fontconfig, however:    */
+  /*                                                                       */
+  /*  - Fontconfig can't set things on a glyph-by-glyph basis.             */
+  /*  - The tweaks that happen here are very low-level, from an average    */
+  /*    user's point of view and are best implemented in the hinter        */
+  /*                                                                       */
+  /* Ideally, some of these should be generalized across affected fonts,   */
+  /* and enabled by default in the code.  The rule structure is designed   */
+  /* so that entirely new rules can easily be added when a new             */
+  /* compatibility feature is discovered.                                  */
+  /*                                                                       */
+  /* The rule structures could also use some enhancement to handle ranges. */
+  /*                                                                       */
+  /*     ****************** WORK IN PROGRESS *******************           */
+  /*                                                                       */
+
+#define SPH_OPTION_BITMAP_WIDTHS           FALSE
+#define SPH_OPTION_SET_SUBPIXEL            TRUE
+#define SPH_OPTION_SET_GRAYSCALE           FALSE
+#define SPH_OPTION_SET_COMPATIBLE_WIDTHS   FALSE
+#define SPH_OPTION_SET_RASTERIZER_VERSION  37
+#define SPH_OPTION_GRIDS_PER_PIXEL_X       64
+#define SPH_OPTION_GRIDS_PER_PIXEL_Y       1
+
+  /* Don't do subpixel hinting; do normal hinting */
+#define PIXEL_HINTING_RULES_SIZE  8
+  SPH_TweakRule  PIXEL_HINTING_Rules
+                   [PIXEL_HINTING_RULES_SIZE] =
+                 {
+                   { "Courier New", 0, "", '<' },
+                   { "Courier New", 0, "", '>' },
+                   { "Tahoma", 0, "Bold", '<' },
+                   { "Tahoma", 0, "Bold", '>' },
+                   { "Trebuchet MS", 0, "Regular", '<' },
+                   { "Trebuchet MS", 0, "Regular", '>' },
+                   { "Verdana", 0, "", '<' },
+                   { "Verdana", 0, "", '>' },
+                 };
+
+
+  /* Don't avoid RS Rules (as the basic subpixel hinting does) */
+#define DO_RS_RULES_SIZE  1
+  SPH_TweakRule  DO_RS_Rules
+                   [DO_RS_RULES_SIZE] =
+                 {
+                   { "-", 0, "", 0 },
+                 };
+
+  /* Detection code for this is in place but not currently functional */
+#define SKIP_INLINE_DELTAS_RULES_SIZE  1
+  SPH_TweakRule  SKIP_INLINE_DELTAS_Rules
+                   [SKIP_INLINE_DELTAS_RULES_SIZE] =
+                 {
+                   { "-", 0, "", 0 },
+                 };
+
+  /* Subpixel hinting ignores SHPIX rules.  Enable SHPIX for these. */
+#define DO_SHPIX_RULES_SIZE  5
+  SPH_TweakRule  DO_SHPIX_Rules
+                   [DO_SHPIX_RULES_SIZE] =
+                 {
+                   { "Segoe UI", 0, "", 0 },
+                   { "Verdana", 10, "Regular", '4' },
+                   { "Verdana", 13, "Regular", 0 },
+                   { "Verdana", 18, "Regular", '4' },
+                   { "Verdana", 9, "Regular", '4' },
+                 };
+
+  /* Default is that SHPIX needs to match a touched point on x AND y */
+  /* This indicates that SHPIX needs to match a touched point on x OR y */
+#define SHPIX_EITHER_TOUCHED_RULES_SIZE  23
+  SPH_TweakRule  SHPIX_EITHER_TOUCHED_Rules
+                   [SHPIX_EITHER_TOUCHED_RULES_SIZE] =
+                 {
+                   { "", 0, "", 'e' },
+                   { "Arial", 0, "", 'c' },
+                   { "Arial", 0, "Bold", '0' },
+                   { "Arial", 0, "Bold", '1' },
+                   { "Arial", 0, "Bold", '2' },
+                   { "Arial", 0, "Bold", '3' },
+                   { "Arial", 0, "Bold", '4' },
+                   { "Arial", 0, "Bold", '5' },
+                   { "Arial", 0, "Bold", '6' },
+                   { "Arial", 0, "Bold", '7' },
+                   { "Arial", 0, "Bold", '8' },
+                   { "Arial", 0, "Bold", '9' },
+                   { "Arial", 0, "Regular", 0 },
+                   { "Consolas", 0, "", 0 },
+                   { "Courier New", 0, "", 0 },
+                   { "Georgia", 0, "", 'i' },
+                   { "Lucida Grande", 0, "Regular", 0 },
+                   { "Paltino Linotype", 0, "", 0 },
+                   { "Tahoma", 0, "Regular", 0 },
+                   { "Times New Roman", 0, "Bold", 'a' },
+                   { "Times New Roman", 16, "Regular", 'u' },
+                   { "Trebuchet MS", 0, "", 0 },
+                   { "Verdana", 0, "", 0 },
+                 };
+
+#define SHPIX_EITHER_TOUCHED_RULES_EXCEPTIONS_SIZE  5
+  SPH_TweakRule  SHPIX_EITHER_TOUCHED_Rules_Exceptions
+                   [SHPIX_EITHER_TOUCHED_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Arial", 11, "Regular", 's' },
+                   { "Arial", 0, "Regular", 'c' },
+                   { "Consolas", 12, "", 0 },
+                   { "Consolas", 13, "", 0 },
+                   { "Consolas", 14, "", 0 },
+                 };
+
+  /* Skip Y moves that don't move a point to a pixel boundary */
+  /* This fixes Tahoma, Trebuchet oddities and some issues with `$' */
+#define SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE  4
+  SPH_TweakRule  SKIP_NONPIXEL_INLINE_MOVES_Rules
+                   [SKIP_NONPIXEL_INLINE_MOVES_RULES_SIZE] =
+                 {
+                   { "", 0, "Regular", 0 },
+                   { "Courier New", 0, "Bold", 'w' },
+                   { "DejaVu Sans", 0, "Book", 'N' },
+                   { "Bitstream Vera Sans", 0, "Roman", 'N' },
+                 };
+
+#define SKIP_NONPIXEL_INLINE_MOVES_RULES_EXCEPTIONS_SIZE  9
+  SPH_TweakRule  SKIP_NONPIXEL_INLINE_MOVES_Rules_Exceptions
+                   [SKIP_NONPIXEL_INLINE_MOVES_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Droid Serif", 0, "", 0 },
+                   { "Droid Sans", 0, "Bold", 0 },
+                   { "Droid Sans Mono", 0, "Bold", 0 },
+                   { "Droid Sans", 0, "Bold Italic", 0 },
+                   { "Droid Sans Mono", 0, "Bold Italic", 0 },
+                   { "Courier New", 0, "Regular", '2' },
+                   { "Essential PragmataPro", 0, "", 0 },
+                   { "Geneva", 0, "", 0 },
+                   { "PragmataPro", 0, "", 0 },                   
+                 };
+
+  /* round moves that don't move a point to a pixel boundary */
+#define ROUND_NONPIXEL_INLINE_MOVES_RULES_SIZE  2
+  SPH_TweakRule  ROUND_NONPIXEL_INLINE_MOVES_Rules
+                   [ROUND_NONPIXEL_INLINE_MOVES_RULES_SIZE] =
+                 {
+                   { "Droid Sans", 0, "", 0 },
+                   { "Droid Sans Mono", 0, "", 0 },
+                 };
+
+#define ROUND_NONPIXEL_INLINE_MOVES_RULES_EXCEPTIONS_SIZE  3
+  SPH_TweakRule  ROUND_NONPIXEL_INLINE_MOVES_Rules_Exceptions
+                   [ROUND_NONPIXEL_INLINE_MOVES_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Droid Sans", 12, "Bold", 0 },
+                   { "Droid Sans", 13, "Bold", 0 },
+                   { "Droid Sans", 16, "Bold", 0 },
+                 };
+
+  /* Allow a Direct_Move_X along X freedom vector if matched */
+#define ALLOW_DMOVEX_FREEV_RULES_SIZE  7
+  SPH_TweakRule  ALLOW_DMOVEX_FREEV_Rules
+                   [ALLOW_DMOVEX_FREEV_RULES_SIZE] =
+                 {
+                   { "Arial Narrow", 0, "Regular", 0 },
+                   { "Arial", 13, "Regular", 0 },
+                   { "Arial", 14, "Regular", 0 },
+                   { "Verdana", 13, "Regular", 0 },
+                   { "Verdana", 9, "Regular", '4' },
+                   { "Verdana", 10, "Regular", '4' },
+                   { "Verdana", 18, "Regular", '4' },
+                 };
+
+  /* Allow a Direct_Move along X freedom vector if matched */
+#define ALLOW_DMOVE_FREEV_RULES_SIZE  7
+  SPH_TweakRule  ALLOW_DMOVE_FREEV_Rules
+                   [ALLOW_DMOVE_FREEV_RULES_SIZE] =
+                 {
+                   { "Arial Narrow", 0, "Regular", 0 },
+                   { "Arial", 13, "Regular", 0 },
+                   { "Arial", 14, "Regular", 0 },
+                   { "Verdana", 13, "Regular", 0 },
+                   { "Verdana", 9, "Regular", '4' },
+                   { "Verdana", 10, "Regular", '4' },
+                   { "Verdana", 18, "Regular", '4' },
+                 };
+
+#define ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE  3
+  SPH_TweakRule  ALLOW_DMOVEX_FREEV_Rules_Exceptions
+                   [ALLOW_DMOVEX_FREEV_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Arial", 0, "Regular", '4' },
+                   { "Arial", 0, "Regular", 's' },
+                   { "Arial", 0, "Regular", '^' },
+                 };
+
+#define ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE  3
+  SPH_TweakRule  ALLOW_DMOVE_FREEV_Rules_Exceptions
+                   [ALLOW_DMOVE_FREEV_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Arial", 0, "Regular", '4' },
+                   { "Arial", 0, "Regular", 's' },
+                   { "Arial", 0, "Regular", '^' },
+                 };
+
+
+  /* Allow a ZP2 move along freedom vector if matched; */
+  /* This is called from SHP, SHPIX, SHC, SHZ          */
+#define ALLOW_MOVEZP2_FREEV_RULES_SIZE  6
+  SPH_TweakRule  ALLOW_MOVEZP2_FREEV_Rules
+                   [ALLOW_MOVEZP2_FREEV_RULES_SIZE] =
+                 {
+                   { "Arial", 13, "Regular", 0 },
+                   { "Arial", 14, "Regular", 0 },
+                   { "Verdana", 13, "Regular", 0 },
+                   { "Verdana", 9, "Regular", '4' },
+                   { "Verdana", 10, "Regular", '4' },
+                   { "Verdana", 18, "Regular", '4' },
+                   /* this needs a bit of work though */
+                   /*{ "Microsoft Sans Serif", 0, "Regular", 0 },*/
+                 };
+
+#define ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE  8
+  SPH_TweakRule  ALLOW_MOVEZP2_FREEV_Rules_Exceptions
+                   [ALLOW_MOVEZP2_FREEV_RULES_EXCEPTIONS_SIZE] =
+                 {
+                   { "Times New Roman", 0, "Regular", 'a' },
+                   { "Verdana", 13, "Regular", 'N' },
+                   { "Verdana", 13, "Regular", 'f' },
+                   { "Verdana", 13, "Regular", 'v' },
+                   { "Verdana", 13, "Regular", 'w' },
+                   { "Verdana", 13, "Regular", 'x' },
+                   { "Verdana", 13, "Regular", 'y' },
+                   { "Verdana", 13, "Regular", 'z' },
+                 };
+
+  /* Return MS rasterizer version 35 if matched */
+#define RASTERIZER_35_RULES_SIZE 25
+  SPH_TweakRule  RASTERIZER_35_Rules
+                   [RASTERIZER_35_RULES_SIZE] =
+                 {
+                   { "Apple Chancery", 0, "", 0 },
+                   { "Century", 0, "", 0 },
+                   { "Chalkboard", 0, "", 0 },
+                   { "Frutiger Linotype", 0, "", 0 },
+                   { "Geneva CY", 0, "", 0 },
+                   { "Gill Sans", 0, "", 0 },
+                   { "Hard Gothic", 0, "", 0 },
+                   { "Herculanum", 0, "", 0 },
+                   { "Hoefler Text", 0, "", 0 },
+                   { "Lobster", 0, "", 0 },
+                   { "Marker Felt", 0, "", 0 },
+                   { "Neuton", 0, "", 0 },
+                   { "Pragmata", 0, "", 0 },
+                   { "Reenie Beanie", 0, "", 0 },
+                   { "Skia", 0, "", 0 },
+                   { "Sylfaen", 0, "", 0 },
+                   { "Tangerine", 0, "", 0 },
+                   { "Times New Roman", 0, "Regular", 'i' },
+                   { "Times New Roman", 0, "Regular", 'j' },
+                   { "Times New Roman", 0, "Regular", 'm' },
+                   { "Times New Roman", 0, "Regular", 'r' },
+                   { "Times", 0, "", 0 },
+                   { "Vollkorn", 0, "", 0 },
+                   { "Yanone Kaffeesatz", 0, "", 0 },
+                   { "Zapfino", 0, "", 0 },
+                 };
+
+  /* The below rounding functions only have effect on fonts that are  */
+  /* allowed to move in the x direction (see above);  It appears that */
+  /* all MS ClearType fonts may be OK using normal rounds like this.  */
+#define NORMAL_ROUND_MIRP_RULES_SIZE  4
+  SPH_TweakRule  NORMAL_ROUND_MIRP_Rules
+                   [NORMAL_ROUND_MIRP_RULES_SIZE] =
+                 {
+                   { "Calibri", 0, "Bold Italic", 0 },
+                   { "Calibri", 0, "Italic", 0 },
+                   { "Courier New", 0, "", 0 },
+                   { "Verdana", 10, "Regular", '4' },
+                 };
+
+#define NORMAL_ROUND_MIAP_RULES_SIZE  4
+  SPH_TweakRule  NORMAL_ROUND_MIAP_Rules
+                   [NORMAL_ROUND_MIAP_RULES_SIZE] =
+                 {
+                   { "Calibri", 0, "Bold Italic", 0 },
+                   { "Calibri", 0, "Italic", 0 },
+                   { "Courier New", 0, "", 0 },
+                   { "Verdana", 10, "Regular", '4' },
+                 };
+
+#define NORMAL_ROUND_MDRP_RULES_SIZE  4
+  SPH_TweakRule  NORMAL_ROUND_MDRP_Rules
+                   [NORMAL_ROUND_MDRP_RULES_SIZE] =
+                 {
+                   { "Calibri", 0, "Bold Italic", 0 },
+                   { "Calibri", 0, "Italic", 0 },
+                   { "Courier New", 0, "", 0 },
+                   { "Verdana", 10, "Regular", '4' },
+                 };
+
+#define NORMAL_ROUND_MDAP_RULES_SIZE  4
+  SPH_TweakRule  NORMAL_ROUND_MDAP_Rules
+                   [NORMAL_ROUND_MDAP_RULES_SIZE] =
+                 {
+                   { "Calibri", 0, "Bold Italic", 0 },
+                   { "Calibri", 0, "Italic", 0 },
+                   { "Courier New", 0, "", 0 },
+                   { "Verdana", 10, "Regular", '4' },
+                 };
+
+  /* Skip IUP instructions if matched */
+#define SKIP_IUP_RULES_SIZE  1
+  SPH_TweakRule  SKIP_IUP_Rules
+                   [SKIP_IUP_RULES_SIZE] =
+                 {
+                   { "Arial", 13, "Regular", 'a' },
+                 };
+
+  /* Skip DELTAP instructions if matched */
+#define ALWAYS_SKIP_DELTAP_RULES_SIZE  11
+  SPH_TweakRule  ALWAYS_SKIP_DELTAP_Rules
+                   [ALWAYS_SKIP_DELTAP_RULES_SIZE] =
+                 {
+                   { "Georgia", 0, "Regular", '7' },
+                   { "Georgia", 0, "Regular", 'A' },
+                   { "Georgia", 0, "Regular", 'N' },
+                   { "Georgia", 0, "Regular", 'V' },
+                   { "Georgia", 0, "Regular", 'W' },
+                   { "Georgia", 0, "Regular", 'X' },
+                   { "Georgia", 0, "Regular", 'Y' },
+                   { "Georgia", 0, "Regular", 'k' },
+                   { "Trebuchet MS", 0, "Italic", 0 },
+                   { "Trebuchet MS", 14, "Regular", 'e' },
+                   { "Verdana", 10, "Regular", 0 },
+                 };
+
+  /* Always do DELTAP instructions if matched */
+#define ALWAYS_DO_DELTAP_RULES_SIZE  8
+  SPH_TweakRule  ALWAYS_DO_DELTAP_Rules
+                   [ALWAYS_DO_DELTAP_RULES_SIZE] =
+                 {
+                   { "Bitstream Vera Sans", 14, "Roman", 'K' },
+                   { "Bitstream Vera Sans", 14, "Roman", 'k' },
+                   { "Bitstream Vera Sans", 17, "Roman", 'K' },
+                   { "Bitstream Vera Sans", 17, "Roman", 'k' },
+                   { "DejaVu Sans", 14, "Book", 'K' },
+                   { "DejaVu Sans", 14, "Book", 'k' },
+                   { "DejaVu Sans", 17, "Book", 'K' },
+                   { "DejaVu Sans", 17, "Book", 'k' },
+                 };
+
+  /* Do an extra RTG instruction in DELTAP if matched */
+#define DELTAP_RTG_RULES_SIZE  1
+  SPH_TweakRule  DELTAP_RTG_Rules
+                   [DELTAP_RTG_RULES_SIZE] =
+                 {
+                   { "-", 0, "", 0 },
+                 };
+
+  /* Do an extra RUTG instruction in DELTAP if matched */
+#define DELTAP_SKIP_EXAGGERATED_VALUES_RULES_SIZE  2
+  SPH_TweakRule  DELTAP_SKIP_EXAGGERATED_VALUES_Rules
+                   [DELTAP_SKIP_EXAGGERATED_VALUES_RULES_SIZE] =
+                 {
+                   { "Arial", 24, "Bold", 's' },
+                   { "Arial", 25, "Bold", 's' },
+                 };
+
+  /* Don't allow ALIGNRP after IUP */
+#define NO_ALIGNRP_AFTER_IUP_RULES_SIZE  4
+  SPH_TweakRule  NO_ALIGNRP_AFTER_IUP_Rules
+                   [NO_ALIGNRP_AFTER_IUP_RULES_SIZE] =
+                 {
+                   { "Courier New", 0, "Bold", 'C' },
+                   { "Courier New", 0, "Bold", 'D' },
+                   { "Courier New", 0, "Bold", 'Q' },
+                   { "Courier New", 0, "Bold", '0' },
+                 };
+
+  /* Don't allow CALL after IUP */
+#define NO_CALL_AFTER_IUP_RULES_SIZE  3
+  SPH_TweakRule  NO_CALL_AFTER_IUP_Rules
+                   [NO_CALL_AFTER_IUP_RULES_SIZE] =
+                 {
+                   { "Courier New", 0, "Bold", 'O' },
+                   { "Courier New", 0, "Bold", 'Q' },
+                   { "Courier New", 0, "Bold", 'k' },
+                 };
+
+  /* De-embolden these glyphs slightly */
+#define DEEMBOLDEN_RULES_SIZE  9
+  SPH_TweakRule  DEEMBOLDEN_Rules
+                   [DEEMBOLDEN_RULES_SIZE] =
+                 {
+                   { "Courier New", 0, "Bold", 'A' },
+                   { "Courier New", 0, "Bold", 'W' },
+                   { "Courier New", 0, "Bold", 'w' },
+                   { "Courier New", 0, "Bold", 'M' },
+                   { "Courier New", 0, "Bold", 'X' },
+                   { "Courier New", 0, "Bold", 'K' },
+                   { "Courier New", 0, "Bold", 'x' },
+                   { "Courier New", 0, "Bold", 'z' },
+                   { "Courier New", 0, "Bold", 'v' },
+                 };
+
+  /* Embolden these glyphs slightly */
+#define EMBOLDEN_RULES_SIZE  5
+  SPH_TweakRule  EMBOLDEN_Rules
+                   [EMBOLDEN_RULES_SIZE] =
+                 {
+                   { "Courier New", 12, "Italic", 'z' },
+                   { "Courier New", 11, "Italic", 'z' },
+                   { "Courier New", 10, "Italic", 'z' },
+                   { "Courier New", 0, "Regular", 0 },                    
+                   { "Courier New", 0, "Italic", 0 },                    
+                 };
+
+  /* Do an extra RDTG instruction in DELTAP if matched */
+#define DELTAP_RDTG_RULES_SIZE 24
+  SPH_TweakRule  DELTAP_RDTG_Rules
+                   [DELTAP_RDTG_RULES_SIZE] =
+                 {
+                   { "Calibri", 0, "Italic", 0 },
+                   { "Comic Sans MS", 0, "Regular", 0 },
+                   { "Courier New", 0, "Regular", '2' },
+                   { "Lucida Grande", 0, "Regular", 'e' },
+                   { "Lucida Grande", 12, "Bold", 0 },
+                   { "Microsoft Sans Serif", 0, "Regular", '7' },
+                   { "Microsoft Sans Serif", 0, "Regular", 'O' },
+                   { "Microsoft Sans Serif", 0, "Regular", 'Q' },
+                   { "Microsoft Sans Serif", 0, "Regular", 'X' },
+                   { "Microsoft Sans Serif", 0, "Regular", 'e' },
+                   { "Microsoft Sans Serif", 0, "Regular", 'o' },
+                   { "Tahoma", 0, "Bold", '0' },
+                   { "Tahoma", 16, "Bold Italic", 'C' },
+                   { "Tahoma", 16, "Bold", 'C' },
+                   { "Trebuchet MS", 0, "", '0' },
+                   { "Trebuchet MS", 14, "Regular", 'e' },
+                   { "Verdana", 0, "", '0' },
+                   { "Verdana", 0, "Bold Italic", '7' },
+                   { "Verdana", 0, "Bold Italic", 'v' },
+                   { "Verdana", 0, "Bold Italic", 'w' },
+                   { "Verdana", 0, "Bold", 0 },
+                   { "Verdana", 0, "Italic", 'o' },
+                   { "Verdana", 0, "Regular", 'x' },
+                   { "Verdana", 10, "Regular", 'w' },
+                 };
+
+#define TIMES_NEW_ROMAN_HACK_RULES_SIZE 12
+  SPH_TweakRule  TIMES_NEW_ROMAN_HACK_Rules
+                   [TIMES_NEW_ROMAN_HACK_RULES_SIZE] =
+                 {
+                   { "Times New Roman", 16, "Italic", '2' },
+                   { "Times New Roman", 16, "Italic", '5' },
+                   { "Times New Roman", 16, "Italic", '7' },
+                   { "Times New Roman", 16, "Regular", '2' },
+                   { "Times New Roman", 16, "Regular", '5' },
+                   { "Times New Roman", 16, "Regular", '7' },
+                   { "Times New Roman", 17, "Italic", '2' },
+                   { "Times New Roman", 17, "Italic", '5' },
+                   { "Times New Roman", 17, "Italic", '7' },
+                   { "Times New Roman", 17, "Regular", '2' },
+                   { "Times New Roman", 17, "Regular", '5' },
+                   { "Times New Roman", 17, "Regular", '7' },
+                 };
+
+#define COURIER_NEW_2_HACK_RULES_SIZE 15
+  SPH_TweakRule  COURIER_NEW_2_HACK_Rules
+                   [COURIER_NEW_2_HACK_RULES_SIZE] =
+                 {
+                   { "Courier New", 10, "Regular", '2' },
+                   { "Courier New", 11, "Regular", '2' },
+                   { "Courier New", 12, "Regular", '2' },
+                   { "Courier New", 13, "Regular", '2' },
+                   { "Courier New", 14, "Regular", '2' },
+                   { "Courier New", 15, "Regular", '2' },
+                   { "Courier New", 16, "Regular", '2' },
+                   { "Courier New", 17, "Regular", '2' },
+                   { "Courier New", 18, "Regular", '2' },
+                   { "Courier New", 19, "Regular", '2' },
+                   { "Courier New", 20, "Regular", '2' },
+                   { "Courier New", 21, "Regular", '2' },
+                   { "Courier New", 22, "Regular", '2' },
+                   { "Courier New", 23, "Regular", '2' },
+                   { "Courier New", 24, "Regular", '2' },
+                 };                 
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+  /* use compatible widths with these */
+#define COMPATIBLE_WIDTHS_RULES_SIZE 32
+  SPH_TweakRule  COMPATIBLE_WIDTHS_Rules
+                   [COMPATIBLE_WIDTHS_RULES_SIZE] =
+                 {
+                   { "Arial Unicode MS", 12, "Regular", 'm' },
+                   { "Arial Unicode MS", 14, "Regular", 'm' },
+                   { "Arial", 10, "Regular", L'ш' },
+                   { "Arial", 11, "Regular", 'm' },
+                   { "Arial", 12, "Regular", 'm' },
+                   { "Arial", 12, "Regular", L'ш' },
+                   { "Arial", 13, "Regular", L'ш' },
+                   { "Arial", 14, "Regular", 'm' },
+                   { "Arial", 14, "Regular", L'ш' },
+                   { "Arial", 15, "Regular", L'ш' },
+                   { "Arial", 17, "Regular", 'm' },
+                   { "DejaVu Sans", 12, "Book", 0 },
+                   { "DejaVu Sans", 15, "Book", 0 },
+                   { "Georgia", 13, "Regular", 0 },
+                   { "Microsoft Sans Serif", 11, "Regular", 0 },
+                   { "Microsoft Sans Serif", 12, "Regular", 0 },
+                   { "Segoe UI", 11, "Regular", 0 },
+                   { "Segoe UI", 12, "Regular", 'm' },
+                   { "Segoe UI", 14, "Regular", 'm' },
+                   { "Tahoma", 11, "Regular", 0 },
+                   { "Times New Roman", 16, "Regular", 'c' },
+                   { "Times New Roman", 16, "Regular", 'm' },
+                   { "Times New Roman", 16, "Regular", 'o' },
+                   { "Times New Roman", 16, "Regular", 'w' },
+                   { "Trebuchet MS", 12, "Regular", 0 },
+                   { "Trebuchet MS", 14, "Regular", 0 },
+                   { "Trebuchet MS", 15, "Regular", 0 },
+                   { "Verdana", 10, "Regular", L'ш' },
+                   { "Verdana", 11, "Regular", L'ш' },
+                   { "Verdana", 12, "Regular", 0 },
+                   { "Verdana", 13, "Regular", 'U' },
+                   { "Verdana", 14, "Regular", 'm' },
+                 };
+
+#define X_SCALING_RULES_SIZE 24
+  SPH_ScaleRule  X_SCALING_Rules
+                   [X_SCALING_RULES_SIZE] =
+                 {
+                   { "Arial", 13, "Regular", L'л', .95 },
+                   { "Arial", 14, "Regular", 'm', .95 },
+                   { "Arial", 15, "Regular", L'л', .925 },
+                   { "Bitstream Vera Sans", 10, "Roman", 0, 1.1 },
+                   { "Bitstream Vera Sans", 12, "Roman", 0, 1.05},
+                   { "Bitstream Vera Sans", 16, "Roman", 0, 1.05 },
+                   { "Bitstream Vera Sans", 9, "Roman", 0, 1.05},
+                   { "DejaVu Sans", 10, "Book", 0, 1.1 },
+                   { "DejaVu Sans", 12, "Book", 0, 1.05 },
+                   { "Georgia", 10, "", 0, 1.05},
+                   { "Georgia", 11, "", 0, 1.1},
+                   { "Georgia", 12, "", 0, 1.025},
+                   { "Georgia", 13, "", 0, 1.05},
+                   { "Georgia", 16, "", 0, 1.05 },
+                   { "Georgia", 17, "", 0, 1.03 },
+                   { "Liberation Sans", 12, "Regular", 'm', 1.1 },
+                   { "Lucida Grande", 11, "Regular", 'm', 1.1 },
+                   { "Segoe UI", 14, "Regular", 'm', 1.05},
+                   { "Verdana", 10, "Regular", 0, 1.1 },
+                   { "Verdana", 12, "Regular", 'W', 1.05},
+                   { "Verdana", 12, "Regular", 'a', 1.05},
+                   { "Verdana", 13, "Regular", 'U', .90 },
+                   { "Verdana", 16, "Regular", 0, 1.05 },
+                   { "Verdana", 9, "Regular", 0, 1.05},
+                 };
+
+#define Y_SCALING_RULES_SIZE 1
+  SPH_ScaleRule  Y_SCALING_Rules
+                   [Y_SCALING_RULES_SIZE] =
+                 {
+                   { "-", 0, "", 0, 1.2 },
+                 };
+
+
+#if 0
+
+#define FAMILY_CLASS_RULES_SIZE 2
+  Font_Class FAMILY_CLASS_Rules
+                   [FAMILY_CLASS_RULES_SIZE] =
+                 {
+                   { "Verdana Class", { "Verdana", "DejaVu Sans", "Bitstream Vera Sans", "", "", "", "", }, },
+                   { "Arial Class", { "Arial", "Liberation Sans", "Freesans", "Arimo", "", "", "", }, },
+                 };
+
+#define STYLE_CLASS_RULES_SIZE 1
+  Font_Class STYLE_CLASS_Rules
+                   [STYLE_CLASS_RULES_SIZE] =
+                 {
+                   { "Regular Class", { "Regular", "Book", "Medium", "", "", "", "", }, },
+                 };
+
+#endif
+
+#endif /* FT_CONFIG_OPTION_INFINALITY_PATCHSET */
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+
+/* END */
