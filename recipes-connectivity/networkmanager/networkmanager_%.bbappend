FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

DEPENDS_remove = " \
    python3-pygobject-native \
"

# Removed redudant packages.
PACKAGECONFIG_remove = "consolekit \
                        netconfig \
                        nss \
                        modemmanager \
                        ${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', '${BLUEZ}', '', d)} \
                        ${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'wifi', '', d)} \
                        ppp \
                        qt4-x11-free \
"

PACKAGES_remove += "libnmglib-vpn ${PN}-nmtui-doc ${PN}-adsl"

FILES_libnmglib-vpn = ""
FILES_${PN}-adsl = ""

#${libdir}/NetworkManager/VPN
FILES_${PN}_remove += " \
    ${libdir}/pppd/*/nm-pppd-plugin.so \
"

FILES_${PN}-dev_remove += " \
    ${libdir}/pppd/*/*.la \
"

FILES_${PN}-nmtui = " \
    ${bindir}/nmtui \
    ${bindir}/nmtui-edit \
    ${bindir}/nmtui-connect \
    ${bindir}/nmtui-hostname \
"

FILES_${PN}-nmtui-doc = " \
    ${mandir}/man1/nmtui* \
"

PACKAGECONFIG[bluez5] = ""
PACKAGECONFIG[wifi] = ""
PACKAGECONFIG[modemmanager] = ""
#PACKAGECONFIG[ppp] = ""

# We do not use ethernet 1 and wireless
ETH1_STATIC_CIDR = ""
WLAN0_STATIC_CIDR = ""

do_install_append_r1701() {
    # Remove 1st ethernet
    #rm -f ${D}${sysconfdir}/NetworkManager/system-connections/nm.eth0

    # Remove 2nd ethernet
    rm -f ${D}${sysconfdir}/NetworkManager/system-connections/nm.eth1

    # Remove Wireless
    rm -f ${D}${sysconfdir}/NetworkManager/system-connections/nm.wlan0

    # Remove Cellular
    rm -f ${D}${sysconfdir}/NetworkManager/system-connections/nm.cellular
}
