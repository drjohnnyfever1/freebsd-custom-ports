--- ./platform/python/psutil/psutil/_psutil_bsd.c
+++ ./platform/python/psutil/psutil/_psutil_bsd.c
@@ -12,6 +12,7 @@
 #include <errno.h>
 #include <stdlib.h>
 #include <stdio.h>
+#include <err.h>
 #include <signal.h>
 #include <fcntl.h>
 #include <paths.h>
@@ -38,7 +39,7 @@
 #include <netinet/tcp_fsm.h>   // for TCP connection states
 #include <arpa/inet.h>         // for inet_ntop()

-#if __FreeBSD_version < 900000
+#if !defined(__FreeBSD_version)
 #include <utmp.h>         // system users
 #else
 #include <utmpx.h>
@@ -601,11 +602,7 @@ psutil_virtual_mem(PyObject *self, PyObject *args)
     struct vmtotal vm;
     int            mib[] = {CTL_VM, VM_METER};
     long           pagesize = getpagesize();
-#if __FreeBSD_version > 702101
     long buffers;
-#else
-    int buffers;
-#endif
     size_t buffers_size = sizeof(buffers);

     if (sysctlbyname("vm.stats.vm.v_page_count", &total, &size, NULL, 0))
@@ -724,13 +721,13 @@ psutil_cpu_times(PyObject *self, PyObject *args)

 /*
  * XXX
- * These functions are available on FreeBSD 8 only.
+ * These functions were seen available on FreeBSD only.
  * In the upper python layer we do various tricks to avoid crashing
  * and/or to provide alternatives where possible.
  */


