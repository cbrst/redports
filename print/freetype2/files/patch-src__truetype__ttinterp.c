--- src/truetype/ttinterp.c	2012-10-25 13:49:35.515524461 +0800
+++ src/truetype/ttinterp.c	2012-10-25 13:52:36.752525651 +0800
@@ -27,13 +27,16 @@
 #include FT_SYSTEM_H
 
 #include "ttinterp.h"
-
 #include "tterrors.h"
+#include "ttsubpix.h"
 
 
 #ifdef TT_USE_BYTECODE_INTERPRETER
 
 
+#define xxxSPH_DEBUG
+#define xxxSPH_DEBUG_MORE_VERBOSE
+
 #define TT_MULFIX           FT_MulFix
 #define TT_MULDIV           FT_MulDiv
 #define TT_MULDIV_NO_ROUND  FT_MulDiv_No_Round
@@ -153,11 +156,11 @@
 #define NORMalize( x, y, v ) \
           Normalize( EXEC_ARG_ x, y, v )
 
-#define SET_SuperRound( scale, flags ) \
-          SetSuperRound( EXEC_ARG_ scale, flags )
+#define SET_SuperRound( scale, flags, res ) \
+          SetSuperRound( EXEC_ARG_ scale, flags, res )
 
-#define ROUND_None( d, c ) \
-          Round_None( EXEC_ARG_ d, c )
+#define ROUND_None( d, c, e ) \
+          Round_None( EXEC_ARG_ d, c, e )
 
 #define INS_Goto_CodeRange( range, ip ) \
           Ins_Goto_CodeRange( EXEC_ARG_ range, ip )
@@ -168,8 +171,8 @@
 #define CUR_Func_move_orig( z, p, d ) \
           CUR.func_move_orig( EXEC_ARG_ z, p, d )
 
-#define CUR_Func_round( d, c ) \
-          CUR.func_round( EXEC_ARG_ d, c )
+#define CUR_Func_round( d, c, e ) \
+          CUR.func_round( EXEC_ARG_ d, c, e )
 
 #define CUR_Func_read_cvt( index ) \
           CUR.func_read_cvt( EXEC_ARG_ index )
@@ -1850,6 +1853,10 @@
 
     if ( v != 0 )
     {
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( !CUR.ignore_x_mode                                         ||
+          ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_X_DMOVE ) )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
       zone->cur[point].x += TT_MULDIV( distance,
                                        v * 0x10000L,
                                        CUR.F_dot_P );
@@ -1932,6 +1939,10 @@
   {
     FT_UNUSED_EXEC;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( !CUR.ignore_x_mode                                           ||
+         ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_X_DMOVEX ) )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     zone->cur[point].x += distance;
     zone->tags[point]  |= FT_CURVE_TAG_TOUCH_X;
   }
@@ -1994,6 +2005,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    The compensated distance.                                          */
   /*                                                                       */
@@ -2005,11 +2018,13 @@
   /*                                                                       */
   static FT_F26Dot6
   Round_None( EXEC_OP_ FT_F26Dot6  distance,
-                       FT_F26Dot6  compensation )
+                       FT_F26Dot6  compensation,
+                       FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
     FT_UNUSED_EXEC;
+    FT_UNUSED( resolution );
 
 
     if ( distance >= 0 )
@@ -2024,6 +2039,7 @@
       if ( val > 0 )
         val = 0;
     }
+
     return val;
   }
 
@@ -2041,12 +2057,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
   static FT_F26Dot6
   Round_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
-                          FT_F26Dot6  compensation )
+                          FT_F26Dot6  compensation,
+                          FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
@@ -2055,15 +2074,15 @@
 
     if ( distance >= 0 )
     {
-      val = distance + compensation + 32;
+      val = distance + compensation + 32 / resolution;
       if ( distance && val > 0 )
-        val &= ~63;
+        val &= ~( 64 / resolution - 1 );
       else
         val = 0;
     }
     else
     {
-      val = -FT_PIX_ROUND( compensation - distance );
+      val = -FT_PIX_ROUND_GRID( compensation - distance, resolution );
       if ( val > 0 )
         val = 0;
     }
@@ -2085,12 +2104,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
   static FT_F26Dot6
   Round_To_Half_Grid( EXEC_OP_ FT_F26Dot6  distance,
-                               FT_F26Dot6  compensation )
+                               FT_F26Dot6  compensation,
+                               FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
@@ -2099,13 +2121,15 @@
 
     if ( distance >= 0 )
     {
-      val = FT_PIX_FLOOR( distance + compensation ) + 32;
+      val = FT_PIX_FLOOR_GRID( distance + compensation, resolution ) +
+            32 / resolution;
       if ( distance && val < 0 )
         val = 0;
     }
     else
     {
-      val = -( FT_PIX_FLOOR( compensation - distance ) + 32 );
+      val = -( FT_PIX_FLOOR_GRID( compensation - distance, resolution ) +
+               32 / resolution );
       if ( val > 0 )
         val = 0;
     }
@@ -2127,12 +2151,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
   static FT_F26Dot6
   Round_Down_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
-                               FT_F26Dot6  compensation )
+                               FT_F26Dot6  compensation,
+                               FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
@@ -2143,13 +2170,13 @@
     {
       val = distance + compensation;
       if ( distance && val > 0 )
-        val &= ~63;
+        val &= ~( 64 / resolution - 1 );
       else
         val = 0;
     }
     else
     {
-      val = -( ( compensation - distance ) & -64 );
+      val = -( ( compensation - distance ) & -( 64 / resolution ) );
       if ( val > 0 )
         val = 0;
     }
