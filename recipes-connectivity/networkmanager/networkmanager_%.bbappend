FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Removed Ethernet 1 and wifi related configuration.
#SRC_URI_remove += " \
#    file://nm.eth1.dhcp \
#    file://nm.eth1.static \
#    file://nm.wlan0.dhcp \
#    file://nm.wlan0.static \
#"

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
