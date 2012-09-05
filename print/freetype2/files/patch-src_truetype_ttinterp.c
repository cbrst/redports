--- src/truetype/ttinterp.c	2012-09-05 21:25:10.150018323 +0800
+++ src/truetype/ttinterp.c	2012-09-05 21:25:37.856024129 +0800
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
@@ -249,6 +252,14 @@
 #define GUESS_VECTOR( V )
 #endif
 
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /*FT_Int    CUR.num_delta_funcs;
+    FT_ULong  inline_delta_funcs[5];
+    FT_Long   CUR.in_delta_function;*/
+#endif
+
+
   /*************************************************************************/
   /*                                                                       */
   /*                        CODERANGE FUNCTIONS                            */
@@ -1846,16 +1857,26 @@
     FT_ASSERT( !CUR.face->unpatented_hinting );
 #endif
 
-    v = CUR.GS.freeVector.x;
-
-    if ( v != 0 )
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( !CUR.enhanced_mode                                         ||
+         ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_DMOVE_FREEV ) )
     {
-      zone->cur[point].x += TT_MULDIV( distance,
-                                       v * 0x10000L,
-                                       CUR.F_dot_P );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+      v = CUR.GS.freeVector.x;
 
-      zone->tags[point] |= FT_CURVE_TAG_TOUCH_X;
+      if ( v != 0 )
+      {
+        zone->cur[point].x += TT_MULDIV( distance,
+                                         v * 0x10000L,
+                                         CUR.F_dot_P );
+
+        zone->tags[point] |= FT_CURVE_TAG_TOUCH_X;
+      }
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
     }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     v = CUR.GS.freeVector.y;
 
@@ -1932,8 +1953,18 @@
   {
     FT_UNUSED_EXEC;
 
-    zone->cur[point].x += distance;
-    zone->tags[point]  |= FT_CURVE_TAG_TOUCH_X;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( !CUR.enhanced_mode                                          ||
+         ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_DMOVEX_FREEV ) )
+    {
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
+      zone->cur[point].x += distance;
+      zone->tags[point]  |= FT_CURVE_TAG_TOUCH_X;
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
   }
 
 
@@ -1994,6 +2025,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    The compensated distance.                                          */
   /*                                                                       */
@@ -2005,11 +2038,13 @@
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
@@ -2024,6 +2059,7 @@
       if ( val > 0 )
         val = 0;
     }
+
     return val;
   }
 
@@ -2041,12 +2077,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
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
 
@@ -2055,15 +2094,15 @@
 
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
@@ -2085,12 +2124,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
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
 
@@ -2099,13 +2141,15 @@
 
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
@@ -2127,12 +2171,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
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
 
@@ -2143,13 +2190,13 @@
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
@@ -2171,12 +2218,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
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
 
@@ -2185,15 +2235,15 @@
 
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
@@ -2215,12 +2265,15 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
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
 
@@ -2229,15 +2282,15 @@
 
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
@@ -2259,6 +2312,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
@@ -2270,10 +2325,13 @@
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
@@ -2309,6 +2367,8 @@
   /*                                                                       */
   /*    compensation :: The engine compensation.                           */
   /*                                                                       */
+  /*    resolution   :: The grid resolution.                               */
+  /*                                                                       */
   /* <Return>                                                              */
   /*    Rounded distance.                                                  */
   /*                                                                       */
@@ -2318,10 +2378,13 @@
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
@@ -2404,32 +2467,38 @@
   /*    Sets Super Round parameters.                                       */
   /*                                                                       */
   /* <Input>                                                               */
-  /*    GridPeriod :: Grid period                                          */
-  /*    selector   :: SROUND opcode                                        */
+  /*    GridPeriod :: The grid period.                                     */
+  /*                                                                       */
+  /*    selector   :: The SROUND opcode.                                   */
+  /*                                                                       */
+  /*    resolution :: The grid resolution.                                 */
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
-      case 0:
-        CUR.period = GridPeriod / 2;
-        break;
+    case 0:
+      CUR.period = GridPeriod / 2;
+      break;
 
-      case 0x40:
-        CUR.period = GridPeriod;
-        break;
+    case 0x40:
+      CUR.period = GridPeriod;
+      break;
 
-      case 0x80:
-        CUR.period = GridPeriod * 2;
-        break;
+    case 0x80:
+      CUR.period = GridPeriod * 2;
+      break;
 
-      /* This opcode is reserved, but... */
+    /* This opcode is reserved, but... */
 
