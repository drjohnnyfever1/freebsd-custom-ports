--- ./platform/memory/mozjemalloc/jemalloc.c
+++ ./platform/memory/mozjemalloc/jemalloc.c
@@ -174,6 +174,9 @@
 #endif

 #include <sys/types.h>
+#ifdef MOZ_MEMORY_BSD
+#include <sys/sysctl.h>
+#endif

 #include <errno.h>
 #include <stdlib.h>