@@ -2171,12 +2198,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
   static FT_F26Dot6
   Round_Up_To_Grid( EXEC_OP_ FT_F26Dot6  distance,
-                             FT_F26Dot6  compensation )
+                             FT_F26Dot6  compensation,
+                             FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
@@ -2185,15 +2215,15 @@
 
     if ( distance >= 0 )
     {
-      val = distance + compensation + 63;
+      val = distance + compensation + ( 64 / resolution - 1 );
       if ( distance && val > 0 )
-        val &= ~63;
+        val &= ~( 64 / resolution - 1 );
       else
         val = 0;
     }
     else
     {
-      val = - FT_PIX_CEIL( compensation - distance );
+      val = -FT_PIX_CEIL_GRID( compensation - distance, resolution );
       if ( val > 0 )
         val = 0;
     }
@@ -2215,12 +2245,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
   static FT_F26Dot6
   Round_To_Double_Grid( EXEC_OP_ FT_F26Dot6  distance,
-                                 FT_F26Dot6  compensation )
+                                 FT_F26Dot6  compensation,
+                                 FT_Int      resolution )
   {
     FT_F26Dot6 val;
 
@@ -2229,15 +2262,15 @@
 
     if ( distance >= 0 )
     {
-      val = distance + compensation + 16;
+      val = distance + compensation + 16 / resolution;
       if ( distance && val > 0 )
-        val &= ~31;
+        val &= ~( 32 / resolution - 1 );
       else
         val = 0;
     }
     else
     {
-      val = -FT_PAD_ROUND( compensation - distance, 32 );
+      val = -FT_PAD_ROUND( compensation - distance, 32 / resolution );
       if ( val > 0 )
         val = 0;
     }
@@ -2259,6 +2292,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
@@ -2270,10 +2305,13 @@
   /*                                                                       */
   static FT_F26Dot6
   Round_Super( EXEC_OP_ FT_F26Dot6  distance,
-                        FT_F26Dot6  compensation )
+                        FT_F26Dot6  compensation,
+                        FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
+    FT_UNUSED( resolution );
+
 
     if ( distance >= 0 )
     {
@@ -2309,6 +2347,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The number of grid lines per pixel.                */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
@@ -2318,10 +2358,13 @@
   /*                                                                       */
   static FT_F26Dot6
   Round_Super_45( EXEC_OP_ FT_F26Dot6  distance,
-                           FT_F26Dot6  compensation )
+                           FT_F26Dot6  compensation,
+                           FT_Int      resolution )
   {
     FT_F26Dot6  val;
 
+    FT_UNUSED( resolution );
+
 
     if ( distance >= 0 )
     {
@@ -2404,13 +2447,19 @@
   /*    Sets Super Round parameters.                                       */
   /*                                                                       */
   /* <Input>                                                               */
-  /*    GridPeriod :: Grid period                                          */
-  /*    selector   :: SROUND opcode                                        */
+  /*    GridPeriod :: The grid period.                                     */
+  /*                                                                       */
+  /*    selector   :: The SROUND opcode.                                   */
+  /*                                                                       */
+  /*    resolution :: The number of grid lines per pixel.                  */
   /*                                                                       */
   static void
   SetSuperRound( EXEC_OP_ FT_F26Dot6  GridPeriod,
-                          FT_Long     selector )
+                          FT_Long     selector,
+                          FT_Int      resolution )
   {
+    FT_UNUSED( resolution );
+
     switch ( (FT_Int)( selector & 0xC0 ) )
     {
       case 0:
@@ -3080,13 +3129,13 @@
 
 
 #define DO_SROUND                                \
-    SET_SuperRound( 0x4000, args[0] );           \
+    SET_SuperRound( 0x4000, args[0], 1 );        \
     CUR.GS.round_state = TT_Round_Super;         \
     CUR.func_round = (TT_Round_Func)Round_Super;
 
 
 #define DO_S45ROUND                                 \
-    SET_SuperRound( 0x2D41, args[0] );              \
+    SET_SuperRound( 0x2D41, args[0], 1 );           \
     CUR.GS.round_state = TT_Round_Super_45;         \
     CUR.func_round = (TT_Round_Func)Round_Super_45;
 
@@ -3257,12 +3306,12 @@
     args[0] = ( args[0] != args[1] );
 
 
-#define DO_ODD                                                  \
-    args[0] = ( ( CUR_Func_round( args[0], 0 ) & 127 ) == 64 );
+#define DO_ODD                                                     \
+    args[0] = ( ( CUR_Func_round( args[0], 0, 1 ) & 127 ) == 64 );
 
 
-#define DO_EVEN                                                \
-    args[0] = ( ( CUR_Func_round( args[0], 0 ) & 127 ) == 0 );
+#define DO_EVEN                                                   \
+    args[0] = ( ( CUR_Func_round( args[0], 0, 1 ) & 127 ) == 0 );
 
 
 #define DO_AND                        \
@@ -3311,6 +3360,34 @@
 #define DO_CEILING                    \
     args[0] = FT_PIX_CEIL( args[0] );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+#define DO_RS                                             \
+   {                                                      \
+     FT_ULong  I = (FT_ULong)args[0];                     \
+                                                          \
+                                                          \
+     if ( BOUNDSL( I, CUR.storeSize ) )                   \
+     {                                                    \
+       if ( CUR.pedantic_hinting )                        \
+         ARRAY_BOUND_ERROR;                               \
+       else                                               \
+         args[0] = 0;                                     \
+     }                                                    \
+     else                                                 \
+     {                                                    \
+       /* subpixel hinting - avoid Typeman Dstroke and */ \
+       /* IStroke and Vacuform rounds                  */ \
+                                                          \
+       if ( CUR.compatibility_mode                    &&  \
+            ( I == 24 || I == 22 || I == 8 ) )            \
+         args[0] = 0;                                     \
+       else                                               \
+         args[0] = CUR.storage[I];                        \
+     }                                                    \
+   }
+
+#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 #define DO_RS                           \
    {                                    \
@@ -3330,6 +3407,8 @@
        args[0] = CUR.storage[I];        \
    }
 
+#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
 #define DO_WS                           \
    {                                    \
@@ -3405,15 +3484,17 @@
     CUR.error = TT_Err_Debug_OpCode;
 
 
-#define DO_ROUND                                                   \
-    args[0] = CUR_Func_round(                                      \
-                args[0],                                           \
-                CUR.tt_metrics.compensations[CUR.opcode - 0x68] );
+#define DO_ROUND                                                 \
+    args[0] = CUR_Func_round(                                    \
+                args[0],                                         \
+                CUR.tt_metrics.compensations[CUR.opcode - 0x68], \
+                1 );
 
 
-#define DO_NROUND                                                            \
-    args[0] = ROUND_None( args[0],                                           \
-                          CUR.tt_metrics.compensations[CUR.opcode - 0x6C] );
+#define DO_NROUND                                                          \
+    args[0] = ROUND_None( args[0],                                         \
+                          CUR.tt_metrics.compensations[CUR.opcode - 0x6C], \
+                          1 );
 
 
 #define DO_MAX               \
@@ -4587,6 +4668,24 @@
     TT_DefRecord*  rec;
     TT_DefRecord*  limit;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+#if 0
+    int  opcode_pattern[4][12] = {
+           /* VacuFormRound function */
+           {0x45,0x23,0x46,0x60,0x20},
+           /* inline delta function 1 */
+           {0x4B,0x53,0x23,0x4B,0x51,0x5A,0x58,0x38,0x1B,0x21,0x21,0x59},
+           /* inline delta function 2 */
+           {0x4B,0x54,0x58,0x38,0x1B,0x5A,0x21,0x21,0x59},
+           /* diagonal stroke function */
+           {0x20,0x20,0x40,0x60,0x47,0x40,0x23,0x42},
+         };
+    int  opcode_patterns = 4;
+    int  i;
+    int  opcode_pointer[4] = {0,0,0,0};
+#endif
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* some font programs are broken enough to redefine functions! */
     /* We will then parse the current table.                       */
@@ -4620,10 +4719,11 @@
       return;
     }
 
-    rec->range  = CUR.curRange;
-    rec->opc    = (FT_UInt16)n;
-    rec->start  = CUR.IP + 1;
-    rec->active = TRUE;
+    rec->range        = CUR.curRange;
+    rec->opc          = (FT_UInt16)n;
+    rec->start        = CUR.IP + 1;
+    rec->active       = TRUE;
+    rec->inline_delta = FALSE;
 
     if ( n > CUR.maxFunc )
       CUR.maxFunc = (FT_UInt16)n;
@@ -4633,6 +4733,78 @@
 
     while ( SKIP_Code() == SUCCESS )
     {
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+#if 0
+#ifdef SPH_DEBUG_MORE_VERBOSE
+      printf ("Opcode: %d ", CUR.opcode);
+#endif
+
+      for ( i = 0; i < opcode_patterns; i++ )
+      {
+        if ( CUR.opcode == opcode_pattern[i][opcode_pointer[i]] )
+        {
+#ifdef SPH_DEBUG_MORE_VERBOSE
+          printf( "function %d, opcode ptrn: %d"
+                  "  op# %d: %d FOUND \n",
+                  n, i, opcode_pointer[i], CUR.opcode );
+#endif
+          opcode_pointer[i] += 1;
+
+          if ( i == 0 && opcode_pointer[0] == 5 )
+          {
+
+            CUR.inline_delta_funcs[CUR.num_delta_funcs] = n;
+            CUR.num_delta_funcs++;
+#ifdef SPH_DEBUG
+            printf( "Vacuform Round FUNCTION %d detected\n", n);
+#endif
+            /*rec->active = FALSE;*/
+            opcode_pointer[i] = 0;
+          }
+
+          if ( i == 1 && opcode_pointer[1] == 12 )
+          {
+            CUR.inline_delta_funcs[CUR.num_delta_funcs] = n;
+            CUR.num_delta_funcs++;
+#ifdef SPH_DEBUG
+            printf( "inline delta FUNCTION1 %d detected\n",
+                    n, CUR.num_delta_funcs);
+#endif
+            rec->inline_delta = TRUE;
+            opcode_pointer[i] = 0;
+          }
+
+          if ( i == 2 && opcode_pointer[1] == 9 )
+          {
+            CUR.inline_delta_funcs[CUR.num_delta_funcs] = n;
+            CUR.num_delta_funcs++;
+            rec->inline_delta = TRUE;
+#ifdef SPH_DEBUG
+            printf( "inline delta2 FUNCTION2 %d detected\n",
+                    n, CUR.num_delta_funcs);
+#endif
+            opcode_pointer[i] = 0;
+          }
+
+          if ( i == 4 && opcode_pointer[1] == 8 )
+          {
+            CUR.inline_delta_funcs[CUR.num_delta_funcs] = n;
+            CUR.num_delta_funcs++;
+            /*rec->active = FALSE;*/
+#ifdef SPH_DEBUG
+            printf( "diagonal stroke function %d detected\n",
+                    n, CUR.num_delta_funcs);
+#endif
+            opcode_pointer[i] = 0;
+          }
+        }
+
+        else
+          opcode_pointer[i] = 0;
+      }
+#endif
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       switch ( CUR.opcode )
       {
       case 0x89:    /* IDEF */
@@ -4676,6 +4848,15 @@
 
     CUR.step_ins = FALSE;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /*
+     *  CUR.ignore_x_mode may be turned off prior to function calls.  This
+     *  ensures it is turned back on.
+     */
+    CUR.ignore_x_mode = ( CUR.subpixel_hinting || CUR.grayscale_hinting )
+      && !( CUR.sph_tweak_flags & SPH_TWEAK_PIXEL_HINTING );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     if ( pRec->Cur_Count > 0 )
     {
       CUR.callTop++;
@@ -4709,6 +4890,10 @@
     TT_CallRec*    pCrec;
     TT_DefRecord*  def;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Bool        oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* first of all, check the index */
 
@@ -4746,6 +4931,20 @@
     if ( !def->active )
       goto Fail;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* This is test code used to detect inline delta functions */
+    oldF = def->inline_delta;
+    if ( CUR.ignore_x_mode )
+    {
+      if ( def->inline_delta )
+        CUR.in_delta_function = TRUE;
+    }
+
+#ifdef SPH_DEBUG
+      printf("Entering function %d\n", F);
+#endif
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* check the call stack */
     if ( CUR.callTop >= CUR.callSize )
     {
@@ -4767,6 +4966,13 @@
                         def->start );
 
     CUR.step_ins = FALSE;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    CUR.in_delta_function = oldF;
+
+#ifdef SPH_DEBUG
+      printf("Leaving function %d\n", F);
+#endif
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     return;
 
   Fail:
@@ -4787,6 +4993,10 @@
     TT_CallRec*    pCrec;
     TT_DefRecord*  def;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Bool        oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* first of all, check the index */
     F = args[1];
@@ -4823,6 +5033,15 @@
     if ( !def->active )
       goto Fail;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    oldF = def->inline_delta;
+    if ( CUR.ignore_x_mode )
+    {
+      if ( def->inline_delta )
+        CUR.in_delta_function = TRUE;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* check stack */
     if ( CUR.callTop >= CUR.callSize )
     {
@@ -4846,6 +5065,11 @@
 
       CUR.step_ins = FALSE;
     }
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    CUR.in_delta_function = oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     return;
 
   Fail:
@@ -5195,6 +5419,12 @@
       }
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* Disable Type 2 Vacuform Rounds - e.g. Arial Narrow */
+    if ( CUR.ignore_x_mode && FT_ABS( D ) == 64 )
+      D += 1;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     args[0] = D;
   }
 
@@ -5691,7 +5921,12 @@
 
     if ( CUR.GS.freeVector.x != 0 )
     {
-      CUR.zp2.cur[point].x += dx;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( !CUR.ignore_x_mode                                               ||
+          ( CUR.ignore_x_mode                                            &&
+            ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_X_MOVE_ZP2 ) ) )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+        CUR.zp2.cur[point].x += dx;
       if ( touch )
         CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_X;
     }
@@ -5872,6 +6107,9 @@
   {
     FT_F26Dot6  dx, dy;
     FT_UShort   point;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      B1, B2;
+#endif
 
 
     if ( CUR.top < CUR.GS.loop + 1 )
@@ -5917,7 +6155,77 @@
         }
       }
       else
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      {
+        /*  If not using ignore_x_mode rendering, allow ZP2 move.          */
+        /*  If inline deltas aren't allowed, skip ZP2 move.                */
+        /*  If using ignore_x_mode rendering, allow ZP2 point move if:     */
+        /*   - freedom vector is y and compatibility_mode is off           */
+        /*   - the glyph is composite and the move is in the Y direction   */
+        /*   - the glyph is specifically set to allow SHPIX moves          */
+        /*   - the move is on a previously Y-touched point                 */
+
+        if ( CUR.ignore_x_mode )
+        {
+          /* save point for later comparison */
+          if ( CUR.GS.freeVector.y != 0 )
+            B1 = CUR.zp2.cur[point].y;
+          else
+            B1 = CUR.zp2.cur[point].x;
+
+          if ( CUR.GS.freeVector.y != 0                                    &&
+               ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_INLINE_DELTAS ) )
+            goto Skip;
+
+          if ( CUR.ignore_x_mode                                           &&
+               !CUR.compatibility_mode && CUR.GS.freeVector.y != 0 )
+            MOVE_Zp2_Point( point, dx, dy, TRUE );
+
+          else if ( CUR.ignore_x_mode && CUR.compatibility_mode )
+          {
+            if ( CUR.ignore_x_mode                                         &&
+                ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES ) )
+            {
+              dx = FT_PIX_ROUND ( B1 + dx ) - B1;
+              dy = FT_PIX_ROUND ( B1 + dy ) - B1;
+            }
+
+            if ( !( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_SKIP_DELTAP )   &&
+                  ( ( CUR.is_composite && CUR.GS.freeVector.y != 0 )     ||
+                    ( CUR.zp2.tags[point] & FT_CURVE_TAG_TOUCH_Y )       ||
+                    ( CUR.sph_tweak_flags & SPH_TWEAK_DO_SHPIX ) )
+               )
+              MOVE_Zp2_Point( point, dx, dy, TRUE );
+          }
+
+          /* save new point */
+          if ( CUR.GS.freeVector.y != 0 )
+            B2 = CUR.zp2.cur[point].y;
+          else B2 = CUR.zp2.cur[point].x;
+
+          /* reverse any disallowed moves */
+          if ( ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES ) &&
+                 CUR.GS.freeVector.y != 0                                  &&
+                 B1 % 64 != 0                                              &&
+                 B2 % 64 != 0 && B1 != B2 )                                  ||
+               ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_OFFPIXEL_Y_MOVES ) &&
+                 CUR.GS.freeVector.y != 0                                  &&
+                 B1 % 64 == 0                                              &&
+                 B2 % 64 != 0 && B1 != B2 ) )
+          {
+#ifdef SPH_DEBUG
+            printf( "Reversing ZP2 move\n" );
+#endif
+            MOVE_Zp2_Point( point, -dx, -dy, TRUE );
+          }
+      }
+        else
+          MOVE_Zp2_Point( point, dx, dy, TRUE );
+      }
+  Skip:
+#else
         MOVE_Zp2_Point( point, dx, dy, TRUE );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
       CUR.GS.loop--;
     }