-      case 0xC0:
-        CUR.period = GridPeriod;
-        break;
+    case 0xC0:
+      CUR.period = GridPeriod;
+      break;
     }
 
     switch ( (FT_Int)( selector & 0x30 ) )
@@ -3080,13 +3149,13 @@
 
 
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
 
@@ -3262,12 +3331,12 @@
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
@@ -3316,6 +3385,35 @@
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
+       if ( CUR.enhanced_mode                               && \
+            ( I == 24 || I == 22 || I == 8 )           && \
+            !( CUR.sph_tweak_flags & SPH_TWEAK_DO_RS ) )  \
+         args[0] = 0;                                     \
+       else                                               \
+         args[0] = CUR.storage[I];                        \
+     }                                                    \
+   }
+
+#else /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 #define DO_RS                           \
    {                                    \
@@ -3325,9 +3423,7 @@
      if ( BOUNDSL( I, CUR.storeSize ) ) \
      {                                  \
        if ( CUR.pedantic_hinting )      \
-       {                                \
          ARRAY_BOUND_ERROR;             \
-       }                                \
        else                             \
          args[0] = 0;                   \
      }                                  \
@@ -3335,6 +3431,8 @@
        args[0] = CUR.storage[I];        \
    }
 
+#endif /* !TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
 #define DO_WS                           \
    {                                    \
@@ -3344,9 +3442,7 @@
      if ( BOUNDSL( I, CUR.storeSize ) ) \
      {                                  \
        if ( CUR.pedantic_hinting )      \
-       {                                \
          ARRAY_BOUND_ERROR;             \
-       }                                \
      }                                  \
      else                               \
        CUR.storage[I] = args[1];        \
@@ -3361,9 +3457,7 @@
      if ( BOUNDSL( I, CUR.cvtSize ) )    \
      {                                   \
        if ( CUR.pedantic_hinting )       \
-       {                                 \
          ARRAY_BOUND_ERROR;              \
-       }                                 \
        else                              \
          args[0] = 0;                    \
      }                                   \
@@ -3380,9 +3474,7 @@
      if ( BOUNDSL( I, CUR.cvtSize ) )    \
      {                                   \
        if ( CUR.pedantic_hinting )       \
-       {                                 \
          ARRAY_BOUND_ERROR;              \
-       }                                 \
      }                                   \
      else                                \
        CUR_Func_write_cvt( I, args[1] ); \
@@ -3397,9 +3489,7 @@
      if ( BOUNDSL( I, CUR.cvtSize ) )                           \
      {                                                          \
        if ( CUR.pedantic_hinting )                              \
-       {                                                        \
          ARRAY_BOUND_ERROR;                                     \
-       }                                                        \
      }                                                          \
      else                                                       \
        CUR.cvt[I] = TT_MULFIX( args[1], CUR.tt_metrics.scale ); \
@@ -3410,15 +3500,17 @@
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
@@ -3436,10 +3528,11 @@
 
 #undef  ARRAY_BOUND_ERROR
 #define ARRAY_BOUND_ERROR                   \
+    do                                      \
     {                                       \
       CUR.error = TT_Err_Invalid_Reference; \
       return;                               \
-    }
+    } while ( 0 )
 
 
   /*************************************************************************/
@@ -4592,6 +4685,23 @@
     TT_DefRecord*  rec;
     TT_DefRecord*  limit;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
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
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* some font programs are broken enough to redefine functions! */
     /* We will then parse the current table.                       */
