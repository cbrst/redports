# $FreeBSD$

PORTNAME     = dunst
PORTVERSION  = 0.3.1
PORTREVISION = 1
CATEGORIES   = x11

USE_GITHUB   = yes
GH_ACCOUNT   = knopwob
GH_TAGNAME   = ${PORTVERSION}
GH_COMMIT    = 63ceed3

MAINTAINER   = crshd@mail.com
COMMENT      = A dmenu-ish notification system

USE_GMAKE    = yes
USE_XORG     = x11 xscrnsaver xinerama
USE_PERL5_BUILD = yes

OPTIONS      = XFT "Enable XFT support" On

BUILD_DEPENDS= ${LOCALBASE}/lib/libxdg-basedir.so:${PORTSDIR}/x11/libxdg-basedir

.include <bsd.port.pre.mk>

.if defined(WITH_XFT)
USE_XORG+= xft
.endif

post-patch:
	@${GREP} -Rl '%%[[:alpha:]]\+%%' ${WRKSRC}|${XARGS} ${REINPLACE_CMD} \
	         -e "s,%%PREFIX%%,${PREFIX},g" \
	         -e "s,%%LOCALBASE%%,${LOCALBASE},g" \
	         -e "s,%%MANPREFIX%%,${MANPREFIX},g"
.if defined(WITHOUT_XFT)
	@${REINPLACE_CMD} -e 's,$${XFTINC},,g' \
	         -e 's,$${XFTLIBS},,g' ${WRKSRC}/config.mk
.endif

.include <bsd.port.post.mk>