@@ -5939,7 +6247,18 @@
   {
     FT_UShort   point;
     FT_F26Dot6  distance;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      gridlines_per_pixel = 1;
+
 
+    if ( CUR.ignore_x_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0 )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_X;
+      else if ( CUR.GS.freeVector.y != 0 )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     point = (FT_UShort)args[0];
 
@@ -5963,6 +6282,15 @@
     distance = CUR_Func_project( CUR.zp1.cur + point,
                                  CUR.zp0.cur + CUR.GS.rp0 );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* subpixel hinting - make MSIRP respect CVT cut-in; */
+    if ( CUR.ignore_x_mode                              &&
+         CUR.GS.freeVector.x != 0                       &&
+         FT_ABS( distance - args[1] ) >=
+           CUR.GS.control_value_cutin / gridlines_per_pixel )
+      distance = args[1];
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     CUR_Func_move( &CUR.zp1, point, args[1] - distance );
 
     CUR.GS.rp1 = CUR.GS.rp0;
@@ -5985,7 +6313,21 @@
     FT_UShort   point;
     FT_F26Dot6  cur_dist,
                 distance;
+    FT_Int      gridlines_per_pixel = 1;
+
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                                    &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_X;
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     point = (FT_UShort)args[0];
 
@@ -6000,7 +6342,8 @@
     {
       cur_dist = CUR_fast_project( &CUR.zp0.cur[point] );
       distance = CUR_Func_round( cur_dist,
-                                 CUR.tt_metrics.compensations[0] ) - cur_dist;
+                                 CUR.tt_metrics.compensations[0],
+                                 gridlines_per_pixel ) - cur_dist;
     }
     else
       distance = 0;