@@ -4625,10 +4735,11 @@
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
@@ -4638,6 +4749,83 @@
 
     while ( SKIP_Code() == SUCCESS )
     {
+#ifdef SPH_DEBUG_MORE_VERBOSE
+      printf ("%d ", CUR.opcode);
+#endif
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+
+      if (CUR.enhanced_mode && ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_INLINE_DELTAS ))
+      for ( i = 0; i < opcode_patterns; i++ )
+      {
+        if ( CUR.opcode == opcode_pattern[i][opcode_pointer[i]] )
+        {
+#ifdef SPH_DEBUG_MORE_VERBOSE
+          printf( "function %d, opcode ptrn: %d"
+                  "  op# %d: %d FOUND \n",
+                  n, i, opcode_pointer[i], CUR.opcode );
+#endif
+
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
+
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
+
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
+
+            opcode_pointer[i] = 0;
+          }
+        }
+
+        else
+          opcode_pointer[i] = 0;
+      }
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
       switch ( CUR.opcode )
       {
       case 0x89:    /* IDEF */
@@ -4681,6 +4869,15 @@
 
     CUR.step_ins = FALSE;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /*
+     *  CUR.enhanced_mode may be turned off prior to function calls.  This
+     *  ensures it is turned back on.
+     */
+    CUR.enhanced_mode = ( CUR.subpixel_hinting || CUR.grayscale_hinting ) 
+      && !( CUR.sph_tweak_flags & SPH_TWEAK_PIXEL_HINTING );
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     if ( pRec->Cur_Count > 0 )
     {
       CUR.callTop++;
@@ -4714,6 +4911,10 @@
     TT_CallRec*    pCrec;
     TT_DefRecord*  def;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Bool        oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* first of all, check the index */
 
@@ -4751,6 +4952,26 @@
     if ( !def->active )
       goto Fail;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* This is test code used to detect inline delta functions */
+    oldF = def->inline_delta;
+    if ( CUR.enhanced_mode )
+    {
+      if ( def->inline_delta )
+        CUR.in_delta_function = TRUE;
+    }
+
+    if (FALSE)
+    if ( F == 35 || F == 34 )
+    {
+      CUR.enhanced_mode = FALSE;
+#ifdef SPH_DEBUG
+      printf("Entering %d\n", F);
+#endif
+    }
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* check the call stack */
     if ( CUR.callTop >= CUR.callSize )
     {
@@ -4772,6 +4993,19 @@
                         def->start );
 
     CUR.step_ins = FALSE;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    CUR.in_delta_function = oldF;
+
+    if (FALSE)
+    if ( F == 35 || F == 34 )
+    {
+      CUR.enhanced_mode = !( CUR.sph_tweak_flags & SPH_TWEAK_PIXEL_HINTING );
+#ifdef SPH_DEBUG
+      printf("Leaving %d\n", F);
+#endif
+    }
+
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     return;
 
   Fail:
@@ -4792,6 +5026,10 @@
     TT_CallRec*    pCrec;
     TT_DefRecord*  def;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Bool        oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
 
     /* first of all, check the index */
     F = args[1];
@@ -4828,6 +5066,15 @@
     if ( !def->active )
       goto Fail;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    oldF = def->inline_delta;
+    if ( CUR.enhanced_mode )
+    {
+      if ( def->inline_delta )
+        CUR.in_delta_function = TRUE;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* check stack */
     if ( CUR.callTop >= CUR.callSize )
     {
@@ -4851,6 +5098,11 @@
 
       CUR.step_ins = FALSE;
     }
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    CUR.in_delta_function = oldF;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     return;
 
   Fail:
@@ -5200,6 +5452,12 @@
       }
     }
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* Disable Type 2 Vacuform Rounds - e.g. Arial Narrow */
+    if ( CUR.enhanced_mode && FT_ABS( D ) == 64 )
+      D += 1;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     args[0] = D;
   }
 
@@ -5694,12 +5952,21 @@
     }
 #endif
 
-    if ( CUR.GS.freeVector.x != 0 )
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( !CUR.enhanced_mode                                               ||
+         ( CUR.enhanced_mode                                            &&
+           ( CUR.sph_tweak_flags & SPH_TWEAK_ALLOW_MOVEZP2_FREEV ) ) )
     {
-      CUR.zp2.cur[point].x += dx;
-      if ( touch )
-        CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_X;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+      if ( CUR.GS.freeVector.x != 0 )
+      {
+        CUR.zp2.cur[point].x += dx;
+        if ( touch )
+          CUR.zp2.tags[point] |= FT_CURVE_TAG_TOUCH_X;
+      }
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
     }
+#endif
 
     if ( CUR.GS.freeVector.y != 0 )
     {
@@ -5877,6 +6144,9 @@
   {
     FT_F26Dot6  dx, dy;
     FT_UShort   point;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      B1, B2;
+#endif
 
 
     if ( CUR.top < CUR.GS.loop + 1 )
@@ -5922,11 +6192,111 @@
         }
       }
       else
