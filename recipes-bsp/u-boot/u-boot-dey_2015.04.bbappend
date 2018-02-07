FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-Changed-code-to-support-r1701-rev-1-hardware.patch \
"

COMPATIBLE_MACHINE = "(r1701)"
