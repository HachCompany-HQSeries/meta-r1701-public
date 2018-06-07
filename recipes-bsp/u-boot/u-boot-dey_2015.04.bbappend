FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-Adding-Rev-3-board-support.patch \
"

COMPATIBLE_MACHINE = "(r1701)"
