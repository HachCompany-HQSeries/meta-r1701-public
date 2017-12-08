#!/bin/sh
case "$1" in
  start)
        start-stop-daemon -b -S -q -m -p /var/run/simpleserver.pid --exec /usr/bin/simpleserver
        echo "OK"
        ;;
  stop)
        start-stop-daemon -K -q -p /var/run/simpleserver.pid
        echo "OK"
        ;;
  *)
        echo "Usage: $0 {start|stop}"
        exit 1
esac
exit $?
