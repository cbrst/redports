# $FreeBSD$

PORTNAME=       ranger
PORTVERSION=    1.5.5
CATEGORIES=     sysutils python
MASTER_SITES=   http://ranger.nongnu.org/
PKGNAMEPREFIX=  ${PYTHON_PKGNAMEPREFIX}

MAINTAINER=     crshd@mail.com
COMMENT=        Ranger is a file manager with VI key bindings

OPTIONSFILE=    ${PORT_DBDIR}/py-${PORTNAME}/options

OPTIONS_DEFINE= ENCODING GNU SUDO HIGHLIGHT BT EXIF HTML

OPTIONS_SINGLE= HTML
OPTIONS_SINGLE_HTML= LYNX W3M ELINKS

ENCODING_DESC=  Fix encoding problems
GNU_DESC=       Use GNU coreutils
SUDO_DESC=      Enable "Run as root"
HIGHLIGHT_DESC= Syntax highlighting for files
LYNX_DESC=      Preview HTML files with lynx
W3M_DESC=       Preview HTML files with w3m
ELINKS_DESC=    Preview HTML files with elinks
BT_DESC=        Preview BitTorrent information
EXIF_DESC=      Show EXIF data

.include <bsd.port.options.mk>

USE_PYTHON=      yes
USE_PYDISTUTILS= yes

.if ${PORT_OPTIONS:MGNU}
RUN_DEPENDS+=    gcp:${PORTSDIR}/sysutils/coreutils
.endif

.if ${PORT_OPTIONS:MSUDO}
RUN_DEPENDS+=    sudo:${PORTSDIR}/security/sudo
.endif

.if ${PORT_OPTIONS:MENCODING}
RUN_DEPENDS+=    ${PYTHON_PKGNAMEPREFIX}chardet>0:${PORTSDIR}/textproc/py-chardet
.endif

.if ${PORT_OPTIONS:MHIGHLIGHT}
RUN_DEPENDS+=    highlight:${PORTSDIR}/textproc/highlight
.endif

.if ${PORT_OPTIONS:MLYNX}
RUN_DEPENDS+=    lynx:${PORTSDIR}/www/lynx
.endif

.if ${PORT_OPTIONS:MW3M}
RUN_DEPENDS+=    w3m:${PORTSDIR}/www/w3m
.endif

.if ${PORT_OPTIONS:MELINKS}
RUN_DEPENDS+=    elinks:${PORTSDIR}/www/elinks
.endif

.if ${PORT_OPTIONS:MBT}
RUN_DEPENDS+=    transmission-show:${PORTSDIR}/net-p2p/transmission
.endif

.if ${PORT_OPTIONS:MEXIF}
RUN_DEPENDS+=    mediainfo:${PORTSDIR}/multimedia/mediainfo
.endif

MAN1=            ranger.1

post-patch:
	@${REINPLACE_CMD} -e 's|setuptools|distutils.core|; s|share/man/man|man/man|' ${WRKSRC}/${PYSETUP}

.include <bsd.port.mk>
