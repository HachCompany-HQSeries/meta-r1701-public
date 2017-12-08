#!/bin/sh
case "$1" in
  start)
        start-stop-daemon -S -q -m -p /var/run/sys_mgr.pid --exec /usr/bin/sys_mgr -- -d
        echo "OK"
        ;;
  stop)
        start-stop-daemon -K -q -p /var/run/sys_mgr.pid
        echo "OK"
        ;;
  *)
        echo "Usage: $0 {start|stop}"
        exit 1
esac
exit $?
