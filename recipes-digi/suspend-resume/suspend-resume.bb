# Copyright (C) 2017 Digi International Inc.

SUMMARY = "Suspend and resume test application"
SECTION = "examples"
LICENSE = "MPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MPL-2.0;md5=815ca599c9df247a0c7f619bab123dad"

SRC_URI = "file://suspend-resume"

S = "${WORKDIR}/suspend-resume"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 suspram ${D}${bindir}
}

COMPATIBLE_MACHINE = "(ccimx6ul | r1701)"