@@ -6025,8 +6368,22 @@
     FT_UShort   point;
     FT_F26Dot6  distance,
                 org_dist;
+    FT_Int      gridlines_per_pixel = 1;
 
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                                    &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_X;
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     cvtEntry = (FT_ULong)args[1];
     point    = (FT_UShort)args[0];
 
@@ -6062,21 +6419,34 @@
 
     if ( CUR.GS.gep0 == 0 )   /* If in twilight zone */
     {
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      /* only adjust legacy fonts x otherwise breaks Calibri italic */
+      if ( CUR.compatibility_mode )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
       CUR.zp0.org[point].x = TT_MulFix14( (FT_UInt32)distance,
                                           CUR.GS.freeVector.x );
       CUR.zp0.org[point].y = TT_MulFix14( (FT_UInt32)distance,
                                           CUR.GS.freeVector.y ),
       CUR.zp0.cur[point]   = CUR.zp0.org[point];
     }
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( distance > 0                                                &&
+           ( CUR.sph_tweak_flags & SPH_TWEAK_MIAP_HACK )               &&
+           CUR.GS.freeVector.y != 0 )
+        distance = 0 ;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     org_dist = CUR_fast_project( &CUR.zp0.cur[point] );
 
     if ( ( CUR.opcode & 1 ) != 0 )   /* rounding and control cutin flag */
     {
-      if ( FT_ABS( distance - org_dist ) > CUR.GS.control_value_cutin )
+      if ( FT_ABS( distance - org_dist ) >
+             CUR.GS.control_value_cutin / gridlines_per_pixel )
         distance = org_dist;
 
-      distance = CUR_Func_round( distance, CUR.tt_metrics.compensations[0] );
+      distance = CUR_Func_round( distance,
+                                 CUR.tt_metrics.compensations[0],
+                                 gridlines_per_pixel );
     }
 
     CUR_Func_move( &CUR.zp0, point, distance - org_dist );
