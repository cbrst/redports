--- include/freetype/config/ftoption.h	2012-09-05 21:25:10.092018827 +0800
+++ include/freetype/config/ftoption.h	2012-09-05 21:25:37.776018932 +0800
@@ -92,7 +92,7 @@
   /* This is done to allow FreeType clients to run unmodified, forcing     */
   /* them to display normal gray-level anti-aliased glyphs.                */
   /*                                                                       */
-/* #define FT_CONFIG_OPTION_SUBPIXEL_RENDERING */
+#define FT_CONFIG_OPTION_SUBPIXEL_RENDERING
 
 
   /*************************************************************************/
@@ -473,6 +473,18 @@
 
 
   /*************************************************************************/
+  /*                                                                       */
+  /* Define FT_CONFIG_OPTION_INFINALITY_PATCHSET if you want to enable     */
+  /* all additional infinality patches, which are configured via env       */
+  /* variables.                                                            */
+  /*                                                                       */
+  /*   This option requires TT_CONFIG_OPTION_SUBPIXEL_HINTING to           */
+  /*   defined.                                                            */
+  /*                                                                       */
+#define FT_CONFIG_OPTION_INFINALITY_PATCHSET
+
+
+  /*************************************************************************/
   /*************************************************************************/
   /****                                                                 ****/
   /****        S F N T   D R I V E R    C O N F I G U R A T I O N       ****/
@@ -560,6 +572,28 @@
 
   /*************************************************************************/
   /*                                                                       */
+  /* Define TT_CONFIG_OPTION_SUBPIXEL_HINTING if you want to compile       */
+  /* EXPERIMENTAL subpixel hinting support into the TrueType driver.  This */
+  /* replaces the native TrueType hinting mechanism when anything but      */
+  /* FT_RENDER_MODE_MONO is requested.                                     */
+  /*                                                                       */
+  /* Enabling this causes the TrueType driver to ignore instructions under */
+  /* certain conditions.  This is done in accordance with the guide here,  */
+  /* with some minor differences:                                          */
+  /*                                                                       */
+  /*  http://www.microsoft.com/typography/cleartype/truetypecleartype.aspx */
+  /*                                                                       */
+  /* By undefining this, you only compile the code necessary to hint       */
+  /* TrueType glyphs with native TT hinting.                               */
+  /*                                                                       */
+  /*   This option requires TT_CONFIG_OPTION_BYTECODE_INTERPRETER to be    */
+  /*   defined.                                                            */
+  /*                                                                       */
+#define TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+
+  /*************************************************************************/
+  /*                                                                       */
   /* If you define TT_CONFIG_OPTION_UNPATENTED_HINTING, a special version  */
   /* of the TrueType bytecode interpreter is used that doesn't implement   */
   /* any of the patented opcodes and algorithms.  The patents related to   */
