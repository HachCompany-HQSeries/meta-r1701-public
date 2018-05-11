#
# Copyright (C) 2013-2017, Digi International Inc.
#
SUMMARY = "QT packagegroup for HACH image"

QT5_PKS_append_ccimx6ul = " qtdeclarative-tools"
QT5_PKS_append_r1701 = " qtdeclarative-tools"

# remove serail port support.
QT5_PKS_remove += "qtserialport"

# We do not want any example and demos in final image.
QT5_EXAMPLES = ""
QT5_DEMOS = ""
MACHINE_QT5_EXTRA_INSTALL = ""

# No need to have these dependancies.
RDEPENDS_${PN}_remove += " \
    liberation-fonts \
    ${QT5_DEMOS} \
    ${QT5_EXAMPLES} \
"

