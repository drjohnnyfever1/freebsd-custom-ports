--- platform/config/gcc-stl-wrapper.template.h  2020-12-16 01:42:37.000000000 -0600
+++ platform/config/gcc-stl-wrapper.template.h  2021-01-20 12:18:37.835554000 -0600
@@ -32,6 +32,14 @@
 #  define moz_dont_include_mozalloc_for_cstdlib
 #endif

+#ifndef moz_dont_include_mozalloc_for_type_traits
+#  define moz_dont_include_mozalloc_for_type_traits
+#endif
+
+#ifndef moz_dont_include_mozalloc_for_limits
+#  define moz_dont_include_mozalloc_for_limits
+#endif
+
 // Include mozalloc after the STL header and all other headers it includes
 // have been preprocessed.
 #if !defined(MOZ_INCLUDE_MOZALLOC_H) && \
