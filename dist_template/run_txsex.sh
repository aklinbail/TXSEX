############################################################
# Copy this File to $mmPath/AddOns to Launch Automatically
###########################################################

# Set up the environment
mmPath=$(cat /dev/shm/.mmPath)
. $mmPath/MockbaMod/env.sh
port="$(cat $mmPath/AddOns/txSex/DX-MIDI-PORT.txt)"
if test "$1" == "kill"; then
    killall txsex_force 2>/dev/null
else
  $mmPath/AddOns/txSex/txsex_force -p "$port"  2>/dev/null   &
fi

