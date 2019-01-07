FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://udhcpd.conf"

FILES_${PN}-udhcpd = "${sysconfdir}/udhcpd.conf"

do_install_append_r1701() {

    # Install USB RNDIS related static IP address information.
    install -m 0644 ${WORKDIR}/udhcpd.conf ${D}/${sysconfdir}/udhcpd.conf

    # Remove pswitch-standby script from system. No need to remove standby since we use LION battery and we have now
    # different logic for power management.
    #if grep "CONFIG_ACPID=y" ${WORKDIR}/defconfig; then
    #    rm -f ${D}${sysconfdir}/acpi/pswitch-standby
    #fi

    # Removed HTTPD related stuff.
    if grep "CONFIG_HTTPD=y" ${WORKDIR}/defconfig; then
        rm -f ${D}/srv/www/index.html
        rm -f ${D}/srv/www/digi-logo.png
    fi
}
