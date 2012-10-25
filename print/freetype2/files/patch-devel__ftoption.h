--- devel/ftoption.h	2012-10-25 13:52:36.735525460 +0800
+++ devel/ftoption.h	2012-10-25 13:52:45.947524080 +0800
@@ -582,6 +582,17 @@
 
   /*************************************************************************/
   /*                                                                       */
+  /* Define FT_CONFIG_OPTION_INFINALITY_PATCHSET if you want to enable     */
+  /* all additional infinality patches, which are configured via env       */
+  /* variables.                                                            */
+  /*                                                                       */
+  /*   This option requires TT_CONFIG_OPTION_SUBPIXEL_HINTING to           */
+  /*   defined.                                                            */
+  /*                                                                       */
+#define FT_CONFIG_OPTION_INFINALITY_PATCHSET
+
+  /*************************************************************************/
+  /*                                                                       */
   /* If you define TT_CONFIG_OPTION_UNPATENTED_HINTING, a special version  */
   /* of the TrueType bytecode interpreter is used that doesn't implement   */
   /* any of the patented opcodes and algorithms.  The patents related to   */
