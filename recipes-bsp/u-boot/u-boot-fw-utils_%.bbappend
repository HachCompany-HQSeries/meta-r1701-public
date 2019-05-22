# Copyright (C) 2019 HACH Company

pkg_postinst_ontarget_${PN}() {
	CONFIG_FILE="/etc/fw_env.config"
	MMCDEV="$(sed -ne 's,.*root=/dev/mmcblk\([0-9]\)p.*,\1,g;T;p' /proc/cmdline)"
	if [ -n "${MMCDEV}" ]; then
		sed -i -e "s,^/dev/mmcblk[^[:blank:]]\+,/dev/mmcblk${MMCDEV},g" ${CONFIG_FILE}
	fi

	PARTTABLE="/proc/mtd"
	
	# HACH Start - Create update partition ubifs.
	update_mtd="$(sed -ne "s/\(^mtd[0-9]\+\):.*\<update\>.*/\1/g;T;p" ${PARTTABLE} 2>/dev/null)"
	ubidetach -p /dev/${update_mtd} 2>/dev/null
	dev_number="$(ubiattach -p /dev/${update_mtd} 2>/dev/null | sed -ne 's,.*device number \([0-9]\).*,\1,g;T;p' 2>/dev/null)"
	ubimkvol "/dev/ubi${dev_number}" -N "update" -m 2>/dev/null
	# HACH End
	
	MTDINDEX="$(sed -ne "s/\(^mtd[0-9]\+\):.*\<environment\>.*/\1/g;T;p" ${PARTTABLE} 2>/dev/null)"
	if [ -n "${MTDINDEX}" ]; then
		# Initialize variables for fixed offset values
		# (backwards compatible with old U-Boot)
		ENV_OFFSET="${UBOOT_ENV_OFFSET}"
		ENV_REDUND_OFFSET="${UBOOT_ENV_SIZE}"
		ENV_SIZE="${UBOOT_ENV_SIZE}"
		ERASEBLOCK=""
		NBLOCKS=""

		if [ -f "/proc/device-tree/digi,uboot,dynamic-env" ]; then
			# Update variables for dynamic environment
			# - Both copies starting at the same offset
			ENV_REDUND_OFFSET="${UBOOT_ENV_OFFSET}"
			# - Calculated erase block size
			ERASEBLOCK="$(grep "^${MTDINDEX}:" ${PARTTABLE} | awk '{printf("0x%d",$3)}')"
			# - Calculated number of blocks
			MTDSIZE="$(grep "^${MTDINDEX}:" ${PARTTABLE} | awk '{printf("0x%d",$2)}')"
			NBLOCKS="$(((MTDSIZE - UBOOT_ENV_OFFSET) / ERASEBLOCK))"
			# If a range was provided, calculate the number of
			# blocks in the range and use that number, unless they
			# exceed the total number of blocks available in the
			# whole partition.
			if [ -n "${UBOOT_ENV_RANGE}" ]; then
				RANGE_BLOCKS="$((UBOOT_ENV_RANGE / ERASEBLOCK))"
				[ "${RANGE_BLOCKS}" -lt "${NBLOCKS}" ] && NBLOCKS="${RANGE_BLOCKS}"
			fi
		fi

		# Substitute stub with configuration and calculated values
		sed -i  -e "s/##MTDINDEX##/${MTDINDEX}/g" \
			-e "s/##ENV_OFFSET##/${ENV_OFFSET}/g" \
			-e "s/##ENV_REDUND_OFFSET##/${ENV_REDUND_OFFSET}/g" \
			-e "s/##ENV_SIZE##/${ENV_SIZE}/g" \
			-e "s/##ERASEBLOCK##/${ERASEBLOCK}/g" \
			-e "s/##NBLOCKS##/${NBLOCKS}/g" \
			${CONFIG_FILE}
	fi
}
