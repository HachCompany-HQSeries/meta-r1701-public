SUMMARY = "Framebuffer nanoX splash utility"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

DEPENDS = "microwindows"

SRC_URI = "	 \
file://xsplash.c  \
file://xsplash-config.h \
file://hachLogo.gif \
"

S = "${WORKDIR}"

do_compile() {
	${CC} -O2 -Wall ${LDFLAGS} xsplash.c -o xsplash -lnano-X
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 xsplash ${D}${bindir}
	install -m 0744 hachLogo.gif ${D}${bindir}
}
