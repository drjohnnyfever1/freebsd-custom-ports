--- ./platform/gfx/harfbuzz/src/hb-blob.cc
+++ ./platform/gfx/harfbuzz/src/hb-blob.cc
@@ -25,6 +25,8 @@
  */

+#ifndef __FreeBSD__
 /* http://www.oracle.com/technetwork/articles/servers-storage-dev/standardheaderfiles-453865.html */
 #ifndef _POSIX_C_SOURCE
 #define _POSIX_C_SOURCE 199309L
 #endif
+#endif

 #include "hb-private.hh"

 #include "hb-object-private.hh"
