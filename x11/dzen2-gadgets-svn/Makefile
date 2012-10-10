# $FreeBSD$

PORTNAME      = dzen2
PKGNAMESUFFIX = -gadgets-svn
PORTVERSION   = 20091018
PORTREVISION  = 1
CATEGORIES    = x11
MASTER_SITES  = http://xmonad-dzen2-freebsd.googlecode.com/files/

MAINTAINER = crshd@mail.com
COMMENT    = General purpose messaging, notification and menuing program for X11

USE_XORG = x11

WRKSRC = ${WRKDIR}/${PORTNAME}-${PORTVERSION}/gadgets

.include <bsd.port.pre.mk>

post-patch:
	${REINPLACE_CMD} -e 's,/usr/X11R6,${LOCALBASE},g'       \
		${WRKSRC}/config.mk

.include <bsd.port.post.mk>
