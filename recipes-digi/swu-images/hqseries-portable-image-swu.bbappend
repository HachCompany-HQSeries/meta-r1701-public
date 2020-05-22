FILESEXTRAPATHS_prepend := "${THISDIR}/${BPN}:"

SRC_URI_append = " file://priv.pem"

# swupdate signing options:
SWUPDATE_SIGNING = "RSA"
SWUPDATE_PRIVATE_KEY = "${WORKDIR}/priv.pem"
