--- src/truetype/ttinterp.h	2012-09-05 21:25:10.147018974 +0800
+++ src/truetype/ttinterp.h	2012-09-05 21:25:27.941023533 +0800
@@ -68,7 +68,8 @@
   /* Rounding function */
   typedef FT_F26Dot6
   (*TT_Round_Func)( EXEC_OP_ FT_F26Dot6  distance,
-                             FT_F26Dot6  compensation );
+                             FT_F26Dot6  compensation,
+                             FT_Int      resolution );
 
   /* Point displacement along the freedom vector routine */
   typedef void
@@ -107,6 +108,43 @@
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
+  
+  typedef struct  Font_Class_
+  {
+    const char  name[32];
+    const char  family[7][32];
+  } Font_Class;    
+  
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+
   /*************************************************************************/
   /*                                                                       */
   /* The main structure for the interpreter which collects all necessary   */
@@ -218,6 +256,33 @@
 
     FT_Bool            grayscale;      /* are we hinting for grayscale? */
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    TT_Round_Func      func_round_sphn;   /* subpixel rounding function */
+
+    FT_Bool            grayscale_hinting; /* use grayscale hinting?        */
+    FT_Bool            subpixel_hinting;  /* use subpixel hinting?         */
+    FT_Bool            enhanced_mode;     /* enhanced rendering on? (used  */
+                                          /* to enable/disable grayscale or*/
+                                          /* subpixel hinting at run time) */
+    FT_Bool            native_hinting;    /* native hinting?               */
+
+    /* the following 3 are unimplemented but here for future reference */
+    FT_Bool            compatible_widths;     /* compatible widths?     */
+    FT_Bool            symmetrical_smoothing; /* symmetrical_smoothing? */
+    FT_Bool            bgr;                   /* bgr instead of rgb?    */
+
+    FT_Int             rasterizer_version; /* MS rasterizer version */
+
+    FT_Bool            iup_called;        /* IUP[x] been called for glyph? */
+    FT_Bool            iupy_called;       /* IUP[y] been called for glyph? */
+    FT_Bool            in_delta_function; /* inside an inline delta func?  */
+
+    FT_ULong           sph_tweak_flags;   /* flags to control hint tweaks */
+
+    FT_Int             num_delta_funcs;
+    FT_ULong           inline_delta_funcs[5];
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
   } TT_ExecContextRec;
 
 
