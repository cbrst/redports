--- src/Configmenu.cc.orig	2012-08-30 01:24:39.208840400 +0800
+++ src/Configmenu.cc	2012-08-30 01:28:02.626918885 +0800
@@ -86,6 +86,7 @@
   FocusNewWindows,
   FocusLastWindowOnWorkspace,
   ChangeWorkspaceWithMouseWheel,
+  ReverseMouseWheelDirection,
   ShadeWindowWithMouseWheel,
   ToolbarActionsWithMouseWheel,
   DisableBindings,
@@ -126,6 +127,8 @@
              FocusLastWindowOnWorkspace);
   insertItem(bt::toUnicode("Change Workspace with Mouse Wheel"),
              ChangeWorkspaceWithMouseWheel);
+  insertItem(bt::toUnicode("Reverse Mouse Wheel Direction"),
+             ReverseMouseWheelDirection);
   insertItem(bt::toUnicode("Shade Windows with Mouse Wheel"),
              ShadeWindowWithMouseWheel);
   insertItem(bt::toUnicode("Toolbar Actions with Mouse Wheel"),
@@ -148,6 +151,8 @@
   setItemChecked(FocusLastWindowOnWorkspace, res.focusLastWindowOnWorkspace());
   setItemChecked(ChangeWorkspaceWithMouseWheel,
                  res.changeWorkspaceWithMouseWheel());
+  setItemChecked(ReverseMouseWheelDirection,
+                 res.reverseMouseWheelDirection());
   setItemChecked(ShadeWindowWithMouseWheel,
                  res.shadeWindowWithMouseWheel());
   setItemChecked(ToolbarActionsWithMouseWheel,
@@ -182,6 +187,10 @@
   case ChangeWorkspaceWithMouseWheel:
     res.setChangeWorkspaceWithMouseWheel(!res.changeWorkspaceWithMouseWheel());
     break;
+    
+  case ReverseMouseWheelDirection:
+    res.setReverseMouseWheelDirection(!res.reverseMouseWheelDirection());
+    break;
 
   case ShadeWindowWithMouseWheel:
     res.setShadeWindowWithMouseWheel(!res.shadeWindowWithMouseWheel());
