FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://public.pem"

do_install_append() {
    # Install RSA public key for verifying firmware images
    install -d ${D}${sysconfdir}/ssl/certs
    install -m 0600 ${WORKDIR}/public.pem ${D}${sysconfdir}/ssl/certs/key.pub
}