-        MOVE_Zp2_Point( point, dx, dy, TRUE );
+      {
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+        /*
+         *  The conditionals here still do not do a perfect job and need
+         *  work.
+         *
+         *  If not using enhanced_mode rendering, allow ZP2 move.
+         *
+         *  If using enhanced_mode rendering, allow ZP2 point move if
+         *
+         *  - the glyph is composite
+         *  - the glyph is specifically set to allow SHPIX moves
+         *  - the move is in the Y direction on a previously touched point
+         *
+         *  It seems that what qualifies as a previously touched point varies
+         *  somewhat from font to font.  Some render better when either X
+         *  or Y must be touched (SPH_TWEAK_SHPIX_EITHER_TOUCHED) and some render
+         *  better when both must be touched.
+         */
+
+        if ( CUR.enhanced_mode )
+        {
+          if ( CUR.GS.freeVector.y != 0 )
+            B1 = CUR.zp2.cur[point].y;
+          else
+            B1 = CUR.zp2.cur[point].x;
+
+          if ( CUR.enhanced_mode                                                   &&
+              ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_INLINE_MOVES ) )
+          {
+            dx = FT_PIX_ROUND ( dx );                
+            dy = FT_PIX_ROUND ( dy );               
+          }
+          
+          if ( /* ORIG WAY  */
+               ( CUR.sph_tweak_flags & ~SPH_TWEAK_SKIP_INLINE_DELTAS ) && ( CUR.is_composite                                 ||
+               ( CUR.sph_tweak_flags &
+                   SPH_TWEAK_DO_SHPIX )                         ||
+               ( CUR.GS.freeVector.y != 0                     &&
+                 CUR.iup_called == 0                          &&
+                 CUR.iupy_called == 0                         &&
+                 ( ( ( CUR.sph_tweak_flags &
+                         SPH_TWEAK_SHPIX_EITHER_TOUCHED )       &&
+                     ( ( CUR.pts.tags[point] &
+                           FT_CURVE_TAG_TOUCH_X ) != 0 ||
+                       ( CUR.pts.tags[point] &
+                           FT_CURVE_TAG_TOUCH_Y ) != 0 ) ) ||
+                   ( !( CUR.sph_tweak_flags &
+                          SPH_TWEAK_SHPIX_EITHER_TOUCHED )      &&
+                     ( ( CUR.pts.tags[point] &
+                           FT_CURVE_TAG_TOUCH_X ) != 0 &&
+                       ( CUR.pts.tags[point] &
+                           FT_CURVE_TAG_TOUCH_Y ) != 0 ) ) )
+                        ) ) 
+            /*||    NEW WAY BUT CRASHES CHROME 
+             CUR.sph_tweak_flags & SPH_TWEAK_SKIP_INLINE_DELTAS && ( CUR.is_composite                                ||
+              ( CUR.sph_tweak_flags & SPH_TWEAK_DO_SHPIX )       ||
+              ( CUR.GS.freeVector.y != 0                      
+                && CUR.iup_called == 0 
+                && CUR.iupy_called == 0 
+                && ( 
+                  ( CUR.in_delta_function 
+                    && ( CUR.pts.tags[point] & FT_CURVE_TAG_TOUCH_X ) 
+                    && ( CUR.pts.tags[point] & FT_CURVE_TAG_TOUCH_Y ) ) 
+                 || 
+                  ( CUR.in_delta_function 
+                    && ( CUR.sph_tweak_flags & SPH_TWEAK_SHPIX_EITHER_TOUCHED )
+                    && ( CUR.pts.tags[point] & FT_CURVE_TAG_TOUCH_X  
+                      || CUR.pts.tags[point] & FT_CURVE_TAG_TOUCH_Y ) )  
+                 ||
+                  !CUR.in_delta_function  )))*/
+          )
+            
+          {
+            MOVE_Zp2_Point( point, dx, dy, TRUE );
+            
+            if ( CUR.GS.freeVector.y != 0 )
+              B2 = CUR.zp2.cur[point].y;
+            else B2 = CUR.zp2.cur[point].x;
+
+            /* reverse moves that move the point off a pixel boundary */
+            if ( ( CUR.sph_tweak_flags &
+                    SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES ) &&
+                B1 % 64 == 0                               &&
+                B2 % 64 != 0                               )
+            {
+#ifdef SPH_DEBUG
+              printf( "Reversing ZP2 move\n" );
+#endif
+              MOVE_Zp2_Point( point, -dx, -dy, TRUE );
+            }
+          }
+        }
+        else
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+          MOVE_Zp2_Point( point, dx, dy, TRUE );
+      }
 
       CUR.GS.loop--;
     }
 
+#ifdef SPH_DEBUG_MORE_VERBOSE
+    printf( "SHPIX: %d\n", CUR.in_delta_function );
+#endif
+
   Fail:
     CUR.GS.loop = 1;
     CUR.new_top = CUR.args;
