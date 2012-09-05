--- infinality/infctl.sh.orig	2012-09-06 02:46:49.459031622 +0800
+++ infinality/infctl.sh	2012-09-06 02:47:07.752016584 +0800
@@ -1,6 +1,6 @@
-#!/bin/bash
+#!/usr/bin/env bash
 
-cd /etc/fonts/infinality
+cd /usr/local/etc/fonts/infinality
 
 stylesdir=styles.conf.avail
 styles=`/bin/ls $stylesdir`
