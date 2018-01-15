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
    r1701-apps \
"

# Add predefined runtime package groups. 
IMAGE_FEATURES += " \
    ssh-server-openssh \
"

# Assign a password to root account.
inherit extrausers
EXTRA_USERS_PARAMS += " \
    usermod -P hqd123 root; \
"

DISTRO_FEATURES_remove = "x11"

export IMAGE_BASENAME = "hach-image"