@@ -5944,7 +6314,18 @@
   {
     FT_UShort   point;
     FT_F26Dot6  distance;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      resolution = 1;
+
 
+    if ( CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0 )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_X;
+      else if ( CUR.GS.freeVector.y != 0 )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     point = (FT_UShort)args[0];
 
@@ -5968,6 +6349,16 @@
     distance = CUR_Func_project( CUR.zp1.cur + point,
                                  CUR.zp0.cur + CUR.GS.rp0 );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /* subpixel hinting - make MSIRP respect CVT cut-in; */
+    /* this fixes `k' issue with Arial                   */
+    if ( CUR.enhanced_mode                              &&
+         CUR.GS.freeVector.x != 0                  &&
+         FT_ABS( distance - args[1] ) >=
+           CUR.GS.control_value_cutin / resolution )
+      distance = args[1];
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     CUR_Func_move( &CUR.zp1, point, args[1] - distance );
 
     CUR.GS.rp1 = CUR.GS.rp0;
@@ -5990,7 +6381,21 @@
     FT_UShort   point;
     FT_F26Dot6  cur_dist,
                 distance;
+    FT_Int      resolution = 1;
+
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                               &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MDAP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_X;
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MDAP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
     point = (FT_UShort)args[0];
 
@@ -6005,7 +6410,8 @@
     {
       cur_dist = CUR_fast_project( &CUR.zp0.cur[point] );
       distance = CUR_Func_round( cur_dist,
-                                 CUR.tt_metrics.compensations[0] ) - cur_dist;
+                                 CUR.tt_metrics.compensations[0],
+                                 resolution ) - cur_dist;
     }
     else
       distance = 0;
@@ -6030,8 +6436,22 @@
     FT_UShort   point;
     FT_F26Dot6  distance,
                 org_dist;
+    FT_Int      resolution = 1;
 
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                               &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MIAP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_X;
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MIAP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     cvtEntry = (FT_ULong)args[1];
     point    = (FT_UShort)args[0];
 
@@ -6076,12 +6496,15 @@
 
     org_dist = CUR_fast_project( &CUR.zp0.cur[point] );
 
-    if ( ( CUR.opcode & 1 ) != 0 )   /* rounding and control cutin flag */
+    if ( ( CUR.opcode & 1 ) != 0 )   /* rounding and control cut-in flag */
     {
-      if ( FT_ABS( distance - org_dist ) > CUR.GS.control_value_cutin )
+      if ( FT_ABS( distance - org_dist ) >
+             CUR.GS.control_value_cutin / resolution )
         distance = org_dist;
 
-      distance = CUR_Func_round( distance, CUR.tt_metrics.compensations[0] );
+      distance = CUR_Func_round( distance,
+                                 CUR.tt_metrics.compensations[0],
+                                 resolution );
     }
 
     CUR_Func_move( &CUR.zp0, point, distance - org_dist );
@@ -6103,6 +6526,24 @@
   {
     FT_UShort   point;
     FT_F26Dot6  org_dist, distance;
+    FT_Int      minimum_distance_factor = 64;
+    FT_Int      resolution              = 1;
+
+
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                               &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MDRP ) )
+      {
+        resolution              = SPH_OPTION_GRIDS_PER_PIXEL_X;
+        minimum_distance_factor = 64 - resolution / 3;
+      }
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MDRP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 
     point = (FT_UShort)args[0];
@@ -6168,11 +6609,13 @@
     if ( ( CUR.opcode & 4 ) != 0 )
       distance = CUR_Func_round(
                    org_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   resolution );
     else
       distance = ROUND_None(
                    org_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   resolution );
 
     /* minimum distance flag */
 
@@ -6180,13 +6623,17 @@
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
 
@@ -6223,10 +6670,36 @@
                 cur_dist,
                 org_dist;
 
+    FT_Int      minimum_distance_factor = 64;
+    FT_Int      resolution              = 1;
+
+    FT_Int      B1;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    FT_Int      B2;
+#endif
 
     point    = (FT_UShort)args[0];
     cvtEntry = (FT_ULong)( args[1] + 1 );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0                               &&
+           !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MIRP ) )
+      {
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_X;
+        /* high value emboldens glyphs at lower ppems (< 14); */
+        /* Courier looks better with 52 --                    */
+        /* MS ClearType Rasterizer supposedly uses 32         */
+        minimum_distance_factor = 64 - resolution / 3;
+      }
+
+      else if ( CUR.GS.freeVector.y != 0                               &&
+                !( CUR.sph_tweak_flags & SPH_TWEAK_NORMAL_ROUND_MIRP ) )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* XXX: UNDOCUMENTED! cvt[-1] = 0 always */
 
     if ( BOUNDS( point,      CUR.zp1.n_points ) ||
@@ -6279,8 +6752,15 @@
       if ( ( org_dist ^ cvt_dist ) < 0 )
         cvt_dist = -cvt_dist;
     }
