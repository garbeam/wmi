#!/bin/sh

# Gavin McCullagh proposed following change to the original
# statustext.sh (only works under GNU/Linux):

##########################
# time format
TFORMAT="%d.%m.%Y, %H:%M"
# give wmi time to start.
sleep 2

while pgrep wmi &>/dev/null
do
     MSG=`date "+${TFORMAT}"`
     APM=`cat /proc/apm| awk '{ print $7 }'`

     wmiremote -t "[$APM]  [$MSG]"
     # launched every 10 seconds, wait until wmi is up.
     sleep 10
done

# Additionally just uncomment and add following lines to
# your .xinitrc or .xsession file:

##############################################
# if ! pgrep statustext.sh;  then
#     exec ~/bin/statustext.sh &
# fi
##############################################
