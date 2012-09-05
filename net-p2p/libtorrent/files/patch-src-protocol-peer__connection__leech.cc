--- src/protocol/peer_connection_leech.cc	2012-03-19 19:30:55.000000000 +0800
+++ src/protocol/peer_connection_leech.cc	2012-03-19 19:39:18.000000000 +0800
@@ -109,7 +109,7 @@
 template<Download::ConnectionType type>
 bool
 PeerConnection<type>::receive_keepalive() {
-  if (cachedTime - m_timeLastRead > rak::timer::from_seconds(240))
+  if (cachedTime - m_timeLastRead > rak::timer::from_seconds(40))
     return false;
 
   // There's no point in adding ourselves to the write poll if the
@@ -148,7 +148,7 @@
   // Do we also need to remove from download throttle? Check how it
   // worked again.
 
-  if (!download_queue()->canceled_empty() && m_downStall >= 6)
+  if (!download_queue()->canceled_empty() && m_downStall >= 2)
     download_queue()->cancel();
   else if (!download_queue()->queued_empty() && m_downStall++ != 0)
     download_queue()->stall();
