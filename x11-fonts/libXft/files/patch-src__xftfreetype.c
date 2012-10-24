--- src/xftfreetype.c	2009-10-16 23:27:08.000000000 +0200
+++ src/xftfreetype.c	2010-11-03 11:56:52.780577002 +0100
@@ -469,6 +469,21 @@
 	goto bail1;
     }
     
+#ifdef FC_LCD_FILTER 
+    /*
+     * Get lcd_filter value
+     */
+    switch (FcPatternGetInteger (pattern, FC_LCD_FILTER, 0, &fi->lcd_filter)) {
+    case FcResultNoMatch:
+	fi->lcd_filter = FC_LCD_DEFAULT;
+	break;
+    case FcResultMatch:
+	break;
+    default:
+	goto bail1;
+    }
+#endif
+    
     /*
      * Get matrix and transform values
      */