-
-    /* control value cutin and round */
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    if ( CUR.GS.freeVector.y != 0                               &&
+                ( CUR.sph_tweak_flags & SPH_TWEAK_TIMES_NEW_ROMAN_HACK ) )
+    {
+      if ( cur_dist < -64 ) cvt_dist -= 16;
+      else if ( cur_dist > 64  && cur_dist < 84) cvt_dist += 32;
+    }
+#endif
+    /* control value cut-in and round */
 
     if ( ( CUR.opcode & 4 ) != 0 )
     {
@@ -6301,18 +6781,21 @@
         /*      `ttinst2.doc', version 1.66, is thus incorrect since  */
         /*      it implies `>=' instead of `>'.                       */
 
-        if ( FT_ABS( cvt_dist - org_dist ) > CUR.GS.control_value_cutin )
+        if ( FT_ABS( cvt_dist - org_dist ) >
+               CUR.GS.control_value_cutin / resolution )
           cvt_dist = org_dist;
       }
 
       distance = CUR_Func_round(
                    cvt_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   resolution );
     }
     else
       distance = ROUND_None(
                    cvt_dist,
-                   CUR.tt_metrics.compensations[CUR.opcode & 3] );
+                   CUR.tt_metrics.compensations[CUR.opcode & 3],
+                   resolution );
 
     /* minimum distance test */
 
@@ -6320,18 +6803,55 @@
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
-      }
-    }
+        if ( distance > -FT_MulDiv( minimum_distance_factor,
+                                    CUR.GS.minimum_distance, 64 ) )
+          distance = -FT_MulDiv( minimum_distance_factor,
+                                 CUR.GS.minimum_distance, 64 );
+      }
+    }
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    if ( CUR.GS.freeVector.y != 0 
+      && ( CUR.opcode & 16 ) == 0 
+      && ( CUR.opcode & 8 ) == 0 
+      && ( CUR.sph_tweak_flags & SPH_TWEAK_COURIER_NEW_2_HACK ) )
+      distance +=48;        
+#endif  
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode                                                   &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_INLINE_MOVES ) )
+      distance = FT_PIX_ROUND( distance );   
+#endif
+    B1 = CUR.zp1.cur[point].y;
 
     CUR_Func_move( &CUR.zp1, point, distance - cur_dist );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode                                                   &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES ) )
+    {
+      B2 = CUR.zp1.cur[point].y;
+
+      if ( ( CUR.GS.freeVector.x != 0 && B1 % 64 == 0 && B2 % 64 != 0 ) ||
+           ( CUR.GS.freeVector.y != 0 && B2 % 64 != 0 )                 )
+      {
+        /* reverse the MIRP move; ideally this could be implemented better */
+        CUR_Func_move( &CUR.zp1, point, -( distance - cur_dist ) );
+      }
+    }
+    
+   /* else if ( CUR.enhanced_mode                                                   &&    CUR.GS.freeVector.y != 0 &&
+    abs(distance - cur_dist) > 32 && ( CUR.sph_tweak_flags & SPH_TWEAK_DELTAP_SKIP_EXAGGERATED_VALUES ) )
+        CUR_Func_move( &CUR.zp1, point, -( distance - cur_dist ) + FT_PIX_ROUND (distance - cur_dist) );  */
+        
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
   Fail:
     CUR.GS.rp1 = CUR.GS.rp0;
 
@@ -6355,8 +6875,14 @@
     FT_F26Dot6  distance;
 
     FT_UNUSED_ARG;
