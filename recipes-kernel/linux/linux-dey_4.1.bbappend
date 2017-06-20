FILESEXTRAPATHS_prepend := "${THISDIR}/linux-dey-4.1:"

SRCREV = "f1810a3190faad9b96cc8385b394ce7c9a6c5c3e"
SRC_URI = " \
    git://github.com/digi-embedded/linux;branch=v4.1/dey-2.2/maint \
    file://0001-drivers-rtc-mca-cc6ul-fix-issue-with-alarm-wakeup.patch \
    file://0002-ccimx6ul-defconfig-configure-imx-sdhci-as-module.patch \
    file://0003-ARM-dts-ccimx6ulstarter-remove-all-interfaces-except.patch \
    file://defconfig \
"
