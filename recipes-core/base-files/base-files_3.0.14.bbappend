FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI += " \
    file://sw-version \
    file://authorized_keys \
"

SOFTWARE_VERSION ?= "0.0.1"
USER="root"

do_install_append() {
	install -m 0644 ${WORKDIR}/sw-version ${D}${sysconfdir}/
	install -m 0700 -d ${D}/home/${USER}/.ssh
    install -m 0600 ${WORKDIR}/authorized_keys ${D}/home/${USER}/.ssh/
}

do_unpack[postfuncs] += "fill_version"

fill_version() {
	sed -i -e "s,##FW_VERSION##,${SOFTWARE_VERSION},g" "${WORKDIR}/sw-version"
}

FILES_${PN} +="/home/${USER}/.ssh"
