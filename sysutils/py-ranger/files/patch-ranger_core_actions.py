--- ranger/core/actions.py	2012-09-23 13:53:41.303318949 +0800
+++ ranger/core/actions.py	2012-09-23 13:56:22.362323041 +0800
@@ -1053,7 +1053,7 @@
 				descr = "moving: " + one_file.path
 			else:
 				descr = "moving files from: " + one_file.dirname
-			obj = CommandLoader(args=['mv'] + mv_flags \
+			obj = CommandLoader(args=['gmv'] + mv_flags \
 					+ [f.path for f in copied_files] \
 					+ [cwd.path], descr=descr)
 		else:
@@ -1065,10 +1065,10 @@
 					and one_file.dirname == cwd.path:
 				# Special case: yypp
 				# copying a file onto itself -> create a backup
-				obj = CommandLoader(args=['cp', '-f'] + cp_flags \
+				obj = CommandLoader(args=['gcp', '-f'] + cp_flags \
 						+ [one_file.path, one_file.path], descr=descr)
 			else:
-				obj = CommandLoader(args=['cp'] + cp_flags \
+				obj = CommandLoader(args=['gcp'] + cp_flags \
 						+ [f.path for f in copied_files] \
 						+ [cwd.path], descr=descr)
 
