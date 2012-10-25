# $FreeBSD$

PORTNAME      = dzen2
DISTVERSION   = 20121024
PORTREVISION  = 1
CATEGORIES    = x11

USE_GITHUB    = yes
MASTER_SITES  = GITHUB

GH_ACCOUNT    = robm
GH_PROJECT    = dzen
GH_TAGNAME    = master
GH_COMMIT     = 982023f

MAINTAINER = crshd@mail.com
COMMENT    = General purpose messaging, notification and menuing program for X11

USE_XORG = x11

OPTIONS = XPM       "Enable XPM support"      On \
          XINERAMA  "Enable Xinerama support" On

.include <bsd.port.pre.mk>

.if !defined(WITHOUT_XPM)
USE_XORG+=	xpm
.endif

.if !defined(WITHOUT_XINERAMA)
USE_XORG+=	xinerama
.endif

post-patch:
.if !defined(WITHOUT_XPM)
	${REINPLACE_CMD} -e 's/^LIBS =\(.*\)$$/LIBS =\1 -lXpm/g'	\
	  	-e 's/^CFLAGS =\(.*\)$$/CFLAGS =\1 -DDZEN_XPM/g'	\
		${WRKSRC}/config.mk
.endif
.if !defined(WITHOUT_XINERAMA)
	${REINPLACE_CMD} -e 's/^LIBS =\(.*\)$$/LIBS =\1 -lXinerama/g'	\
	  	-e 's/^CFLAGS =\(.*\)$$/CFLAGS =\1 -DDZEN_XINERAMA/g'	\
		${WRKSRC}/config.mk
.endif
	${REINPLACE_CMD} -e 's,/usr/X11R6,${LOCALBASE},g'       \
		${WRKSRC}/config.mk

.include <bsd.port.post.mk>