@@ -6098,6 +6468,24 @@
   {
     FT_UShort   point;
     FT_F26Dot6  org_dist, distance;
+    FT_Int      minimum_distance_factor = 64;
+    FT_Int      gridlines_per_pixel = 1;
+
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                                    &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+      {
+        gridlines_per_pixel              = SPH_OPTION_GRIDLINES_PER_PIXEL_X;
+        minimum_distance_factor = 64 - gridlines_per_pixel / 3;
+      }
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 
     point = (FT_UShort)args[0];
@@ -6163,11 +6551,13 @@
     if ( ( CUR.opcode & 4 ) != 0 )
       distance = CUR_Func_round(
                    org_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   gridlines_per_pixel );
     else
       distance = ROUND_None(
                    org_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   gridlines_per_pixel );
 
     /* minimum distance flag */
 
@@ -6175,13 +6565,17 @@
     {
       if ( org_dist >= 0 )
       {
-        if ( distance < CUR.GS.minimum_distance )
-          distance = CUR.GS.minimum_distance;
+        if ( distance < FT_MulDiv( minimum_distance_factor,
+                                   CUR.GS.minimum_distance, 64 ) )
+          distance = FT_MulDiv( minimum_distance_factor,
+                                CUR.GS.minimum_distance, 64 );
       }
       else
       {
-        if ( distance > -CUR.GS.minimum_distance )
-          distance = -CUR.GS.minimum_distance;
+        if ( distance > -FT_MulDiv( minimum_distance_factor,
+                                    CUR.GS.minimum_distance, 64 ) )
+          distance = -FT_MulDiv( minimum_distance_factor,
+                                 CUR.GS.minimum_distance, 64 );
       }
     }
 
@@ -6218,10 +6612,37 @@
                 cur_dist,
                 org_dist;
 
+    FT_Int      minimum_distance_factor = 64;
+    FT_Int      gridlines_per_pixel = 1;
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      B1;
+    FT_Int      B2;
+    FT_Bool     reverse_move = FALSE;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     point    = (FT_UShort)args[0];
     cvtEntry = (FT_ULong)( args[1] + 1 );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                                    &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+      {
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_X;
+        /* high value emboldens glyphs at lower ppems (< 14); */
+        /* Courier looks better with 52 --                    */
+        /* MS ClearType Rasterizer supposedly uses 32         */
+        minimum_distance_factor = 64 - gridlines_per_pixel / 3;
+      }
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND ) )
+        gridlines_per_pixel = SPH_OPTION_GRIDLINES_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* XXX: UNDOCUMENTED! cvt[-1] = 0 always */
 
     if ( BOUNDS( point,      CUR.zp1.n_points ) ||
@@ -6237,6 +6658,10 @@
       cvt_dist = 0;
     else
       cvt_dist = CUR_Func_read_cvt( cvtEntry - 1 );
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+      if ( CUR.sph_tweak_flags & SPH_TWEAK_MIRP_CVT_ZERO )
+        cvt_dist = 0;
+#endif
 
     /* single width test */
 
@@ -6274,8 +6699,15 @@
       if ( ( org_dist ^ cvt_dist ) < 0 )
         cvt_dist = -cvt_dist;
     }
