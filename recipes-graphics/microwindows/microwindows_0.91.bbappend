# Copyright (C) 2018 Hach Company.

FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}-${PV}:"

SRC_URI += " \
    file://0002-Use-14x16-font.patch;striplevel=2 \
"

