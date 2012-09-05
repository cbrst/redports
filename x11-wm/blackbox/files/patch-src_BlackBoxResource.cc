--- src/BlackboxResource.cc.orig	2012-08-30 01:13:07.047833065 +0800
+++ src/BlackboxResource.cc	2012-08-30 01:16:30.852871834 +0800
@@ -196,6 +196,10 @@
     res.read("session.changeWorkspaceWithMouseWheel",
              "session.changeWorkspaceWithMouseWheel",
              true);
+  reverse_mouse_wheel_direction =
+    res.read("session.reverseMouseWheelDirection",
+             "session.reverseMouseWheelDirection",
+             false);
   shade_window_with_mouse_wheel =
     res.read("session.shadeWindowWithMouseWheel",
              "session.shadeWindowWithMouseWheel",
@@ -310,6 +314,8 @@
   res.write("session.focusLastWindow", focus_last_window_on_workspace);
   res.write("session.changeWorkspaceWithMouseWheel",
             change_workspace_with_mouse_wheel);
+  res.write("session.reverseMouseWheelDirection",
+            reverse_mouse_wheel_direction);
   res.write("session.shadeWindowWithMouseWheel",
             shade_window_with_mouse_wheel);
   res.write("session.toolbarActionsWithMouseWheel",
