FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI_append = " \
	file://0001-Add-r1701-device-trees-to-the-kernel-makefile.patch \
"

