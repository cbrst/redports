# $FreeBSD$

PORTNAME     = fontconfig-infinality
PORTVERSION  = 20120615
CATEGORIES   = x11-fonts
MASTER_SITES = http://www.infinality.net/fedora/linux/zips/
DISTFILES    = ${PORTNAME}-1-${PORTVERSION}_1.tar.bz2

MAINTAINER   = crshd@mail.com
COMMENT      = Additional fontconfig files for infinality-patched freetype

NO_WRKSUBDIR = yes
.include <bsd.port.pre.mk>

do-build:

do-install:
	(cd ${WRKSRC} && ${COPYTREE_SHARE} \* ${PREFIX}/etc/fonts/)
	${INSTALL_SCRIPT} ${WRKDIR}/infinality/infctl.sh ${PREFIX}/bin/infctl

.include <bsd.port.post.mk>
