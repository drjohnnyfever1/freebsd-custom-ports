# $FreeBSD$

PORTNAME=	inputstream.adaptive
DISTVERSION=	2.6.0
DISTVERSIONSUFFIX=	-Matrix
PORTREVISION=	3
CATEGORIES=	multimedia
PKGNAMEPREFIX=	kodi-addon-
PKGNAMESUFFIX=	-devel

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Adaptive stream support for Kodi

LICENSE=	GPLv2
LICENSE_FILE=	${WRKSRC}/LICENSE.GPL

BUILD_DEPENDS=	kodi-devel>=19.0.a20200727:multimedia/kodi-devel
LIB_DEPENDS=	libexpat.so:textproc/expat2
RUN_DEPENDS=	kodi-devel>=19.0.a20200727:multimedia/kodi-devel

USES=		cmake compiler:c++14-lang
USE_GITHUB=	yes
GH_ACCOUNT=	peak3d

PLIST_SUB=	SOVERSION=${DISTVERSION}

post-extract:
	${MV} ${WRKSRC}/inputstream.adaptive/resources/resource.language.sk_sk \
		${WRKSRC}/inputstream.adaptive/resources/language

post-install:
	${STRIP_CMD} ${STAGEDIR}${PREFIX}/lib/kodi/addons/inputstream.adaptive/libssd_wv.so
	@cd ${STAGEDIR}${PREFIX} && \
		${FIND} share/kodi/addons/inputstream.adaptive/resources/language \
		-name strings.po >> ${TMPPLIST}

.include <bsd.port.mk>
