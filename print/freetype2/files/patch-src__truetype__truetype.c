--- src/truetype/truetype.c	2012-10-25 13:49:35.514523485 +0800
+++ src/truetype/truetype.c	2012-10-25 13:52:36.740524546 +0800
@@ -27,6 +27,7 @@
 
 #ifdef TT_USE_BYTECODE_INTERPRETER
 #include "ttinterp.c"
+#include "ttsubpix.c"
 #endif
 
 #ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
