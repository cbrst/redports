--- lib/Bitmap.hh	3 Jan 2005 09:42:38 -0000	1.4
+++ lib/Bitmap.hh	19 Apr 2006 15:04:32 -0000
@@ -43,6 +43,11 @@
   void drawBitmap(const Bitmap &bitmap, const Pen &pen,
                   ::Drawable drawable, const Rect &rect);
 
+  void drawShadowBitmap(const bt::Bitmap &bitmap, const bt::Pen &pen,
+                        const bt::Pen &shadowPen,
+                        ::Drawable drawable, const bt::Rect &rect);
+
+
   /*
     The bitmap object.  You can create custom bitmaps from raw data,
     or use one of 5 standard bitmaps.
