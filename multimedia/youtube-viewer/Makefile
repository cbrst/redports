# $FreeBSD$

PORTNAME=      youtube-viewer
PORTVERSION=   3.0.3
CATEGORIES=    multimedia

MASTER_SITES=  https://nodeload.github.com/trizen/${PORTNAME}/tar.gz/
DISTFILES=     ${PORTVERSION}

MAINTAINER=    crshd@mail.com
COMMENT=       Search and play YouTube videos streaming in MPlayer

LICENSE=       GPLv3

RUN_DEPENDS=   p5-XML-Fast>=0:${PORTSDIR}/net/p5-XML-Fast \
               p5-libwww>=0:${PORTSDIR}/www/p5-libwww \
               p5-Term-ANSIColor>=0:${PORTSDIR}/devel/p5-Term-ANSIColor \
               mplayer:${PORTSDIR}/multimedia/mplayer

USE_PERL5_RUN= yes
NO_BUILD=      yes
PLIST_FILES=	bin/youtube-viewer \
               %%SITE_PERL%%/WWW/YoutubeViewer.pm

post-patch:
	@${REINPLACE_CMD} -e "s|#!/usr/bin/perl|#!${PERL}|g" ${WRKSRC}/WWW-YoutubeViewer/bin/youtube-viewer

do-install:
	@${INSTALL_SCRIPT} ${WRKSRC}/WWW-YoutubeViewer/bin/youtube-viewer ${PREFIX}/bin/
	@${MKDIR} ${SITE_PERL}/WWW/
	@${INSTALL_SCRIPT} ${WRKSRC}/WWW-YoutubeViewer/lib/WWW/YoutubeViewer.pm ${SITE_PERL}/WWW/

.include <bsd.port.mk>
