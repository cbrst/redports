--- lib/Bitmap.cc	3 Jan 2005 09:42:38 -0000	1.4
+++ lib/Bitmap.cc	19 Apr 2006 15:04:32 -0000
@@ -145,6 +145,17 @@
   XSetClipMask(pen.XDisplay(), pen.gc(), None);
 }
 
+void bt::drawShadowBitmap(const bt::Bitmap &bitmap, const bt::Pen &pen,
+                          const bt::Pen &shadowPen,
+                          ::Drawable drawable, const bt::Rect &rect) {
+  bt::Rect shadowRect(rect);
+  shadowRect.setX(1);
+  shadowRect.setY(1);
+  drawBitmap(bitmap, shadowPen, drawable, shadowRect);
+  drawBitmap(bitmap, pen, drawable, rect);
+}
+
+
 bt::Bitmap::Bitmap(unsigned int scr, const unsigned char *data,
                    unsigned int w, unsigned int h)
   : _screen(~0u), _drawable(0ul), _width(0u), _height(0u)
