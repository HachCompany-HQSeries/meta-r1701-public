VIRTUAL-RUNTIME_http-server = "lighttpd"
VIRTUAL-RUNTIME_ftp-server = ""
VIRTUAL-RUNTIME_snmp-manager = ""

# We do not use any 2G/3G/4G modem related stuff.
CELLULAR_PKGS = ""

RDEPENDS_${PN} = "\
	iproute2 \
	${VIRTUAL-RUNTIME_ftp-server} \
	${VIRTUAL-RUNTIME_http-server} \
	${VIRTUAL-RUNTIME_network-utils} \
	${VIRTUAL-RUNTIME_snmp-manager} \
	${VIRTUAL-RUNTIME_ntp-client} \
"
