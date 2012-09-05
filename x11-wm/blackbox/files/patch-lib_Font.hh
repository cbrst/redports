--- lib/Font.hh	7 Apr 2005 15:28:41 -0000	1.23
+++ lib/Font.hh	19 Apr 2006 15:04:33 -0000
@@ -54,6 +54,9 @@
   Rect textRect(unsigned int screen, const Font &font,
                 const bt::ustring &text);
 
+  void drawShadowedText(const Font &font, const Pen &pen1, const Pen &pen2,
+                        Drawable drawable, const Rect &rect,
+                        Alignment alignment, const ustring &text);
   void drawText(const Font &font, const Pen &pen,
                 Drawable drawable, const Rect &rect,
                 Alignment alignment, const ustring &text);

