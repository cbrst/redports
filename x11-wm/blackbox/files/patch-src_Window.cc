--- src/Window.cc	6 Dec 2005 09:06:17 -0000	1.406
+++ src/Window.cc	19 Apr 2006 15:04:46 -0000
@@ -2773,8 +2773,19 @@
               u.top() + style.label_margin,
               u.right() - style.label_margin,
               u.bottom() - style.label_margin);
-  bt::drawText(style.font, pen, frame.label, u,
-               style.alignment, client.visible_title);
+  bool draw_shadow = (client.state.focused) ?
+    style.focus.draw_shadow : style.unfocus.draw_shadow;
+  if (draw_shadow) {
+    const bt::Pen penShadow(_screen->screenNumber(),
+                            ((client.state.focused)
+                             ? style.focus.text_shadow
+                             : style.unfocus.text_shadow));
+    bt::drawShadowedText(style.font, pen, penShadow, frame.label, u,
+                         style.alignment, client.visible_title);
+  } else {
+    bt::drawText(style.font, pen, frame.label, u,
+                 style.alignment, client.visible_title);
+  }
 }
 
 
@@ -2812,7 +2823,19 @@
                     (client.state.focused
                      ? style.focus.foreground
                      : style.unfocus.foreground));
-  bt::drawBitmap(style.iconify, pen, frame.iconify_button, u);
+  bool drawShadow =
+    client.state.focused? style.focus.draw_shadow : style.unfocus.draw_shadow;
+   if (drawShadow) {
+     const bt::Pen shadowPen(_screen->screenNumber(),
+                             client.state.focused
+                             ? style.focus.button_shadow
+                             : style.unfocus.button_shadow);
+     bt::drawShadowBitmap(style.iconify, pen, shadowPen,
+                          frame.iconify_button, u);
+   } else {
+     bt::drawBitmap(style.iconify, pen, frame.iconify_button, u);
+   }
+
 }
 
 
@@ -2847,8 +2870,20 @@
                     (client.state.focused
                      ? style.focus.foreground
                      : style.unfocus.foreground));
-  bt::drawBitmap(isMaximized() ? style.restore : style.maximize,
-                 pen, frame.maximize_button, u);
+
+  bool drawShadow =
+    client.state.focused ? style.focus.draw_shadow: style.unfocus.draw_shadow;
+  if (drawShadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(),
+                            client.state.focused
+                            ? style.focus.button_shadow
+                            : style.unfocus.button_shadow);
+    bt::drawShadowBitmap(isMaximized() ? style.restore : style.maximize,
+                         pen, shadowPen, frame.maximize_button, u);
+  } else {
+    bt::drawBitmap(isMaximized() ? style.restore : style.maximize, pen,
+                   frame.maximize_button, u);
+  }
 }
 
 
@@ -2880,7 +2915,17 @@
                     (client.state.focused
                      ? style.focus.foreground
                      : style.unfocus.foreground));
-  bt::drawBitmap(style.close, pen, frame.close_button, u);
+  bool drawShadow =
+    client.state.focused ? style.focus.draw_shadow : style.unfocus.draw_shadow;
+  if (drawShadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(),
+                            client.state.focused
+                            ? style.focus.button_shadow
+                            : style.unfocus.button_shadow);
+    bt::drawShadowBitmap(style.close, pen, shadowPen, frame.close_button, u);
+  } else {
+    bt::drawBitmap(style.close, pen, frame.close_button, u);
+  }
 }
 
 
