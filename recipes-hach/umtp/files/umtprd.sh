#!/bin/sh

FUNCTION_PATH=/dev/ffs-mtp
UMTPRD_DAEMON_NAME=uMTPrd
UMTPRD_DAEMON_PATH=/usr/bin/umtprd
UMTPRD_DAEMON_USER=root
UMTPRD_MOUNT_DIR=/tmp/cfg
UMTPRD_FUNCTIONFS_MOUNT_DIR=/dev/ffs-mtp

case "$1" in
  start)
    echo -n "Start uMTPrd daemon :"

    modelName=$(fw_printenv -n "model-name" 2>/dev/null)
    serialNumber=$(fw_printenv -n "serial-number" 2>/dev/null)

    # Make sure that all USB related kernel modules are not loaded. Firmware will handle the USB gadget connection
    # based on meter settings.
    modprobe -r g_ether usb_f_rndis usb_f_ecm libcomposite u_ether
    ifdown usb0

    modprobe libcomposite
    #echo "libcomposite ok"

    mkdir -p ${UMTPRD_MOUNT_DIR}
    #echo "Created ${UMTPRD_MOUNT_DIR}"

    mount none ${UMTPRD_MOUNT_DIR} -t configfs
    #echo "Mounted none on ${UMTPRD_MOUNT_DIR}"

    mkdir -p ${UMTPRD_MOUNT_DIR}/usb_gadget/g1
    #echo "Made DIR ${UMTPRD_MOUNT_DIR}/usb_gadget/g1"

    cd ${UMTPRD_MOUNT_DIR}/usb_gadget/g1
    mkdir -p configs/c.1
    #echo "Dir made - configs/c.1"

    mkdir -p functions/ffs.mtp
    #echo "DIR made - functions/ffs.mtp"

    mkdir -p strings/0x409
    mkdir -p configs/c.1/strings/0x409
    #echo "DIR made - strings/0x409  and configs/c.1/strings/0x409"

    echo 0x025C > idProduct
    echo 0x1670 > idVendor
    echo ${serialNumber} > strings/0x409/serialnumber
    echo "Hach" > strings/0x409/manufacturer
    echo ${modelName} > strings/0x409/product
    echo "Conf 1" > configs/c.1/strings/0x409/configuration
    echo 120 > configs/c.1/MaxPower

    ln -s functions/ffs.mtp configs/c.1
    #echo "SYMLINK made"

    mkdir -p ${UMTPRD_FUNCTIONFS_MOUNT_DIR}
    mount -t functionfs mtp ${UMTPRD_FUNCTIONFS_MOUNT_DIR}
    #echo "DIR made and mounted - ${UMTPRD_FUNCTIONFS_MOUNT_DIR} and functionfs"

    start-stop-daemon --start --quiet --exec ${UMTPRD_DAEMON_PATH}&

    #sleep 2

    # Disable the usb functions in beginning.
    #echo "" > ${UMTPRD_MOUNT_DIR}/usb_gadget/g1/UDC
    #echo "Empty string to ${UMTPRD_MOUNT_DIR}/usb_gadget/g1/UDC..."
    #ls /sys/class/udc/ > /tmp/cfg/usb_gadget/g1/UDC

    echo "DONE"
  ;;
  stop)
    echo -n "Stop uMTPrd daemon :"
    echo "" > ${UMTPRD_MOUNT_DIR}/usb_gadget/g1/UDC
    start-stop-daemon --stop --quiet --exec $UMTPRD_DAEMON_PATH
    echo " DONE"
    ;;
  restart|reload)
    "$0" stop
    "$0" start
    ;;
  *)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
esac

exit $?
