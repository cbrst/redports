--- src/torrent/tracker.h	2012-03-19 19:30:54.000000000 +0800
+++ src/torrent/tracker.h	2012-03-19 19:38:33.000000000 +0800
@@ -98,7 +98,7 @@
 
   void                set_group(uint32_t v)                 { m_group = v; }
 
-  void                set_normal_interval(int v)            { if (v >= 60 && v <= 3600) m_normalInterval = v; }
+  void                set_normal_interval(int v)            { if (v >= 60 && v <= 600) m_normalInterval = v; }
   void                set_min_interval(int v)               { if (v >= 0 && v <= 600)   m_minInterval = v; }
 
   bool                m_enabled;