-
-
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode && CUR.iup_called                                &&
+         ( CUR.sph_tweak_flags & SPH_TWEAK_NO_ALIGNRP_AFTER_IUP ) )
+    {
+      CUR.error = TT_Err_Invalid_Reference;      
+      goto Fail;
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     if ( CUR.top < CUR.GS.loop ||
          BOUNDS( CUR.GS.rp0, CUR.zp0.n_points ) )
     {
@@ -6851,6 +7377,15 @@
     contour = 0;
     point   = 0;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode )
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
@@ -6920,7 +7455,19 @@
     FT_UShort  A;
     FT_ULong   C;
     FT_Long    B;
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    /*FT_Int     resolution = 1;*/
+    FT_UShort  B1, B2;
+
 
+    /*if (CUR.enhanced_mode )
+    {
+      if ( CUR.GS.freeVector.x != 0 )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_X;
+      else if ( CUR.GS.freeVector.y != 0 )
+        resolution = SPH_OPTION_GRIDS_PER_PIXEL_Y;
+    }*/
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
 
 #ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
     /* Delta hinting is covered by US Patent 5159668. */
@@ -6993,7 +7540,85 @@
             B++;
           B = B * 64 / ( 1L << CUR.GS.delta_shift );
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+          /*
+           *  Undocumented hack that rounds the point prior to or instead
+           *  of the delta move.  Fixes glitches in various fonts due to bad
+           *  y-hinting routines.
+           */
+          if ( CUR.enhanced_mode && CUR.GS.freeVector.y != 0 )
+          {
+            FT_Byte  orig_round_state = CUR.GS.round_state;
+
+
+            if ( CUR.sph_tweak_flags & SPH_TWEAK_DELTAP_RDTG )
+            {
+              COMPUTE_Round( TT_Round_Down_To_Grid );
+              B = CUR_Func_round( B, CUR.tt_metrics.compensations[0], 1 );
+            }
+
+            else if ( CUR.sph_tweak_flags & SPH_TWEAK_DELTAP_RTG )
+            {
+              COMPUTE_Round( TT_Round_To_Grid );
+              B = CUR_Func_round( B, CUR.tt_metrics.compensations[0], 1 );
+            }
+
+            COMPUTE_Round( orig_round_state );
+          }
+
+          /*
+           *  Allow delta move if
+           *
+           *  - not using enhanced_mode rendering
+           *  - glyph is specifically set to allow it
+           *  - glyph is composite
+           */
+          if ( !CUR.enhanced_mode                                        ||
+               ( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_DO_DELTAP ) ||
+               CUR.is_composite                                     )
+            CUR_Func_move( &CUR.zp0, A, B );
+
+          else if ( !( CUR.sph_tweak_flags & SPH_TWEAK_ALWAYS_SKIP_DELTAP ) )
+          {
+            /* save the y value of the point now; compare after move */
+            B1 = CUR.zp0.cur[A].y;
+
+            if ( CUR.enhanced_mode                                                   &&
+                ( CUR.sph_tweak_flags & SPH_TWEAK_ROUND_NONPIXEL_INLINE_MOVES ) )
+              B = FT_PIX_ROUND( B );             
+                  
+            /*
+             *  Allow delta move if using enhanced_mode rendering, IUP has not
+             *  been called, and point is touched on X or Y.
+             *
+             *  Working code, but needs more features.
+             */
+            if ( CUR.enhanced_mode                                        &&
+                 CUR.GS.freeVector.y != 0                            &&
+                 CUR.iup_called == 0                                 &&
+                 CUR.iupy_called == 0                                &&
+                 ( ( CUR.pts.tags[A] & FT_CURVE_TAG_TOUCH_X ) != 0 ||
+                   ( CUR.pts.tags[A] & FT_CURVE_TAG_TOUCH_Y ) != 0 ) )
+              /* Should resolution always be 1 for this move? */
+              CUR_Func_move( &CUR.zp0, A, B );
+
+            B2 = CUR.zp0.cur[A].y;
+
+            /*
+             *  reverse this move if it results in a move off a pixel
+             *  boundary
+             */
+            if ( ( CUR.sph_tweak_flags &
+                     SPH_TWEAK_DELTAP_SKIP_EXAGGERATED_VALUES )
+                || ( ( CUR.sph_tweak_flags &
+                     SPH_TWEAK_SKIP_NONPIXEL_INLINE_MOVES ) &&
+                   B1 % 64 == 0 && B2 % 64 != 0 ) )
+
+              CUR_Func_move( &CUR.zp0, A, -B );
+          }
+#else
           CUR_Func_move( &CUR.zp0, A, B );
+#endif /* *TT_CONFIG_OPTION_SUBPIXEL_HINTING */
         }
       }
       else
