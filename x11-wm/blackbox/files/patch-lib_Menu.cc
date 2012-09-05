--- lib/Menu.cc	27 Jan 2005 17:08:22 -0000	1.69
+++ lib/Menu.cc	19 Apr 2006 15:04:34 -0000
@@ -87,6 +87,14 @@
                       resource.read("menu.title.textColor",
                                     "Menu.Title.TextColor",
                                     "white"));
+  title.textShadow =
+    Color::namedColor(_app.display(), _screen,
+                      resource.read("menu.title.textShadowColor",
+                                    "Menu.Title.TextShadowColor",
+                                    "white"));
+  title.drawShadow = resource.read("menu.title.drawShadow",
+                                   "Menu.Title.DrawShadow",
+                                    false);
   frame.foreground =
     Color::namedColor(_app.display(), _screen,
                       resource.read("menu.frame.foregroundColor",
@@ -97,6 +105,14 @@
                       resource.read("menu.frame.textColor",
                                     "Menu.Frame.TextColor",
                                     "black"));
+  frame.textShadow =
+    Color::namedColor(_app.display(), _screen,
+                      resource.read("menu.frame.textShadowColor",
+                                    "Menu.Frame.TextShadowColor",
+                                    "black"));
+  frame.drawShadow = resource.read("menu.frame.drawShadow",
+                                   "Menu.Frame.DrawShadow",
+                                    false);
   frame.disabled =
     Color::namedColor(_app.display(), _screen,
                       resource.read("menu.frame.disabledColor",
@@ -112,6 +128,14 @@
                       resource.read("menu.active.textColor",
                                     "Menu.Active.TextColor",
                                     "white"));
+  active.textShadow =
+    Color::namedColor(_app.display(), _screen,
+                      resource.read("menu.active.textShadowColor",
+                                    "Menu.Active.TextShadowColor",
+                                    "white"));
+  active.drawShadow = resource.read("menu.active.drawShadow",
+                                    "Menu.Active.DrawShadow",
+                                    false);
 
   // fonts
   title.font.setFontName(resource.read("menu.title.font", "Menu.Title.Font"));
@@ -186,7 +210,14 @@
   Rect r;
   r.setCoords(rect.left() + titleMargin(), rect.top(),
               rect.right() - titleMargin(), rect.bottom());
-  drawText(title.font, pen, window, r, title.alignment, text);
+
+  if (title.drawShadow) {
+    Pen penShadow(_screen, title.textShadow);
+    drawShadowedText(title.font, pen, penShadow,
+                     window, r, title.alignment, text);
+  } else {
+    drawText(title.font, pen, window, r, title.alignment, text);
+  }
 }
 
 
@@ -209,9 +240,17 @@
                                          frame.foreground) : frame.disabled));
   Pen tpen(_screen, (item.isEnabled() ? (item.isActive() ? active.text :
                                          frame.text) : frame.disabled));
+  bool drawShadow = (item.isActive() ? active.drawShadow : frame.drawShadow);
   if (item.isActive() && item.isEnabled())
     drawTexture(_screen, active.texture, window, rect, rect, pixmap);
-  drawText(frame.font, tpen, window, r2, frame.alignment, item.label());
+  if (drawShadow) {
+    Pen tpenShadow(_screen, 
+                   (item.isActive() ? active.textShadow : frame.textShadow));
+    drawShadowedText(frame.font, tpen, tpenShadow,
+                     window, r2, frame.alignment, item.label());
+  } else {
+    drawText(frame.font, tpen, window, r2, frame.alignment, item.label());
+  }
 
   if (item.isChecked()) {
     drawBitmap(bt::Bitmap::checkMark(_screen), fpen, window,
