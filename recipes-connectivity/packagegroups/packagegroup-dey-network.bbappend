VIRTUAL-RUNTIME_http-server = ""
VIRTUAL-RUNTIME_ftp-server = ""
VIRTUAL-RUNTIME_snmp-manager = ""
VIRTUAL-RUNTIME_ntp-client = ""

# We do not use any 2G/3G/4G modem related stuff.
CELLULAR_PKGS = ""

RDEPENDS_${PN} = "\
	iproute2 \
	batctl \
	${@bb.utils.contains('DISTRO_FEATURES', 'cellular', '${CELLULAR_PKGS}', '', d)} \
	${VIRTUAL-RUNTIME_ftp-server} \
	${VIRTUAL-RUNTIME_http-server} \
	${VIRTUAL-RUNTIME_network-utils} \
	${VIRTUAL-RUNTIME_snmp-manager} \
	${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', '${VIRTUAL-RUNTIME_ntp-client}', d)} \
"
