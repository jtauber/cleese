images: grub.img cleese.img

clean:
	-rm -f grub.img cleese.img

grub.img: boot/grub/stage1 boot/grub/stage2
	cat boot/grub/stage1 boot/grub/stage2 > grub.img

cleese.img: hello_world/KERNEL.BIN echo/KERNEL.BIN
	hdiutil create -size 5M -fs "MS-DOS" -layout NONE cleese
	mv cleese.dmg cleese.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp -r boot ./mnt
	cp hello_world/KERNEL.BIN ./mnt/hello_world.bin 
	cp echo/KERNEL.BIN ./mnt/echo.bin
	umount -f ./mnt
	rm -r ./mnt

update-image:
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp hello_world/KERNEL.BIN ./mnt/hello_world.bin 
	cp echo/KERNEL.BIN ./mnt/echo.bin
	umount -f ./mnt
	rm -r ./mnt
