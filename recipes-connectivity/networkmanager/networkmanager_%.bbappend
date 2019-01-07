FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Removed redudant packages.
PACKAGECONFIG_remove = "netconfig nss modemmanager \
                        ${@bb.utils.contains('DISTRO_FEATURES', 'bluetooth', '${BLUEZ}', '', d)} \
                        ${@bb.utils.contains('DISTRO_FEATURES', 'wifi', 'wifi', '', d)}"
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
