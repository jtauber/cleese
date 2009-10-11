# note: this will deliberately fail to make without an empty-disk because
# you need to do 'make empty-disk' then manually setup grub on it before
# running 'make'

BUILD_OS := `uname -s`

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
ifeq ($(BUILD_OS), Darwin)
	hdiutil create -size 5M -fs "MS-DOS" -layout NONE empty
	mv empty.dmg empty.img
	mkdir -p mnt
	mount_msdos -o nosync `hdid -nomount empty.img` ./mnt
else
	dd if=/dev/zero of=empty.img bs=1024 count=5120
	mkfs.msdos empty.img
	mkdir -p mnt
	sudo mount -o loop,user,owner,uid=`id -u` -t msdos empty.img ./mnt
endif
	cp -r boot ./mnt
ifeq ($(BUILD_OS), Darwin)
	umount -f ./mnt
else
	sudo umount ./mnt
endif
	rm -r ./mnt

cleese.img: empty.img hello_world/KERNEL.BIN oneplusone/KERNEL.BIN echo/KERNEL.BIN menu.lst
	cp empty.img cleese.img
	mkdir -p mnt
ifeq ($(BUILD_OS), Darwin)
	mount_msdos -o nosync `hdid -nomount cleese.img` ./mnt
else
	sudo mount -o loop,user,owner,uid=`id -u` -t msdos cleese.img ./mnt
endif
	cp menu.lst ./mnt/boot/grub
	cp hello_world/KERNEL.BIN ./mnt/hello_world.bin 
	cp oneplusone/KERNEL.BIN ./mnt/oneplusone.bin
	cp echo/KERNEL.BIN ./mnt/echo.bin
ifeq ($(BUILD_OS), Darwin)
	umount -f ./mnt
else
	sudo umount ./mnt
endif
	rm -r ./mnt
