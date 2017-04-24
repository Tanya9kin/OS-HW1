#!/bin/bash

echo "Compiling your kernel..."

cd /usr/src/linux-2.4.18-14custom

make bzImage 2> errors.txt 1> /dev/null
if [ $? != 0 ]; then 
	cat errors.txt
	rm -f errors.txt
	return
fi

echo "make bzImage done!"

make modules 2> errors.txt 1> /dev/null
if [ $? != 0 ]; then 
	cat errors.txt
	rm -f errors.txt
	return
fi

echo "make modules done!"

make modules_install 2> errors.txt 1> /dev/null
if [ $? != 0 ]; then 
	cat errors.txt
	rm -f errors.txt
	return
fi

echo "make modules_install done!"

cp -f arch/i386/boot/bzImage /boot/vmlinuz-2.4.18-14custom
cd /boot
mkinitrd -f 2.4.18-14custom.img 2.4.18-14custom 2> errors.txt 1> /dev/null
if [ $? != 0 ]; then 
	cat errors.txt
	rm -f errors.txt
	return
fi

echo "makeinitrd done!"
echo "Compiling was successfull!"
echo "Fasten your seatbelts, we will be rebooting now"

reboot