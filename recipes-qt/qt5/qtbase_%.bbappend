# Copyright (C) 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"

PACKAGECONFIG_append = " fontconfig"

PACKAGECONFIG_append_r1701 = " linuxfb"

QT_CONFIG_FLAGS += " -optimize-size -gif"

COMPATIBLE_MACHINE = "(r1701)"
PACKAGE_ARCH = "${MACHINE_ARCH}"
