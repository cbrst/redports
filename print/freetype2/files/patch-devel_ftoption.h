--- devel/ftoption.h	2012-09-05 21:25:10.277016184 +0800
+++ devel/ftoption.h	2012-09-05 21:25:32.889020480 +0800
@@ -560,6 +560,28 @@
 
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
