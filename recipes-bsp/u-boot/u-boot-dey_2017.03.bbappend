FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRCREV = "1ef810133fcecacf14e09155bee10aa66fc6ade7"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-BUGFIX-Dont-call-trustfence-function-since-HQ-meter-does-not-use-this-feature.patch \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|r1701)"
