FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRCREV = "76d748841eb363555ff67a7160b7bc8d0f51acb8"

SRC_URI_append = " \
    file://0001-Added-custom-r1701-platform.patch \
    file://0002-BUGFIX-Dont-call-trustfence-function-since-HQ-meter-does-not-use-this-feature.patch \
"

COMPATIBLE_MACHINE = "(ccimx6$|ccimx6ul|ccimx8x|r1701)"
