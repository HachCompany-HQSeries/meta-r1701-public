FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-MPP-SPI-display-support.patch \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|r1701)"
