--- src/download/download_wrapper.cc	2012-03-19 19:30:54.000000000 +0800
+++ src/download/download_wrapper.cc	2012-03-19 19:37:28.000000000 +0800
@@ -272,7 +272,7 @@
     return;
 
   // Every 2 minutes.
-  if (ticks % 4 == 0) {
+  if (ticks % 1 == 0) {
     if (info()->is_active()) {
       if (info()->is_pex_enabled()) {
         m_main->do_peer_exchange();
