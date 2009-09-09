#!/bin/sh

# Deleuze proposed following statustext.sh

# Just add following line to your .xinitrc to launch this status text:
#
# exec <patch-to-this-dir>/statustext.sh&
# (or whatever file name you prefer)
#

# set statusbar to
# .wmi/common.conf:statusbar.status-width=60

# time format
TFORMAT="%d.%m., %H:%M"

while sleep 8
do
    UP=`uptime |awk '{ print $1 " " $2  $3 $4 " " $5 " " $6 " " $7 " "$9
    +$10 $11
}'`
    TAIL=`tail -n 1 /var/log/messages`

    wmiremote -t "[$TAIL] "
    sleep 8
    wmiremote -t "[$UP] "
done
