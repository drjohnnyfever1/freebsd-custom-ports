# $FreeBSD$
# Created by Olivier Certner <olce.freebsd.ports@certner.fr>

PORTNAME=	newmoon
DISTVERSION=	28.17.0
# UXP_VERSION has to be updated according to DISTVERSION (see recorded commit
# of the 'platform' git sub-module and corresponding release in UXP/releases;
# see MASTER_SITES for base URLs).
UXP_VERSION=	20201218
CATEGORIES=	www
MASTER_SITES=	https://repo.palemoon.org/MoonchildProductions/Pale-Moon/archive/:palemoon \
		https://repo.palemoon.org/MoonchildProductions/UXP/archive/:uxp
DISTFILES=	${DISTVERSION}_Release.tar.gz:palemoon \
		RELBASE_${UXP_VERSION}.tar.gz:uxp
DIST_SUBDIR=	MoonchildProductions
WRKSRC=		${WRKDIR}/pale-moon

MAINTAINER=	olce.freebsd.ports@certner.fr
COMMENT=	Open-source web browser mostly using Pale Moon(TM) code

# Not sure about the real status of ARM, but only ARM packages of old versions of
# PM are available in some Linux distros.
ONLY_FOR_ARCHS=	amd64 i386
ONLY_FOR_ARCHS_REASON=Upstream only cares about i386/amd64

# Common deps & build options
BUILD_DEPENDS=	autoconf-2.13:devel/autoconf213 \
		yasm:devel/yasm \
		zip:archivers/zip \

LIB_DEPENDS=	libdbus-1.so:devel/dbus \
		libdbus-glib-1.so:devel/dbus-glib \
		libfontconfig.so:x11-fonts/fontconfig \
		libfreetype.so:print/freetype2

USES=		dos2unix perl5 gmake gnome xorg desktop-file-utils
# This is purely so that the patch applied to this file on NO_UAOVERRIDES can
# be checked in cleanly into SVN.
DOS2UNIX_FILES=	palemoon/branding/shared/pref/uaoverrides.inc
USE_PERL5=	build
USE_GNOME=	cairo gdkpixbuf2 glib20 pango
USE_XORG=	xt x11 xcb xext xrender
# 9 is the last version endorsed upstream (and was the default in ports when
# this was written). Should ideally be bumped with GCC's default version, but
# *only if* validated by upstream (or thoroughly tested by us). Hence
# hardcoded. (For the record, building with clang produces malfunctioning
# executables.)
# NOTE: We make sure that there are no runtime dependencies to GCC. See
# [GCC-no-rdep-*] tags below. This should rather be solved in the ports
# infrastructure proper (see PR 211154; also, allowing to choose among C++
# standard libraries, besides providing a common default, would be great).
USE_GCC=	9

# Put this here pending creation of USES=gtar
BUILD_DEPENDS+=	gtar:archivers/gtar
BINARY_ALIAS+=	tar=${LOCALBASE}/bin/gtar

BUILD_DEPENDS+=	tauthon:lang/tauthon
BINARY_ALIAS+=	python=${LOCALBASE}/bin/tauthon \
		python2=${LOCALBASE}/bin/tauthon \
		python2.7=${LOCALBASE}/bin/tauthon

BUNDLE_LIBS=	yes

# Options

OPTIONS_SINGLE+=	GUI
OPTIONS_SINGLE_GUI=	GTK2 GTK3
OPTIONS_DEFAULT+=	GTK3

OPTIONS_SINGLE+=	SOUND
OPTIONS_SINGLE_SOUND=	ALSA PULSEAUDIO
OPTIONS_DEFAULT+=	ALSA

OPTIONS_DEFINE+= 	SYNC NO_UAOVERRIDES

