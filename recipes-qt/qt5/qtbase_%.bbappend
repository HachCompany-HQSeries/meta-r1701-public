# Copyright (C) 2015-2017, Digi International Inc.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://qt5.sh"
PACKAGECONFIG_append = " accessibility examples fontconfig sql-sqlite"

PACKAGECONFIG_append_r1701 = " linuxfb"

COMPATIBLE_MACHINE = "(r1701)"

