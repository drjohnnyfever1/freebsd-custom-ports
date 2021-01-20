--- ./platform/ipc/chromium/src/third_party/libevent/evutil_rand.c
+++ ./platform/ipc/chromium/src/third_party/libevent/evutil_rand.c
@@ -139,7 +139,7 @@ evutil_secure_rng_get_bytes(void *buf, size_t n)
 	ev_arc4random_buf(buf, n);
 }

-#if !defined(__OpenBSD__) && !defined(ANDROID) && !defined(__sun__)
+#if !defined(__OpenBSD__) && !defined(__FreeBSD__) && !defined(ANDROID) && !defined(__sun__)
 void
 evutil_secure_rng_add_bytes(const char *buf, size_t n)
 {
