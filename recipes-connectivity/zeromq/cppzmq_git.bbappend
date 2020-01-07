# Do install in /etc --> this is the trick to add zmq.hpp in target and then it will add in
# /usr/include in sdk directory. DO NOT REMOVE /ETC INSTALL
do_install () {
        install -d ${D}/usr/include
        install -d ${D}/etc
        install -m 0755 ${S}/zmq.hpp ${D}/usr/include/
        install -m 0755 ${S}/zmq.hpp ${D}/etc
}

# Add both packages so that they both appear in respective locations.
PACKAGES = "${PN}-dev ${PN}"
