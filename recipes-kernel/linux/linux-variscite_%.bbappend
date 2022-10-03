FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

# Prevent the use of in-tree defconfig
unset KBUILD_DEFCONFIG

#CUSTOM_DEVICETREE = "imx8mp-var-som-symphony.dts"

SRC_URI += "\
    file://defconfig \
"

# do_configure_append() {
#     # For arm64 bit freescale/NXP devices
#     cp ${WORKDIR}/${CUSTOM_DEVICETREE} ${S}/arch/arm64/boot/dts/freescale
# }