-
-    /* control value cutin and round */
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.GS.freeVector.y != 0                                        &&
+                ( CUR.sph_tweak_flags & SPH_TWEAK_TIMES_NEW_ROMAN_HACK ) )
+    {
+      if ( cur_dist < -64 ) cvt_dist -= 16;
+      else if ( cur_dist > 64  && cur_dist < 84) cvt_dist += 32;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+    /* control value cut-in and round */
 
     if ( ( CUR.opcode & 4 ) != 0 )
     {
@@ -6296,18 +6728,21 @@
         /*      `ttinst2.doc', version 1.66, is thus incorrect since  */
         /*      it implies `>=' instead of `>'.                       */
 
-        if ( FT_ABS( cvt_dist - org_dist ) > CUR.GS.control_value_cutin )
+        if ( FT_ABS( cvt_dist - org_dist ) >
+               CUR.GS.control_value_cutin / gridlines_per_pixel )
           cvt_dist = org_dist;
       }
 
       distance = CUR_Func_round(
                    cvt_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   gridlines_per_pixel );
     }
     else
       distance = ROUND_None(
                    cvt_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   gridlines_per_pixel );
 
     /* minimum distance test */
 
@@ -6315,18 +6750,63 @@
     {
       if ( org_dist >= 0 )
       {
-        if ( distance < CUR.GS.minimum_distance )
-          distance = CUR.GS.minimum_distance;
+        if ( distance < FT_MulDiv( minimum_distance_factor,
+                                   CUR.GS.minimum_distance, 64 ) )
+          distance = FT_MulDiv( minimum_distance_factor,
+                                CUR.GS.minimum_distance, 64 );
       }
       else
       {
-        if ( distance > -CUR.GS.minimum_distance )
-          distance = -CUR.GS.minimum_distance;
+        if ( distance > -FT_MulDiv( minimum_distance_factor,
+                                    CUR.GS.minimum_distance, 64 ) )
+          distance = -FT_MulDiv( minimum_distance_factor,
+                                 CUR.GS.minimum_distance, 64 );
       }
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    B1 = CUR.zp1.cur[point].y;
+
+    /* Round moves if necessary */
+    if ( CUR.ignore_x_mode                                               &&
+         CUR.GS.freeVector.y != 0                                        &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES ) )
+      distance = FT_PIX_ROUND( B1 + distance - cur_dist ) - B1 + cur_dist;
+
+    if ( CUR.GS.freeVector.y != 0      &&
+         ( CUR.opcode & 16 ) == 0      &&
+         ( CUR.opcode & 8 ) == 0       &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_COURIER_NEW_2_HACK ) )
+      distance +=64;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     CUR_Func_move( &CUR.zp1, point, distance - cur_dist );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    B2 = CUR.zp1.cur[point].y;
+
+    /* Reverse move if necessary */
+    if ( CUR.ignore_x_mode )
+    {
+      if ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_OFFPIXEL_Y_MOVES )     &&
+           CUR.GS.freeVector.y != 0 && B1 % 64 == 0 && B2 % 64 != 0 )
+        reverse_move = TRUE;
+
+      if ( ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES )     &&
+           CUR.GS.freeVector.y != 0 && B2 % 64 != 0 && B1 % 64 != 0 )
+        reverse_move = TRUE;
+
+      if ( ( CUR.sph_tweak_flags & SPH_TWEAK_DELTAP_SKIP_EXAGGERATED_VALUES ) &&
+           !reverse_move                                                 &&
+           abs ( B1 - B2 ) >= 64 )
+        reverse_move = TRUE;
+    }
+
+    if ( reverse_move )
+      CUR_Func_move( &CUR.zp1, point, -( distance - cur_dist ) );
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
   Fail:
     CUR.GS.rp1 = CUR.GS.rp0;
 
