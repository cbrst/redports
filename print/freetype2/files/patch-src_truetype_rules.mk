--- src/truetype/rules.mk	2012-09-05 21:25:10.148020154 +0800
+++ src/truetype/rules.mk	2012-09-05 21:25:27.909025589 +0800
@@ -31,7 +31,8 @@
               $(TT_DIR)/ttinterp.c \
               $(TT_DIR)/ttobjs.c   \
               $(TT_DIR)/ttpic.c    \
-              $(TT_DIR)/ttpload.c
+              $(TT_DIR)/ttpload.c  \
+              $(TT_DIR)/ttsubpix.c
 
 # TrueType driver headers
 #
