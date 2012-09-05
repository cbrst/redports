--- src/ScreenResource.cc	13 Apr 2005 05:54:08 -0000	1.13
+++ src/ScreenResource.cc	19 Apr 2006 15:04:40 -0000
@@ -270,6 +270,14 @@
                           res.read("window.label.focus.textColor",
                                    "Window.Label.Focus.TextColor",
                                    "black"));
+  _windowStyle.focus.draw_shadow =
+    res.read("window.label.unfocus.drawShadow",
+             "Window.Label.Unfocus.DrawShadow", false);
+  _windowStyle.focus.text_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("window.label.focus.textShadowColor",
+                                   "Window.Label.Focus.TextShadowColor",
+                                   "black"));
   _windowStyle.focus.foreground =
     bt::Color::namedColor(display, screen_num,
                           res.read("window.button.focus.foregroundColor",
@@ -277,6 +285,19 @@
                                    res.read("window.button.focus.picColor",
                                             "Window.Button.Focus.PicColor",
                                             "black")));
+  _windowStyle.focus.button_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("window.button.focus.shadowColor",
+                                   "Window.Button.Focus.ShadowColor",
+                                   res.read("window.button.focus.picShadowColor",
+                                            "Window.Button.Focus.PicShadowColor",
+                                            "black")));
+  _windowStyle.focus.draw_shadow =
+    res.read("window.button.focus.drawShadow",
+             "Window.Button.Focus.drawShadow",
+             res.read("window.button.focus.drawPicShadow",
+                      "Window.Button.Focus.drawPicShadow",
+                      false));
   _windowStyle.focus.title =
     bt::textureResource(display, screen_num, res,
                         "window.title.focus",
@@ -314,6 +335,14 @@
                           res.read("window.label.unfocus.textColor",
                                    "Window.Label.Unfocus.TextColor",
                                    "white"));
+  _windowStyle.unfocus.draw_shadow =
+    res.read("window.label.unfocus.drawShadow",
+             "Window.Label.Unfocus.DrawShadow", false);
+  _windowStyle.unfocus.text_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("window.label.unfocus.textShadowColor",
+                                   "Window.Label.Unfocus.TextShadowColor",
+                                   "black"));
   _windowStyle.unfocus.foreground =
     bt::Color::namedColor(display, screen_num,
                           res.read("window.button.unfocus.foregroundColor",
@@ -321,6 +350,19 @@
                                    res.read("window.button.unfocus.picColor",
                                             "Window.Button.Unfocus.PicColor",
                                             "white")));
+  _windowStyle.unfocus.button_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("window.button.unfocus.shadowColor",
+                                   "Window.Button.Unfocus.ShadowColor",
+                                   res.read("window.button.unfocus.picShadowColor",
+                                            "Window.Button.Unfocus.PicShadowColor",
+                                            "black")));
+  _windowStyle.unfocus.draw_shadow =
+    res.read("window.button.unfocus.drawShadow",
+             "Window.Button.Unfocus.drawShadow",
+             res.read("window.button.unfocus.drawPicShadow",
+                      "Window.Button.Unfocus.drawPicShadow",
+                      false));
   _windowStyle.unfocus.title =
     bt::textureResource(display, screen_num, res,
                         "window.title.unfocus",
@@ -444,16 +486,31 @@
                           res.read("toolbar.label.textColor",
                                    "Toolbar.Label.TextColor",
                                    "black"));
+  _toolbarStyle.slabel_text_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("toolbar.label.textShadowColor",
+                                   "Toolbar.Label.TextShadowColor",
+                                   "white"));
   _toolbarStyle.wlabel_text =
     bt::Color::namedColor(display, screen_num,
                           res.read("toolbar.windowLabel.textColor",
                                    "Toolbar.Label.TextColor",
                                    "black"));
+  _toolbarStyle.wlabel_text_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("toolbar.windowLabel.textShadowColor",
+                                   "Toolbar.WindowLabel.TextShadowColor",
+                                   "white"));
   _toolbarStyle.clock_text =
     bt::Color::namedColor(display, screen_num,
                           res.read("toolbar.clock.textColor",
                                    "Toolbar.Label.TextColor",
                                    "black"));
+  _toolbarStyle.clock_text_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("toolbar.clock.textShadowColor",
+                                   "Toolbar.Clock.TextShadowColor",
+                                   "black"));
   _toolbarStyle.foreground =
     bt::Color::namedColor(display, screen_num,
                           res.read("toolbar.button.foregroundColor",
@@ -461,6 +518,19 @@
                                    res.read("toolbar.button.picColor",
                                             "Toolbar.Button.PicColor",
                                             "black")));
+  _toolbarStyle.button_shadow =
+    bt::Color::namedColor(display, screen_num,
+                          res.read("toolbar.button.shadowColor",
+                                   "Toolbar.Button.ShadowColor",
+                                   res.read("toolbar.button.picShadowColor",
+                                            "Toolbar.Button.PicShadowColor",
+                                            "black")));
+  _toolbarStyle.draw_shadow =
+    res.read("toolbar.button.drawShadow",
+             "Toolbar.Button.DrawShadow",
+             res.read("toolbar.button.drawShadow",
+                      "Toolbar.Button.DrawShadow",
+                      false));
   _toolbarStyle.alignment =
     bt::alignResource(res, "toolbar.alignment", "Toolbar.Alignment");
 
@@ -471,6 +541,9 @@
   _toolbarStyle.button_margin =
     res.read("toolbar.button.marginWidth", "Toolbar.Button.MarginWidth", 2);
 
+  _toolbarStyle.draw_shadow =
+    res.read("toolbar.drawShadow", "Toolbar.DrawShadow", false);
+
   const bt::Bitmap &left = bt::Bitmap::leftArrow(screen_num),
                   &right = bt::Bitmap::rightArrow(screen_num);
   _toolbarStyle.button_width =
