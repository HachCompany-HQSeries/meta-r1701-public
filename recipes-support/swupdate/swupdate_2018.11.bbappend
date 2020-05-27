FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

do_configure_append() {
    # Enable these config option so that we can use private key verify during upgrade.
    echo "CONFIG_SIGNED_IMAGES=y" >> ${S}/.config
    echo "CONFIG_SIGNED_IMAGES=y" >> ${S}/.config
    echo "CONFIG_SIGALG_RAWRSA=y" >> ${S}/.config
    echo "CONFIG_ENCRYPTED_IMAGES=y" >> ${S}/.config
    cml1_do_configure
}
