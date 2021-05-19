#!/bin/sh
kill `cat /var/run/lighttpd_webdav.pid`
lighttpd -f /etc_ro/lighttpd/lighttpd_webdav.conf -m /etc_ro/lighttpd/lib

