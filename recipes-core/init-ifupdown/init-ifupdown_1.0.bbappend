FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append_r1701 = "\
    file://interfaces.usb \
    file://usb-pre-up \
    file://usb-post-down \
"

USB_INTERFACE ?= "usb0"
USB0_AUTO ?= "1"
USB0_STATIC_IP ?= "192.168.69.1"
USB0_STATIC_NETMASK ?= "255.255.255.0"

do_install_append_r1701() {
        if [ -n "${USB_INTERFACE}" ]; then
            install -m 0755 ${WORKDIR}/usb-pre-up ${D}${sysconfdir}/network/if-pre-up.d/
            install -m 0755 ${WORKDIR}/usb-post-down ${D}${sysconfdir}/network/if-post-down.d/
            cat ${WORKDIR}/interfaces.usb >> ${D}${sysconfdir}/network/interfaces
            [ -n "${USB0_AUTO}" ] && sed -i -e 's/^#auto ##USB_INTERFACE##/auto ##USB_INTERFACE##/g' ${D}${sysconfdir}/network/interfaces
                sed -i -e 's,##USB_INTERFACE##,${USB_INTERFACE},g' ${D}${sysconfdir}/network/interfaces
        fi

        # Remove config entries if corresponding variable is not defined
        [ -z "${USB00_STATIC_DNS}" ] && sed -i -e "/##USB0_STATIC_DNS##/d" ${D}${sysconfdir}/network/interfaces
        [ -z "${USB0_STATIC_GATEWAY}" ] && sed -i -e "/##USB0_STATIC_GATEWAY##/d" ${D}${sysconfdir}/network/interfaces
        [ -z "${USB0_STATIC_IP}" ] && sed -i -e "/##USB0_STATIC_IP##/d" ${D}${sysconfdir}/network/interfaces
        [ -z "${USB0_STATIC_NETMASK}" ] && sed -i -e "/##USB0_STATIC_NETMASK##/d" ${D}${sysconfdir}/network/interfaces

        # Replace interface parameters
        sed -i -e "s,##USB0_STATIC_IP##,${USB0_STATIC_IP},g" ${D}${sysconfdir}/network/interfaces
        sed -i -e "s,##USB0_STATIC_NETMASK##,${USB0_STATIC_NETMASK},g" ${D}${sysconfdir}/network/interfaces
        sed -i -e "s,##USB0_STATIC_GATEWAY##,${USB0_STATIC_GATEWAY},g" ${D}${sysconfdir}/network/interfaces
        sed -i -e "s,##USB0_STATIC_DNS##,${USB0_STATIC_DNS},g" ${D}${sysconfdir}/network/interfaces
        sed -i -e "s,##WPA_DRIVER##,${WPA_DRIVER},g" ${D}${sysconfdir}/network/interfaces
}
