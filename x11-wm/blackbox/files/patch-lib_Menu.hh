--- lib/Menu.hh	24 Jan 2005 09:06:11 -0000	1.29
+++ lib/Menu.hh	19 Apr 2006 15:04:35 -0000
@@ -151,6 +151,8 @@
       Texture texture;
       Color foreground;
       Color text;
+      Color textShadow;
+      bool drawShadow;
       Font font;
       Alignment alignment;
     } title;
@@ -158,6 +160,8 @@
       Texture texture;
       Color foreground;
       Color text;
+      Color textShadow;
+      bool drawShadow;
       Color disabled;
       Font font;
       Alignment alignment;
@@ -166,6 +170,8 @@
       Texture texture;
       Color foreground;
       Color text;
+      Color textShadow;
+      bool drawShadow;
       // font and alignment used from frame above
     } active;
     unsigned int title_margin, frame_margin, item_indent;

