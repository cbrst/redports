--- src/Screen.cc.orig	2012-08-30 01:29:57.618854592 +0800
+++ src/Screen.cc	2012-08-30 01:39:27.702852698 +0800
@@ -1780,8 +1780,16 @@
   const bt::Pen pen(screen_info.screenNumber(), style.focus.text);
   bt::drawTexture(screen_info.screenNumber(), texture, geom_window,
                   u, u, geom_pixmap);
-  bt::drawText(style.font, pen, geom_window, t, style.alignment,
-               bt::toUnicode(label));
+  if (style.focus.draw_shadow) {
+    const bt::Pen penShadow(screen_info.screenNumber(),
+                            style.focus.text_shadow);
+    bt::drawShadowedText(style.font, pen, penShadow,
+                         geom_window, t, style.alignment,
+                         bt::toUnicode(label));
+  } else {
+    bt::drawText(style.font, pen, geom_window, t, style.alignment,
+                 bt::toUnicode(label));
+  }
 }
 
 
@@ -1870,11 +1878,12 @@
 
   if (event->message_type == _blackbox->ewmh().numberOfDesktops()) {
     unsigned int number = event->data.l[0];
-    if (number > workspaceCount()) {
-      for (; number != workspaceCount(); --number)
+    unsigned int wsCount = workspaceCount();
+    if (number > wsCount) {
+      for (; number != wsCount; --number)
         addWorkspace();
-    } else if (number < workspaceCount()) {
-      for (; number != workspaceCount(); ++number)
+    } else if (number < wsCount) {
+      for (; number != wsCount; ++number)
         removeLastWorkspace();
     }
   } else if (event->message_type == _blackbox->ewmh().desktopNames()) {
@@ -1895,15 +1904,20 @@
   */
   _blackbox->setActiveScreen(this);
 
+  unsigned short int upbutton = (_blackbox->resource().
+  reverseMouseWheelDirection() ? 5 : 4);
+  unsigned short int downbutton = (_blackbox->resource().
+  reverseMouseWheelDirection() ? 4 : 5);
+
   if (event->button == 2) {
     _workspacemenu->popup(event->x_root, event->y_root);
   } else if (event->button == 3) {
     _blackbox->checkMenu();
     _rootmenu->popup(event->x_root, event->y_root);
-  } else if (event->button == 4 &&
+  } else if (event->button == upbutton &&
              _blackbox->resource().changeWorkspaceWithMouseWheel()) {
     nextWorkspace();
-  } else if (event->button == 5 &&
+  } else if (event->button == downbutton &&
              _blackbox->resource().changeWorkspaceWithMouseWheel()) {
     prevWorkspace();
   }
