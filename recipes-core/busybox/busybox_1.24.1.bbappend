FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://udhcpd.conf"

FILES_${PN}-udhcpd = "${sysconfdir}/udhcpd.conf"

do_install_append() {
    install -m 0644 ${WORKDIR}/udhcpd.conf ${D}/${sysconfdir}/udhcpd.conf
}
