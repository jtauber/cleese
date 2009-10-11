# note: this will deliberately fail to make without an empty-disk because
# you need to do 'make empty-disk' then manually setup grub on it before
# running 'make'

all: cleese.img

.PHONY: empty-disk clean clobber

clobber: clean
	-rm -f grub.img empty.img

clean:
	-rm -f cleese.img
	$(MAKE) -C hello_world clean
	$(MAKE) -C oneplusone clean
	$(MAKE) -C echo clean

grub.img: boot/grub/stage1 boot/grub/stage2
	cat boot/grub/stage1 boot/grub/stage2 > grub.img

hello_world/KERNEL.BIN:
	$(MAKE) -C hello_world

oneplusone/KERNEL.BIN:
	$(MAKE) -C oneplusone

echo/KERNEL.BIN:
	$(MAKE) -C echo

empty-disk:
	hdiutil create -size 5M -fs "MS-DOS" -layout NONE empty
	mv empty.dmg empty.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount empty.img` ./mnt
	cp -r boot ./mnt
	umount -f ./mnt
	rm -r ./mnt

cleese.img: empty.img hello_world/KERNEL.BIN oneplusone/KERNEL.BIN echo/KERNEL.BIN menu.lst
	cp empty.img cleese.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
	cp menu.lst ./mnt/boot/grub
	cp hello_world/KERNEL.BIN ./mnt/hello_world.bin 
	cp oneplusone/KERNEL.BIN ./mnt/oneplusone.bin
	cp echo/KERNEL.BIN ./mnt/echo.bin
	umount -f ./mnt
	rm -r ./mnt
