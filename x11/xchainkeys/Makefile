# $FreeBSD$

PORTNAME=	xchainkeys
PORTVERSION=	0.11
CATEGORIES=	x11
MASTER_SITES=	GOOGLE_CODE

MAINTAINER=	crshd@mail.com
COMMENT=	Allows you to launch shell commands under X with your keyboard

USE_XORG=	x11
GNU_CONFIGURE=  yes
CPPFLAGS+=	-I${LOCALBASE}/include
LDFLAGS+=	-L${LOCALBASE}/lib

MAN1=		xchainkeys.1
PLIST_FILES=	bin/xchainkeys \
                share/doc/xchainkeys/README \
                share/doc/xchainkeys/example.conf \
                %%EXAMPLESDIR%%/xchainkeys.conf
PLIST_DIRS=	%%EXAMPLESDIR%% \
                share/doc/xchainkeys \

# WISH=		${LOCALBASE}/bin/wish8.4

.include <bsd.port.mk>
# .include <bsd.port.pre.mk>
# 
# post-install:
# 	@${MKDIR} ${EXAMPLESDIR}
# 	${INSTALL_DATA} ${WRKSRC}/example.conf ${EXAMPLESDIR}/xchainkeys.conf
# 
# .include <bsd.port.post.mk>