@@ -7119,26 +7744,135 @@
   Ins_GETINFO( INS_ARG )
   {
     FT_Long  K;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    /* INFINALITY ENVVAR TO FORCE SUBPIXEL INTERPRETER (useful in debugging) */
+    FT_UInt force_subpixel_interpreter = 0;
+    FT_UInt checked_force_subpixel_interpreter = 0;
 
-
+    if ( checked_force_subpixel_interpreter == 0)
+    {
+      char *force_subpixel_interpreter_env = getenv( "INFINALITY_FT_DEBUG_FORCE_SUBPIXEL_INTERPRETER" );
+      if ( force_subpixel_interpreter_env != NULL )
+      {
+        if ( strcasecmp(force_subpixel_interpreter_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(force_subpixel_interpreter_env, "true") == 0) force_subpixel_interpreter = TRUE;
+          else if ( strcasecmp(force_subpixel_interpreter_env, "1") == 0) force_subpixel_interpreter = TRUE;
+          else if ( strcasecmp(force_subpixel_interpreter_env, "on") == 0) force_subpixel_interpreter = TRUE;
+          else if ( strcasecmp(force_subpixel_interpreter_env, "yes") == 0) force_subpixel_interpreter = TRUE;
+        }
+      }
+      checked_force_subpixel_interpreter = 1;
+    }
+#endif
     K = 0;
 
-    /* We return MS rasterizer version 1.7 for the font scaler. */
+    /********************************/
+    /* RASTERIZER VERSION           */
+    /* Selector Bit:  0             */
+    /* Return Bit(s): 0-7           */
+    /*                              */
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( ( args[0] & 1 ) != 0 && CUR.enhanced_mode )
+    {
+      K = CUR.rasterizer_version;
+#ifdef SPH_DEBUG_MORE_VERBOSE
+      printf(" SETTING AS %d\n", CUR.rasterizer_version );
+#endif
+    }
+    else
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
     if ( ( args[0] & 1 ) != 0 )
+    {
       K = 35;
+#ifdef SPH_DEBUG_MORE_VERBOSE
+      printf(" SETTING AS 35\n" );
+#endif
+    }
 
-    /* Has the glyph been rotated? */
+    /********************************/
+    /* GLYPH ROTATED                */
+    /* Selector Bit:  1             */
+    /* Return Bit(s): 8             */
+    /*                              */
     if ( ( args[0] & 2 ) != 0 && CUR.tt_metrics.rotated )
-      K |= 0x80;
+      K |= 1 << 8;
 
-    /* Has the glyph been stretched? */
+    /********************************/
+    /* GLYPH STRETCHED              */
+    /* Selector Bit:  2             */
+    /* Return Bit(s): 9             */
+    /*                              */
     if ( ( args[0] & 4 ) != 0 && CUR.tt_metrics.stretched )
-      K |= 1 << 8;
+      K |= 1 << 9;
 
-    /* Are we hinting for grayscale? */
+    /********************************/
+    /* HINTING FOR GRAYSCALE        */
+    /* Selector Bit:  5             */
+    /* Return Bit(s): 12            */
+    /*                              */
     if ( ( args[0] & 32 ) != 0 && CUR.grayscale )
       K |= 1 << 12;
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if ( CUR.enhanced_mode && CUR.rasterizer_version >= 35)
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
+      if ( ( args[0] & 64 ) != 0 && CUR.subpixel_hinting && CUR.rasterizer_version >= 37)
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
+      }
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET
+    /* INFINALITY ENVVAR TO FORCE SUBPIXEL INTERPRETER (useful in debugging) */
+    if ( force_subpixel_interpreter )
+    {
+      K |= 1 << 13;
+      CUR.subpixel_hinting = TRUE;
+    }
+#endif
     args[0] = K;
   }
 
@@ -7514,6 +8248,18 @@
     cur = *exc;
 #endif
 
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+    if (CUR.enhanced_mode)
+    {
+      /* ensure some variables are set for this run */
+      CUR.iup_called        = FALSE;
+      CUR.iupy_called       = FALSE;
+      CUR.in_delta_function = FALSE;
+
+      /*if ( CUR.sph_tweak_flags & SPH_TWEAK_DONT_HINT ) return TT_Err_Ok;*/
+    }
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+
     /* set CVT functions */
     CUR.tt_metrics.ratio = 0;
     if ( CUR.metrics.x_ppem != CUR.metrics.y_ppem )
@@ -7785,7 +8531,12 @@
           break;
 
         case 0x2B:  /* CALL */
-          Ins_CALL( EXEC_ARG_ args );
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING          
+        if ( !CUR.enhanced_mode 
+          || !CUR.iup_called                                
+          || !( CUR.sph_tweak_flags & SPH_TWEAK_NO_CALL_AFTER_IUP ) )
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */          
+            Ins_CALL( EXEC_ARG_ args );
           break;
 
         case 0x2C:  /* FDEF */
@@ -7803,8 +8554,16 @@
 
 
         case 0x30:  /* IUP */
+          Ins_IUP( EXEC_ARG_ args );
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+          if (CUR.enhanced_mode) CUR.iup_called = TRUE;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING */
+          break;
         case 0x31:  /* IUP */
           Ins_IUP( EXEC_ARG_ args );
+#ifdef TT_CONFIG_OPTION_SUBPIXEL_HINTING
+          if (CUR.enhanced_mode) CUR.iupy_called = TRUE;
+#endif /* TT_CONFIG_OPTION_SUBPIXEL_HINTING*/
           break;
 
         case 0x32:  /* SHP */