@@ -6350,8 +6830,14 @@
     FT_F26Dot6  distance;
 
     FT_UNUSED_ARG;
-
-
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode && CUR.iup_called                                &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_NO_ALIGNRP_AFTER_IUP ) )
+    {
+      CUR.error = TT_Err_Invalid_Reference;
+      goto Fail;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     if ( CUR.top < CUR.GS.loop ||
          BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
     {
@@ -6846,6 +7332,15 @@
     contour = 0;
     point   = 0;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      CUR.iup_called = 1;
+      if ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_IUP )
+        return;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     do
     {
       end_point   = CUR.pts.contours[contour] - CUR.pts.first_point;
@@ -6915,7 +7410,9 @@
     FT_UShort  A;
     FT_ULong   C;
     FT_Long    B;
-
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_UShort  B1, B2;
+#endif
 
 #ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
     /* Delta hinting is covered by US Patent 5159668. */
@@ -6988,7 +7485,67 @@
             B++;
           B = B * 64 / ( 1L << CUR.GS.delta_shift );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+          /*
+           *  Allow delta move if
+           *
+           *  - not using ignore_x_mode rendering
+           *  - glyph is specifically set to allow it
+           *  - glyph is composite and freedom vector is not subpixel vector
+           */
+          if ( !CUR.ignore_x_mode                                        ||
+              ( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_DO_DELTAP )       ||
+              ( CUR.is_composite && CUR.GS.freeVector.y != 0 ))
+            CUR_Func_move( &CUR.zp0, A, B );
+
+          /* Otherwise apply subpixel hinting and compatibility mode rules */
+          else if ( CUR.ignore_x_mode )
+          {
+            if ( CUR.GS.freeVector.y != 0 )
+              B1 = CUR.zp0.cur[A].y;
+            else
+              B1 = CUR.zp0.cur[A].x;
+
+            /* Standard Subpixel Hinting:  Allow y move */
+            if ( !CUR.compatibility_mode && CUR.GS.freeVector.y != 0 )
+              CUR_Func_move( &CUR.zp0, A, B );
+
+            /* Compatibility Mode: Allow x or y move if point touched in
+                Y direction */
+            else if ( CUR.compatibility_mode                             &&
+                      !( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_SKIP_DELTAP ))
+            {
+              /* save the y value of the point now; compare after move */
+              B1 = CUR.zp0.cur[A].y;
+
+              if ( ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_Y_MOVES ) )
+                B = FT_PIX_ROUND( B1 + B ) - B1;
+
+              /*
+              *  Allow delta move if using compatibility_mode, IUP has not
+              *  been called, and point is touched on Y.
+              */
+              if ( !CUR.iup_called                                       &&
+                  ( CUR.zp0.tags[A] & FT_CURVE_TAG_TOUCH_Y ) )
+                CUR_Func_move( &CUR.zp0, A, B );
+            }
+            B2 = CUR.zp0.cur[A].y;
+
+            /* Reverse this move if it results in a disallowed move */
+            if ( CUR.GS.freeVector.y != 0                                &&
+                 ( ( ( CUR.sph_tweak_flags &
+                        SPH_TWEAK_SKIP_OFFPIXEL_Y_MOVES )            &&
+                        B1 % 64 == 0                                 &&
+                        B2 % 64 != 0 )                                 ||
+                   ( ( CUR.sph_tweak_flags &
+                        SPH_TWEAK_SKIP_NONPIXEL_Y_MOVES )            &&
+                        B1 % 64 != 0                                 &&
+                        B2 % 64 != 0 )))
+              CUR_Func_move( &CUR.zp0, A, -B );
+          }
+#else
           CUR_Func_move( &CUR.zp0, A, B );
+#endif /* *TT_CONFIG_OPTION_SUBPIXEL_HINTING */
         }
       }
       else