SYNC_DESC=		Pale Moon Sync service (see https://www.palemoon.org/sync/)
NO_UAOVERRIDES_DESC=	Deactivate default User Agent overrides (prefs renamed)

GTK2_USE=		gnome=gtk20,gconf2
# XXX
# GTK3 needs gtk20 and gconf2 to pass old-configure (and maybe more).
# See also: https://repo.palemoon.org/MoonchildProductions/UXP/issues/1638.
GTK3_USE=		gnome=gtk30,gtk20:build,gconf2:build

OPTIONS_SUB=		yes

ALSA_BUILD_DEPENDS=	${PREFIX}/include/alsa/asoundlib.h:audio/alsa-lib
ALSA_LIB_DEPENDS=	libasound.so:audio/alsa-lib
PULSEAUDIO_BUILD_DEPENDS=${LOCALBASE}/include/pulse/pulseaudio.h:audio/pulseaudio
PULSEAUDIO_LIB_DEPENDS=	libpulse.so:audio/pulseaudio

NO_UAOVERRIDES_EXTRA_PATCHES= \
${PATCHDIR}/no_uaoverrides_patch-palemoon_branding_shared_pref_uaoverrides.inc

.include <bsd.port.options.mk>

# Vars and targets

# Have GCC link with libc++. Needed because some library dependencies (e.g.,
# libgraphite2.so) are linked against libc++ already, and mixing both with this
# codebase results in instant executable crash.
# As advised here:
# https://libcxx.llvm.org/docs/UsingLibcxx.html#using-libc-with-gcc
# with the change of linking gcc statically, so that this port doesn't need
# to depend on GCC at all at runtime. [GCC-no-rdep-1]
# This is similar to what 'compiler.mk' does on using "gcc-c++11-lib".
CXXFLAGS+=	-nostdinc++ -I/usr/include/c++/v1 -nodefaultlibs \
		-lc++ -lcxxrt -lm -lc -lgcc \
		-pthread -lpthread
LDFLAGS+=	-lgcc

DOT_MOZCONFIG=	${WRKSRC}/.mozconfig
PM_BUILD_DIR=	${WRKSRC}/pmbuild

NEWMOON_DESKTOP=${WRKSRC}/palemoon/branding/unofficial/browser.desktop

.if ${PORT_OPTIONS:MGTK3}
PM_GTK_VERSION:=3
.else
PM_GTK_VERSION:=2
.endif

.if ${ARCH} == "amd64"
PM_BUILD_64=_BUILD_64=1
.else
PM_BUILD_64=
.endif

# Extract only -O options
PM_OPTIMIZE=${CFLAGS:M-O*}

.for VAR in PM_BUILD_64 PM_GTK_VERSION PM_BUILD_DIR PM_OPTIMIZE
PM_MOZCONFIG_REINPLACE_ARGS+=-e 's!%%${VAR}%%!${${VAR}}!'
.endfor


# Taken from bsd.gecko.mk. See comment there.
.if defined(DISABLE_MAKE_JOBS) || defined(MAKE_JOBS_UNSAFE)
MAKE_JOBS_NUMBER=	1
.endif
.if defined(MAKE_JOBS_NUMBER)
MOZ_MAKE_FLAGS+=	-j${MAKE_JOBS_NUMBER}
.endif


post-extract:
	${RMDIR} ${WRKSRC}/platform
	${RLN} ${WRKDIR}/uxp ${WRKSRC}/platform

# Prepare '.mozconfig' for 'configure'
pre-configure:
	${CP} ${FILESDIR}/dot.mozconfig ${DOT_MOZCONFIG}
	${REINPLACE_CMD} ${PM_MOZCONFIG_REINPLACE_ARGS} ${DOT_MOZCONFIG}
.if ${PORT_OPTIONS:MALSA}
	${ECHO_CMD} ac_add_options --enable-alsa >> ${DOT_MOZCONFIG}
	${ECHO_CMD} ac_add_options --disable-pulseaudio >> ${DOT_MOZCONFIG}
.endif
.if ! ${PORT_OPTIONS:MSYNC}
	${ECHO_CMD} ac_add_options --disable-sync >> ${DOT_MOZCONFIG}
.endif
# .if ${PORT_OPTIONS:MDEBUG}
# 	${ECHO_CMD} ac_add_options --disable-strip >> ${DOT_MOZCONFIG}
# 	${ECHO_CMD} ac_add_options --disable-install-strip >> ${DOT_MOZCONFIG}
# # Causes compile error
# #	${ECHO_CMD} ac_add_options --enable-debug >> ${DOT_MOZCONFIG}
# .endif
.if defined(MOZ_MAKE_FLAGS)
	${ECHO_CMD} mk_add_options MOZ_MAKE_FLAGS=${MOZ_MAKE_FLAGS:Q} \
		>> ${DOT_MOZCONFIG}
.endif
# Add build variables
.for VAR in CPP CXXCPP CC CXX CPPFLAGS CFLAGS CXXFLAGS LDFLAGS \
	LD AS AR RANLIB OBJDUMP NM
.if defined(${VAR})
# Remove -rpath options to GCC's directory [GCC-no-rdep-2]
	${ECHO_CMD} export ${VAR}=\"${${VAR}:N*-rpath*lib/gcc*}\" >> ${DOT_MOZCONFIG}
.endif
.endfor

# Running 'mach configure' separately is "strongly discouraged" (see Mozilla's
# doc on build options configuration). Still, things seem to work correctly
# even with this separate step.
do-configure:
	cd ${WRKSRC} && ${SETENV} PATH=${PATH} ./mach configure

do-build:
	cd ${WRKSRC} && ${SETENV} PATH=${PATH} ./mach build
# Going through the package route (the only one documented upstream)
	cd ${WRKSRC} && ${SETENV} PATH=${PATH} ./mach package

do-install:
	${TAR} -C ${STAGEDIR}${PREFIX}/lib \
		-xf ${PM_BUILD_DIR:Q}/dist/palemoon*.tar* \
		palemoon
	${RLN}  ${STAGEDIR}${PREFIX}/lib/palemoon/palemoon \
		${STAGEDIR}${PREFIX}/bin/palemoon
	${INSTALL_DATA} ${NEWMOON_DESKTOP} \
		${STAGEDIR}${PREFIX}/share/applications/newmoon.desktop
	${RLN}  ${STAGEDIR}${PREFIX}/lib/palemoon/browser/icons/mozicon128.png \
		${STAGEDIR}${PREFIX}/share/pixmaps/palemoon.png

.include <bsd.port.mk>
# [GCC-no-rdep-3]
RUN_DEPENDS:=	${RUN_DEPENDS:Ngcc*}
