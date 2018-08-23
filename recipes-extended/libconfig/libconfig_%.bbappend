# This BBAPPEND file is requires since actual URL does not have tar files. They have moved to GITHUB
# So we are overriding the URL and associated License and TAR files checksums.

LICENSE = "LGPLv2.1"
LIC_FILES_CHKSUM = "file://COPYING.LIB;md5=fad9b3332be894bab9bc501572864b29"

SRC_URI = "https://github.com/hyperrealm/libconfig/archive/v${PV}.tar.gz"
SRC_URI[md5sum] = "e92a91c2ddf3bf77bea0f5ed7f09e492"
SRC_URI[sha256sum] = "cae5c02361d8a9b2bb26946c64f089d2e5e599972f386203fbc48975c0d885c8"
