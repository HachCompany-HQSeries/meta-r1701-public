FILESEXTRAPATHS_prepend := "${THISDIR}/linux-dey-4.1:"

SRCREV = "f1810a3190faad9b96cc8385b394ce7c9a6c5c3e"
SRC_URI = " \
    git://github.com/digi-embedded/linux;branch=v4.1/dey-2.2/maint \
    file://defconfig \
"
