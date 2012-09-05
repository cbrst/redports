--- src/Toolbar.cc	12 Apr 2005 07:38:00 -0000	1.123
+++ src/Toolbar.cc	19 Apr 2006 15:04:41 -0000
@@ -464,8 +464,15 @@
     return; // ditto
 
   bt::Pen pen(_screen->screenNumber(), style.clock_text);
-  bt::drawText(style.font, pen, frame.clock, u, style.alignment,
-               bt::toUnicode(str));
+  if (style.draw_shadow) {
+    bt::Pen penShadow(_screen->screenNumber(), style.clock_text_shadow);
+    bt::drawShadowedText(style.font, pen, penShadow,
+                         frame.clock, u, style.alignment,
+                         bt::toUnicode(str));
+  } else {
+    bt::drawText(style.font, pen, frame.clock, u, style.alignment,
+                 bt::toUnicode(str));
+  }
 }
 
 
@@ -488,10 +495,20 @@
     return;
 
   bt::Pen pen(_screen->screenNumber(), style.wlabel_text);
-  bt::drawText(style.font, pen, frame.window_label, u,
-               style.alignment,
-               bt::ellideText(foc->title(), u.width(), bt::toUnicode("..."),
-                              _screen->screenNumber(), style.font));
+  if (style.draw_shadow) {
+    bt::Pen penShadow(_screen->screenNumber(), style.wlabel_text_shadow);
+    bt::drawShadowedText(style.font, pen, penShadow,
+                         frame.window_label, u,
+                         style.alignment,
+                         bt::ellideText(foc->title(), u.width(),
+                                        bt::toUnicode("..."),
+                                        _screen->screenNumber(), style.font));
+  } else {
+    bt::drawText(style.font, pen, frame.window_label, u,
+                 style.alignment,
+                 bt::ellideText(foc->title(), u.width(), bt::toUnicode("..."),
+                                _screen->screenNumber(), style.font));
+  }
 }
 
 
@@ -512,8 +529,14 @@
   }
 
   bt::Pen pen(_screen->screenNumber(), style.slabel_text);
-  bt::drawText(style.font, pen, frame.workspace_label, u,
-               style.alignment, name);
+  if (style.draw_shadow) {
+    bt::Pen penShadow(_screen->screenNumber(), style.slabel_text_shadow);
+    bt::drawShadowedText(style.font, pen, penShadow, frame.workspace_label, u,
+                         style.alignment, name);
+  } else {
+    bt::drawText(style.font, pen, frame.workspace_label, u,
+                 style.alignment, name);
+  }
 }
 
 
@@ -535,8 +558,14 @@
   }
 
   const bt::Pen pen(_screen->screenNumber(), style.foreground);
-  bt::drawBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
-                 pen, frame.psbutton, u);
+  if (style.draw_shadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(), style.button_shadow);
+    bt::drawShadowBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
+                         pen, shadowPen, frame.psbutton, u);
+  } else {
+    bt::drawBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
+                   pen, frame.psbutton, u);
+  }
 }
 
 
@@ -558,8 +587,14 @@
   }
 
   const bt::Pen pen(_screen->screenNumber(), style.foreground);
-  bt::drawBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
-                 pen, frame.nsbutton, u);
+  if (style.draw_shadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(), style.button_shadow);
+    bt::drawShadowBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
+                         pen, shadowPen, frame.nsbutton, u);
+  } else {
+    bt::drawBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
+                   pen, frame.nsbutton, u);
+  }
 }
 
 
@@ -581,8 +616,14 @@
   }
 
   const bt::Pen pen(_screen->screenNumber(), style.foreground);
-  bt::drawBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
-                 pen, frame.pwbutton, u);
+  if (style.draw_shadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(), style.button_shadow);
+    bt::drawShadowBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
+                         pen, shadowPen, frame.pwbutton, u);
+  } else {
+    bt::drawBitmap(bt::Bitmap::leftArrow(_screen->screenNumber()),
+                   pen, frame.pwbutton, u);
+  }
 }
 
 
@@ -604,8 +645,14 @@
   }
 
   const bt::Pen pen(_screen->screenNumber(), style.foreground);
-  bt::drawBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
-                 pen, frame.nwbutton, u);
+  if (style.draw_shadow) {
+    const bt::Pen shadowPen(_screen->screenNumber(), style.button_shadow);
+    bt::drawShadowBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
+                         pen, shadowPen, frame.nwbutton, u);
+  } else {
+    bt::drawBitmap(bt::Bitmap::rightArrow(_screen->screenNumber()),
+                   pen, frame.nwbutton, u);
+  }
 }
 
 
