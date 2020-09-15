FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " file://udhcpd.conf"

do_install_append_r1701() {

    # Install USB RNDIS related static IP address information.
    install -m 0644 ${WORKDIR}/udhcpd.conf ${D}/${sysconfdir}/
    
    # Removed HTTPD related stuff.
    if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
        rm -f ${D}/srv/www/index.html
        rm -f ${D}/srv/www/digi-logo.png
        rm -f ${D}/srv/www/cgi-bin/nm
    fi

    # Remove NTPD files.
    if grep "CONFIG_NTPD=y" ${WORKDIR}/defconfig; then
        rm -f ${D}${sysconfdir}/init.d/busybox-ntpd
    fi
}
