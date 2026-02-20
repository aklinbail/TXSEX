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
  # Use 'chrt' to give the app Real-Time priority (99)
  # This stops the Akai OS from "starving" your MIDI thread
  $mmPath/AddOns/txSex/txsex_force -p "$port" > /dev/null 2>&1 &
fi