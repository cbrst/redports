--- src/ScreenResource.hh	23 Feb 2005 06:01:15 -0000	1.2
+++ src/ScreenResource.hh	19 Apr 2006 15:04:40 -0000
@@ -51,8 +51,9 @@
 
 struct WindowStyle {
   struct {
-    bt::Color text, foreground, frame_border;
+    bt::Color text, text_shadow, foreground, button_shadow, frame_border;
     bt::Texture title, label, button, handle, grip;
+    bool draw_shadow;
   } focus, unfocus;
   bt::Alignment alignment;
   bt::Bitmap iconify, maximize, restore, close;
@@ -61,18 +62,22 @@
   unsigned int title_margin, label_margin, button_margin,
     frame_border_width, handle_height;
 
+
   // calculated
   unsigned int title_height, label_height, button_width, grip_width;
 };
 
 struct ToolbarStyle {
   bt::Bitmap left, right;
-  bt::Color slabel_text, wlabel_text, clock_text, foreground;
+  bt::Color slabel_text, wlabel_text, clock_text, foreground, button_shadow;
+  bt::Color slabel_text_shadow, wlabel_text_shadow, clock_text_shadow;
   bt::Texture toolbar, slabel, wlabel, clock, button, pressed;
   bt::Font font;
   bt::Alignment alignment;
   unsigned int frame_margin, label_margin, button_margin;
 
+  bool draw_shadow;
+
   // calculated extents
   unsigned int toolbar_height, label_height, button_width, hidden_height;
 };
