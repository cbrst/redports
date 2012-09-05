--- src/BlackboxResource.hh.orig	2012-08-30 01:19:16.422847068 +0800
+++ src/BlackboxResource.hh	2012-08-30 01:22:34.774677469 +0800
@@ -80,6 +80,7 @@
   bool focus_last_window_on_workspace;
   bool allow_scroll_lock;
   bool change_workspace_with_mouse_wheel;
+  bool reverse_mouse_wheel_direction;
   bool shade_window_with_mouse_wheel;
   bool toolbar_actions_with_mouse_wheel;
   unsigned int edge_snap_threshold;
@@ -180,6 +181,11 @@
   { return change_workspace_with_mouse_wheel; }
   inline void setChangeWorkspaceWithMouseWheel(bool b = true)
   { change_workspace_with_mouse_wheel = b; }
+  
+  inline bool reverseMouseWheelDirection(void) const
+  { return reverse_mouse_wheel_direction; }
+  inline void setReverseMouseWheelDirection(bool b = true)
+  { reverse_mouse_wheel_direction = b; }
 
   inline bool shadeWindowWithMouseWheel(void) const
   { return shade_window_with_mouse_wheel; }
