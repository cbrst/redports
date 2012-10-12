# $FreeBSD$

PORTNAME=     monsterwm
DISTVERSION=  20121014
PORTREVISION= 1
CATEGORIES=   x11-wm

USE_GITHUB=   yes
MASTER_SITES= GITHUB

GH_ACCOUNT=   c00kiemon5ter
GH_TAGNAME=   master
GH_COMMIT=    ab612e5

LICENSE=      MIT

MAINTAINER=   crshd@mail.com
COMMENT=      Minimal, lightweight, monstrous, tiny, dynamic tiling window manager

USE_XORG=     x11
USE_GMAKE=    yes

PLIST_FILES=  bin/monsterwm \
              man/man1/monsterwm.1

pre-everything::
	@${ECHO_MSG} "You can build monsterwm with your own config.h using the MONSTER_CONF knob:"
	@${ECHO_MSG} "make MONSTER_CONF=/path/to/monster/config.h install clean"

post-extract:
.if defined(MONSTER_CONF)
	@${ECHO_MSG} "creating config.h from ${MONSTER_CONF}"
	@${CP} ${MONSTER_CONF} ${WRKSRC}/config.h
.endif
	@${REINPLACE_CMD} -e 's,install -D,install -d,' \
		-e 's,cc,${CC},' \
		${WRKSRC}/Makefile

do-install:
	@${INSTALL_PROGRAM} ${WRKSRC}/monsterwm ${PREFIX}/bin
	@${INSTALL_MAN} ${WRKSRC}/monsterwm.1 ${PREFIX}/man/man1/monsterwm.1

.include <bsd.port.mk>
