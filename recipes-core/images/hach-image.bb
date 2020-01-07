SUMMARY = "An image derived from image hach-image-public"
LICENSE = "MIT"

require recipes-core/images/hach-image-public.bb

inherit populate_sdk_qt5
inherit populate_sdk_qt5_base

IMAGE_INSTALL += " \
    r1701-apps \
"

# Assign a password to root account.
inherit extrausers
EXTRA_USERS_PARAMS += " \
    usermod -P hqd123 root; \
"

export IMAGE_BASENAME = "hach-image"
