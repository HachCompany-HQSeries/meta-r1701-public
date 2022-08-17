# Copyright (C) 2013-2016 Freescale Semiconductor
# Copyright 2017 NXP
# Copyright 2018-2019 Variscite Ltd.
# Released under the MIT license (see COPYING.MIT for the terms)
FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"


SUMMARY = "Linux kernel provided and supported by Variscite & maintained by HACH Company."
DESCRIPTION = "Linux kernel provided and supported by Variscite (based on the kernel provided by NXP) \
& maintained by HACH Company with focus on i.MX Family SOMs. It includes support for many IPs such as GPU, VPU and IPU."

#
# Original Variscite Kernel Config file for i.MX 6UL
#
# KBUILD_DEFCONFIG_imx6ul-var-dart = "imx_v7_var_defconfig"
KBUILD_DEFCONFIG_imx6ul-var-dart = ""

# Add custom defconfig and Patches.
SRC_URI += " \
    file://defconfig \
    \
    file://imx6ul-imx6ull-var-dart-wifi-dtsi.patch \
    file://imx6ul-imx6ull-var-dart-dtsi.patch \
    file://imx6ul-imx6ull-var-dart-6ulcustomboard-dtsi.patch \
"
