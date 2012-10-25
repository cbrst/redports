--- src/truetype/rules.mk	2012-10-25 13:49:35.514523485 +0800
+++ src/truetype/rules.mk	2012-10-25 13:52:36.740524546 +0800
@@ -31,7 +31,8 @@
               $(TT_DIR)/ttinterp.c \
               $(TT_DIR)/ttobjs.c   \
               $(TT_DIR)/ttpic.c    \
-              $(TT_DIR)/ttpload.c
+              $(TT_DIR)/ttpload.c  \
+              $(TT_DIR)/ttsubpix.c
 
 # TrueType driver headers
 #
