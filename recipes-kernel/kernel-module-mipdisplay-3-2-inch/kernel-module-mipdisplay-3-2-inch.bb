SUMMARY = "Sharp 3.2 inch memory in place display driver"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

SRC_URI = " \
    file://mipdisplay-3-2-inch.c \
    file://Makefile"

S = "${WORKDIR}"

COMPATIBLE_MACHINE = "(r1701)"
