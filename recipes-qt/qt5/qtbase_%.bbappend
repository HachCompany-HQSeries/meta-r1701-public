# Copyright (C) 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"

COMPATIBLE_MACHINE = "(r1701)"
