--- src/truetype/truetype.c	2012-09-05 21:25:10.147018974 +0800
+++ src/truetype/truetype.c	2012-09-05 21:25:27.910025023 +0800
@@ -27,6 +27,7 @@
 
 #ifdef TT_USE_BYTECODE_INTERPRETER
 #include "ttinterp.c"
+#include "ttsubpix.c"
 #endif
 
 #ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
