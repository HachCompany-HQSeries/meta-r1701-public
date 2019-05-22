FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_append = " fontconfig"

PACKAGECONFIG_append_r1701 = " linuxfb"

QT_CONFIG_FLAGS += " -optimize-size -gif"

COMPATIBLE_MACHINE = "(r1701)"
PACKAGE_ARCH = "${MACHINE_ARCH}"
