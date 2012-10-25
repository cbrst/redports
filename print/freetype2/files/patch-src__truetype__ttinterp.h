--- src/truetype/ttinterp.h	2012-10-25 13:49:35.513523625 +0800
+++ src/truetype/ttinterp.h	2012-10-25 13:52:36.753525370 +0800
@@ -68,7 +68,8 @@
   /* Rounding function */
   typedef FT_F26Dot6
   (*TT_Round_Func)( EXEC_OP_ FT_F26Dot6  distance,
-                             FT_F26Dot6  compensation );
+                             FT_F26Dot6  compensation,
+                             FT_Int      resolution );
 
   /* Point displacement along the freedom vector routine */
   typedef void
@@ -107,6 +108,44 @@
   } TT_CallRec, *TT_CallStack;
 
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+  /*************************************************************************/
+  /*                                                                       */
+  /* This structure defines a rule used to tweak subpixel hinting for      */
+  /* various fonts.  "", 0, "", NULL value indicates to match any value.   */
+  /*                                                                       */
+
+  typedef struct  SPH_TweakRule_
+  {
+    const char  family[32];
+    const int   ppem;
+    const char  style[32];
+    const FT_ULong  glyph;
+
+  } SPH_TweakRule;
+
+
+  typedef struct  SPH_ScaleRule_
+  {
+    const char  family[32];
+    const int   ppem;
+    const char  style[32];
+    const FT_ULong  glyph;
+    const float scale;
+  } SPH_ScaleRule;
+
+#define MAX_CLASS_MEMBERS 100
+
+  typedef struct  Font_Class_
+  {
+    const char  name[32];
+    const char  member[MAX_CLASS_MEMBERS][32];
+  } Font_Class;
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+
   /*************************************************************************/
   /*                                                                       */
   /* The main structure for the interpreter which collects all necessary   */
@@ -218,6 +257,37 @@
 
     FT_Bool            grayscale;      /* are we hinting for grayscale? */
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    TT_Round_Func      func_round_sphn;   /* subpixel rounding function */
+
+    FT_Bool            grayscale_hinting; /* Using grayscale hinting?      */
+    FT_Bool            subpixel_hinting;  /* Using subpixel hinting?       */
+    FT_Bool            native_hinting;    /* Using native hinting?         */
+    FT_Bool            ignore_x_mode;     /* Standard rendering mode for   */
+                                          /* subpixel hinting. On if gray  */
+                                          /* or subpixel hinting is on )   */
+    FT_Bool            compatibility_mode;/* Additional exceptions to      */
+                                          /* native TT rules for legacy    */
+                                          /* fonts. Implies ignore_x_mode. */
+
+    /* The following 3 aren't fully implemented but here for MS rasterizer */
+    /*   compatibility. */
+    FT_Bool            compatible_widths;    /* compatible widths?     */
+    FT_Bool            symmetrical_smoothing;/* symmetrical_smoothing? */
+    FT_Bool            bgr;                  /* bgr instead of rgb?    */
+    FT_Bool            subpixel_positioned;  /* MS DW subpixel positioned  */
+
+    FT_Int             rasterizer_version;   /* MS rasterizer version  */
+
+    FT_Bool            iup_called;           /* IUP called for glyph?  */
+    FT_Bool            in_delta_function;    /* inside an inline delta func? */
+
+    FT_ULong           sph_tweak_flags;      /* flags to control hint tweaks */
+
+    FT_Int             num_delta_funcs;
+    FT_ULong           inline_delta_funcs[5];
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
   } TT_ExecContextRec;
 
 
