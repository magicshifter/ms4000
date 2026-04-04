#!/bin/sh
# INFO: uploads the firmware (firmwareName) to the MS3000, uses correct resetmethods so it's hads free
# ATTN expects https://github.com/igrr/esptool-ck in the parent directory. You have to "make" this to work
# baudrates: 115200 460800 921600
#
# Example:
# ./writeFirmware.sh /dev/ttyUSB0 ./arsFirmware_GOLD_2.0.0.bin


if [ "$#" -ne 2 ]; then
	firmwareName=MS3000_Firmware.bin
else
	firmwareName=$2
fi	

echo "Firmware Name: " $firmwareName
../esptool-ck/esptool -vv -cd ck -cb 921600 -cp $1  -cf $firmwareName

## If you encounter a stubborn shifter, then comment out the line with the 921600
## and try flashing using decreasing baud rates. M<aybe make some tea since
## this might take some time...
## This method usually works to revive "bricked" shifters :)

# ../esptool-ck/esptool -vv -cd ck -cb 460800 -cp $1  -cf $firmwareName
# ../esptool-ck/esptool -vv -cd ck -cb 115200 -cp $1  -cf $firmwareName
# ../esptool-ck/esptool -vv -cd ck -cb 57600 -cp $1  -cf $firmwareName
# ../esptool-ck/esptool -vv -cd ck -cb 38400 -cp $1  -cf $firmwareName
# ../esptool-ck/esptool -vv -cd ck -cb 19200 -cp $1  -cf $firmwareName
# ../esptool-ck/esptool -vv -cd ck -cb 9600 -cp $1  -cf $firmwareName


## If this still does not work and you feel like giving up:
## *** Always remember - it will get better again, I promise *hug* :) ***
## Never give up! And if you realy don't know what to do just call a friend.
## Don't worry about not having called for soooo long. Just call and you will
## find out that all humans love getting in touch again after a long 
## long time :) Just talk about whatever comes to mind :)
