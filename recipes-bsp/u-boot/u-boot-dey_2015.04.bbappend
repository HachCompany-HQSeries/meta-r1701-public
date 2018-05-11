FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-Adding-rev-2-board-support.patch \
    file://0003-Add-Micrel-Phy-chip-support.patch \
"

COMPATIBLE_MACHINE = "(r1701)"
