--- lib/Font.cc	7 Apr 2005 15:28:41 -0000	1.42
+++ lib/Font.cc	19 Apr 2006 15:04:33 -0000
@@ -542,6 +542,15 @@
                 str.c_str(), str.length());
 }
 
+void bt::drawShadowedText(const Font &font, const Pen &pen1, const Pen &pen2,
+                          Drawable drawable, const Rect &rect,
+                          Alignment alignment, const ustring &text) {
+  bt::Rect shadowRect = rect;
+  shadowRect.setPos(rect.x() + 1, rect.y() + 1);
+  drawText(font, pen2, drawable, shadowRect, alignment, text);
+  drawText(font, pen1, drawable, rect, alignment, text);
+}
+
 
 bt::ustring bt::ellideText(const bt::ustring &text, size_t count,
                            const bt::ustring &ellide) {
