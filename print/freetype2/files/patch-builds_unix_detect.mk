--- builds/unix/detect.mk	2012-09-05 21:25:10.267017167 +0800
+++ builds/unix/detect.mk	2012-09-05 21:25:16.055016895 +0800
@@ -22,6 +22,9 @@
                      $(wildcard /usr/sbin/init) \
                      $(wildcard /dev/null) \
                      $(wildcard /hurd/auth))
+  # FreeBSD is "unix" so just mandate this to fix the build in
+  # jails that do not have init (see PR#113470).
+  is_unix := true
   ifneq ($(is_unix),)
 
     PLATFORM := unix
@@ -80,9 +83,9 @@
   ifdef must_configure
     ifneq ($(have_Makefile),)
       # we are building FT2 not in the src tree
-	      $(TOP_DIR)/builds/unix/configure $(value CFG)
+#	      $(TOP_DIR)/builds/unix/configure $(value CFG)
     else
-	      cd builds/unix; ./configure $(value CFG)
+#	      cd builds/unix; ./configure $(value CFG)
     endif
   endif
 
