--- src/base/ftoutln.c	2012-10-25 13:49:35.471524091 +0800
+++ src/base/ftoutln.c	2012-10-25 13:52:45.976526283 +0800
@@ -897,7 +897,29 @@
     FT_Vector   v_prev, v_first, v_next, v_cur;
     FT_Int      c, n, first;
     FT_Int      orientation;
+#ifdef FT_CONFIG_OPTION_INFINALITY_PATCHSET 
+    int checked_use_various_tweaks_env = 0;
+    FT_Bool use_various_tweaks = FALSE;
 
+    if ( checked_use_various_tweaks_env == 0 )
+    {
+      char *use_various_tweaks_env = getenv( "INFINALITY_FT_USE_VARIOUS_TWEAKS" );
+      if ( use_various_tweaks_env != NULL )
+      {
+        if ( strcasecmp(use_various_tweaks_env, "default" ) != 0 )
+        {
+          if ( strcasecmp(use_various_tweaks_env, "true") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "1") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "on") == 0) use_various_tweaks = TRUE;
+          else if ( strcasecmp(use_various_tweaks_env, "yes") == 0) use_various_tweaks = TRUE;
+        }
+      }
+      checked_use_various_tweaks_env = 1;
+    }
+
+    if ( use_various_tweaks )
+      ystrength = FT_PIX_FLOOR ( ystrength );
+#endif
 
     if ( !outline )
       return FT_Err_Invalid_Argument;
