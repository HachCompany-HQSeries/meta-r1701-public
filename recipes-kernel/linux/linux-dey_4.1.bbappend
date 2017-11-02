FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
	file://0001-Add-r1701-device-trees-to-the-kernel-makefile.patch \
    file://0002-Adding-sharp-mip-display-support.patch \
    file://0003-Device-trees-for-r1701.patch \
"

