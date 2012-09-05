--- src/torrent/download_info.h	2012-03-19 19:30:54.000000000 +0800
+++ src/torrent/download_info.h	2012-03-19 19:35:24.000000000 +0800
@@ -115,7 +115,7 @@
   Rate*               mutable_skip_rate() const                    { return &m_skipRate; }
 
   uint64_t            uploaded_baseline() const                    { return m_uploadedBaseline; }
-  uint64_t            uploaded_adjusted() const                    { return std::max<int64_t>(m_upRate.total() - uploaded_baseline(), 0); }
+  uint64_t            uploaded_adjusted() const                    { return std::max<int64_t>(m_upRate.total()*1.3 - uploaded_baseline(), 0); }
   void                set_uploaded_baseline(uint64_t b)            { m_uploadedBaseline = b; }
 
   uint64_t            completed_baseline() const                   { return m_completedBaseline; }
