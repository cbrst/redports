--- src/torrent/tracker.cc	2012-03-19 19:30:54.000000000 +0800
+++ src/torrent/tracker.cc	2012-03-19 19:38:06.000000000 +0800
@@ -46,7 +46,7 @@
   m_group(0),
   m_url(url),
 
-  m_normalInterval(1800),
+  m_normalInterval(600),
   m_minInterval(0),
 
   m_scrapeTimeLast(0),
