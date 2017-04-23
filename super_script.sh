#!/bin/bash

cd /usr/src/linux-2.4.18-14custom
make bzImage 2> errors.txt 1> /dev/null
if [$? != 0]; then 
	cat errors.txt
	return
fi

make modules 2> errors.txt 1> /dev/null
if [$? != 0]; then 
	cat errors.txt
	return
fi

make modules_install 2> errors.txt 1> /dev/null
if [$? != 0]; then 
	cat errors.txt
	return
fi

cp -f arch/i386/boot/bzImage /boot/vmlinuz-2.4.18-14custom
cd /boot
mkinitrd -f 2.4.18-14custom.img 2.4.18-14custom
reboot