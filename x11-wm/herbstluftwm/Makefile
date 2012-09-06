# $FreeBSD$

PORTNAME=      herbstluftwm
PORTVERSION=   0.4
PORTREVISION=  1
CATEGORIES=    x11-wm
MASTER_SITES=  http://wwwcip.cs.fau.de/~re06huxa/herbstluftwm/tarballs/

MAINTAINER=    crshd@mail.com
COMMENT=       herbstluftwm is a manual tiling window manager for X11

USE_XORG=      x11

USE_GMAKE=     yes

OPTIONS=   BASH "Install completions for Bash" off \
           ZSH  "Install completions for zsh"  off

.include <bsd.port.pre.mk>

.if defined(WITH_BASH)
PLIST_SUB+= BASH=""
.else
PLIST_SUB+= BASH="@comment "
.endif

.if defined(WITH_ZSH)
PLIST_SUB+= ZSH=""
.else
PLIST_SUB+= ZSH="@comment "
.endif

post-patch:
.for file in dmenu dumpbeautify exec_on_tag execwith herbstcommander layout loadstate savestate
	${REINPLACE_CMD} -e 's#/bin/bash#/usr/bin/env bash#g' ${WRKSRC}/scripts/${file}.sh
.endfor
.for file in autostart panel.sh restartpanels.sh
	${REINPLACE_CMD} -e 's#/bin/bash#/usr/bin/env bash#g' ${WRKSRC}/share/${file}
.endfor

post-install:
.if !defined(WITH_BASH)
	rm -r ${PREFIX}/etc/bash_completion.d
.endif
.if !defined(WITH_ZSH)
	rm -r ${PREFIX}/share/zsh/site-functions
.endif

.include <bsd.port.post.mk>
