SUMMARY = "A QT5 image derived from dey-image-qt"
LICENSE = "MIT"

require recipes-core/images/dey-image-qt.bb

inherit populate_sdk_qt5

IMAGE_LINGUAS = "en-us"

IMAGE_INSTALL += " \
    boost \
    zeromq \
    fcgi \
    sqlite3 \
    busybox-udhcpd \
    u-boot-fw-utils \
"

# Add predefined runtime package groups.
IMAGE_FEATURES += " \
    ssh-server-openssh \
"

# Assign a password to root account.:q
inherit extrausers
EXTRA_USERS_PARAMS += " \
    usermod -P hqd123 root; \
"

# Remove distribution feature.
DISTRO_FEATURES_remove = "x11 wifi bluetooth"

export IMAGE_BASENAME = "hach-dey-image-qt"
