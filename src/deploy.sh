nasm -f bin boot2.asm -o boot2.bin
dd if=/dev/zero of=floppy.img bs=512 count=2880
dd if=boot2.bin of=floppy.img conv=notrunc
sudo bochs -f bochs.conf