@@ -7114,26 +7671,116 @@
   Ins_GETINFO( INS_ARG )
   {
     FT_Long  K;
-
-
     K = 0;
 
-    /* We return MS rasterizer version 1.7 for the font scaler. */
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /********************************/
+    /* RASTERIZER VERSION           */
+    /* Selector Bit:  0             */
+    /* Return Bit(s): 0-7           */
+    /*                              */
+    if ( ( args[0] & 1 ) != 0 && CUR.ignore_x_mode )
+    {
+      K = CUR.rasterizer_version;
+#ifdef SPH_DEBUG_MORE_VERBOSE
+      printf(" SETTING AS %d\n", CUR.rasterizer_version );
+#endif
+    }
+    else
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     if ( ( args[0] & 1 ) != 0 )
       K = 35;
 
-    /* Has the glyph been rotated? */
+    /********************************/
+    /* GLYPH ROTATED                */
+    /* Selector Bit:  1             */
+    /* Return Bit(s): 8             */
+    /*                              */
     if ( ( args[0] & 2 ) != 0 && CUR.tt_metrics.rotated )
       K |= 0x80;
 
-    /* Has the glyph been stretched? */
+    /********************************/
+    /* GLYPH STRETCHED              */
+    /* Selector Bit:  2             */
+    /* Return Bit(s): 9             */
+    /*                              */
     if ( ( args[0] & 4 ) != 0 && CUR.tt_metrics.stretched )
       K |= 1 << 8;
 
-    /* Are we hinting for grayscale? */
+    /********************************/
+    /* HINTING FOR GRAYSCALE        */
+    /* Selector Bit:  5             */
+    /* Return Bit(s): 12            */
+    /*                              */
     if ( ( args[0] & 32 ) != 0 && CUR.grayscale )
       K |= 1 << 12;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode && CUR.rasterizer_version >= 35 )
+    {
+      /********************************/
+      /* HINTING FOR GRAYSCALE        */
+      /* Selector Bit:  5             */
+      /* Return Bit(s): 12            */
+      /*                              */
+      if ( ( args[0] & 32 ) != 0 && CUR.grayscale_hinting )
+        K |= 1 << 12;
+
+      /********************************/
+      /* HINTING FOR SUBPIXEL         */
+      /* Selector Bit:  6             */
+      /* Return Bit(s): 13            */
+      /*                              */
+      if ( ( args[0] & 64 ) != 0         &&
+           CUR.subpixel_hinting          &&
+           CUR.rasterizer_version >= 37 )
+      {
+        K |= 1 << 13;
+
+        /* the stuff below is irrelevant if subpixel_hinting is not set */
+
+        /********************************/
+        /* COMPATIBLE WIDTHS ENABLED    */
+        /* Selector Bit:  7             */
+        /* Return Bit(s): 14            */
+        /*                              */
+        /* Functionality still needs to be added */
+        if ( ( args[0] & 128 ) != 0 && CUR.compatible_widths )
+          K |= 1 << 14;
+
+        /********************************/
+        /* SYMMETRICAL SMOOTHING        */
+        /* Selector Bit:  8             */
+        /* Return Bit(s): 15            */
+        /*                              */
+        /* Functionality still needs to be added */
+        if ( ( args[0] & 256 ) != 0 && CUR.symmetrical_smoothing )
+          K |= 1 << 15;
+
+        /********************************/
+        /* HINTING FOR BGR?             */
+        /* Selector Bit:  9             */
+        /* Return Bit(s): 16            */
+        /*                              */
+        /* Functionality still needs to be added */
+        if ( ( args[0] & 512 ) != 0 && CUR.bgr )
+          K |= 1 << 16;
+
+        if ( CUR.rasterizer_version >= 38 )
+        {
+
+          /********************************/
+          /* SUBPIXEL POSITIONED?         */
+          /* Selector Bit:  10            */
+          /* Return Bit(s): 17            */
+          /*                              */
+          /* Functionality still needs to be added */
+          if ( ( args[0] & 1024 ) != 0 && CUR.subpixel_positioned )
+            K |= 1 << 17;
+        }
+      }
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     args[0] = K;
   }
 
@@ -7509,6 +8156,15 @@
     cur = *exc;
 #endif
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.ignore_x_mode )
+    {
+      /* ensure some variables are set for this run */
+      CUR.iup_called        = FALSE;
+      CUR.in_delta_function = FALSE;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* set CVT functions */
     CUR.tt_metrics.ratio = 0;
     if ( CUR.metrics.x_ppem != CUR.metrics.y_ppem )
@@ -7780,7 +8436,13 @@
           break;
 
         case 0x2B:  /* CALL */
-          Ins_CALL( EXEC_ARG_ args );
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+        if ( !CUR.ignore_x_mode                                         ||
+             !CUR.iup_called                                            ||
+             ( CUR.iup_called                                         &&
+             !( CUR.sph_tweak_flags & SPH_TWEAK_NO_CALL_AFTER_IUP ) ) )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+            Ins_CALL( EXEC_ARG_ args );
           break;
 
         case 0x2C:  /* FDEF */
@@ -7799,6 +8461,9 @@
 
         case 0x30:  /* IUP */
         case 0x31:  /* IUP */
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+          if ( CUR.ignore_x_mode ) CUR.iup_called = TRUE;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
           Ins_IUP( EXEC_ARG_ args );
           break;
 
@@ -7958,6 +8623,12 @@
           break;
 
         case 0x5D:  /* DELTAP1 */
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+        if ( !CUR.ignore_x_mode                                         ||
+             !CUR.iup_called                                            ||
+             ( CUR.iup_called                                         &&
+             !( CUR.sph_tweak_flags & SPH_TWEAK_NO_DELTAP_AFTER_IUP ) ) )
+#endif  /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
           Ins_DELTAP( EXEC_ARG_ args );
           break;
 