-#if defined(__FreeBSD_version) && __FreeBSD_version >= 800000
+#if defined(__FreeBSD_version)
 /*
  * Return files opened by process as a list of (path, fd) tuples.
  * TODO: this is broken as it may report empty paths. 'procstat'
@@ -943,11 +940,19 @@ psutil_sockaddr_matches(int family, int port, void *pcb_addr,
                    psutil_sockaddr_addrlen(family)) == 0);
 }

+#if __FreeBSD_version >= 1200026
+static struct xtcpcb *
+psutil_search_tcplist(char *buf, struct kinfo_file *kif)
+{
+    struct xtcpcb *tp;
+    struct xinpcb *inp;
+#else
 static struct tcpcb *
 psutil_search_tcplist(char *buf, struct kinfo_file *kif)
 {
     struct tcpcb *tp;
     struct inpcb *inp;
+#endif
     struct xinpgen *xig, *oxig;
     struct xsocket *so;

@@ -955,9 +960,15 @@ psutil_search_tcplist(char *buf, struct kinfo_file *kif)
     for (xig = (struct xinpgen *)((char *)xig + xig->xig_len);
             xig->xig_len > sizeof(struct xinpgen);
             xig = (struct xinpgen *)((char *)xig + xig->xig_len)) {
+#if __FreeBSD_version >= 1200026
+        tp = (struct xtcpcb *)xig;
+        inp = &tp->xt_inp;
+        so = &inp->xi_socket;
+#else
         tp = &((struct xtcpcb *)xig)->xt_tp;
         inp = &((struct xtcpcb *)xig)->xt_inp;
         so = &((struct xtcpcb *)xig)->xt_socket;
+#endif

         if (so->so_type != kif->kf_sock_type ||
                 so->xso_family != kif->kf_sock_domain ||
@@ -967,20 +978,36 @@ psutil_search_tcplist(char *buf, struct kinfo_file *kif)
         if (kif->kf_sock_domain == AF_INET) {
             if (!psutil_sockaddr_matches(
                     AF_INET, inp->inp_lport, &inp->inp_laddr,
+#if __FreeBSD_version < 1200031
                     &kif->kf_sa_local))
+#else
+                    &kif->kf_un.kf_sock.kf_sa_local))
+#endif
                 continue;
             if (!psutil_sockaddr_matches(
                     AF_INET, inp->inp_fport, &inp->inp_faddr,
+#if __FreeBSD_version < 1200031
                     &kif->kf_sa_peer))
+#else
+                    &kif->kf_un.kf_sock.kf_sa_peer))
+#endif
                 continue;
         } else {
             if (!psutil_sockaddr_matches(
                     AF_INET6, inp->inp_lport, &inp->in6p_laddr,
+#if __FreeBSD_version < 1200031
                     &kif->kf_sa_local))
+#else
+                    &kif->kf_un.kf_sock.kf_sa_local))
+#endif
                 continue;
             if (!psutil_sockaddr_matches(
                     AF_INET6, inp->inp_fport, &inp->in6p_faddr,
+#if __FreeBSD_version < 1200031
                     &kif->kf_sa_peer))
+#else
+                    &kif->kf_un.kf_sock.kf_sa_peer))
+#endif
                 continue;
         }

@@ -1005,7 +1032,11 @@ psutil_proc_connections(PyObject *self, PyObject *args)
     struct kinfo_file *freep = NULL;
     struct kinfo_file *kif;
     char *tcplist = NULL;
+#if __FreeBSD_version >= 1200026
+    struct xtcpcb *tcp;
+#else
     struct tcpcb *tcp;
+#endif

     PyObject *retList = PyList_New(0);
     PyObject *tuple = NULL;
@@ -1074,19 +1105,35 @@ psutil_proc_connections(PyObject *self, PyObject *args)
                 inet_ntop(
                     kif->kf_sock_domain,
                     psutil_sockaddr_addr(kif->kf_sock_domain,
+#if __FreeBSD_version < 1200031
                                          &kif->kf_sa_local),
+#else
+                                         &kif->kf_un.kf_sock.kf_sa_local),
+#endif
                     lip,
                     sizeof(lip));
                 inet_ntop(
                     kif->kf_sock_domain,
                     psutil_sockaddr_addr(kif->kf_sock_domain,
+#if __FreeBSD_version < 1200031
                                          &kif->kf_sa_peer),
+#else
+                                         &kif->kf_un.kf_sock.kf_sa_peer),
+#endif
                     rip,
                     sizeof(rip));
                 lport = htons(psutil_sockaddr_port(kif->kf_sock_domain,
+#if __FreeBSD_version < 1200031
                                                    &kif->kf_sa_local));
+#else
+                                                   &kif->kf_un.kf_sock.kf_sa_local));
+#endif
                 rport = htons(psutil_sockaddr_port(kif->kf_sock_domain,
+#if __FreeBSD_version < 1200031
                                                    &kif->kf_sa_peer));
+#else
+                                                   &kif->kf_un.kf_sock.kf_sa_peer));
+#endif

                 // construct python tuple/list
                 laddr = Py_BuildValue("(si)", lip, lport);
@@ -1115,7 +1162,11 @@ psutil_proc_connections(PyObject *self, PyObject *args)
             else if (kif->kf_sock_domain == AF_UNIX) {
                 struct sockaddr_un *sun;

+#if __FreeBSD_version < 1200031
                 sun = (struct sockaddr_un *)&kif->kf_sa_local;
+#else
+                sun = (struct sockaddr_un *)&kif->kf_un.kf_sock.kf_sa_local;
+#endif
                 snprintf(
                     path, sizeof(path), "%.*s",
                     (int)(sun->sun_len - (sizeof(*sun) - sizeof(sun->sun_path))),
@@ -1626,7 +1677,7 @@ psutil_users(PyObject *self, PyObject *args)
     if (ret_list == NULL)
         return NULL;

-#if __FreeBSD_version < 900000
+#if !defined(__FreeBSD_version)
     struct utmp ut;
     FILE *fp;

@@ -1754,7 +1805,11 @@ int psutil_gather_inet(int proto, PyObject *py_retlist)
     struct xinpgen *xig, *exig;
     struct xinpcb *xip;
     struct xtcpcb *xtp;
+#if __FreeBSD_version >= 1200026
+    struct xinpcb *inp;
+#else
     struct inpcb *inp;
+#endif
     struct xsocket *so;
     const char *varname = NULL;
     size_t len, bufsize;
@@ -1820,8 +1875,13 @@ int psutil_gather_inet(int proto, PyObject *py_retlist)
                     goto error;
                 }
                 inp = &xtp->xt_inp;
+#if __FreeBSD_version >= 1200026
+                so = &inp->xi_socket;
+                status = xtp->t_state;
+#else
                 so = &xtp->xt_socket;
                 status = xtp->xt_tp.t_state;
+#endif
                 break;
             case IPPROTO_UDP:
                 xip = (struct xinpcb *)xig;
@@ -1830,7 +1890,11 @@ int psutil_gather_inet(int proto, PyObject *py_retlist)
                                  "struct xinpcb size mismatch");
                     goto error;
                 }
+#if __FreeBSD_version >= 1200026
+                inp = xip;
+#else
                 inp = &xip->xi_inp;
+#endif
                 so = &xip->xi_socket;
                 status = PSUTIL_CONN_NONE;
                 break;
@@ -2166,7 +2230,7 @@ PsutilMethods[] =
      "Return process CPU affinity."},
     {"proc_cpu_affinity_set", psutil_proc_cpu_affinity_set, METH_VARARGS,
      "Set process CPU affinity."},
-#if defined(__FreeBSD_version) && __FreeBSD_version >= 800000
+#if defined(__FreeBSD_version)
     {"proc_open_files", psutil_proc_open_files, METH_VARARGS,
      "Return files opened by process as a list of (path, fd) tuples"},
     {"proc_cwd", psutil_proc_cwd, METH_VARARGS,
@@ -2191,7 +2255,7 @@ PsutilMethods[] =
      "Return swap mem stats"},
     {"cpu_times", psutil_cpu_times, METH_VARARGS,
      "Return system cpu times as a tuple (user, system, nice, idle, irc)"},
-#if defined(__FreeBSD_version) && __FreeBSD_version >= 800000
+#if defined(__FreeBSD_version)
     {"per_cpu_times", psutil_per_cpu_times, METH_VARARGS,
      "Return system per-cpu times as a list of tuples"},
 #endif
