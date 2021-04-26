# devOS
dev OS

# Getting Started

_Coming Soon_


# Note

## Regenerate and mount devOS.img:

```bash
# create the img
dd if=/dev/zero of=devOS.img bs=512 count=2880

# write the bootload.bin at the begining of the img
dd if=/SysBoot/Stage1/bootloader.bin of=devOS.img conv=notrunc


losetup -f # gives you the first available loop ex: /dev/loop9

# mount the img
sudo losetup /dev/loop9 devOS.img
sudo mkdir /mnt/devOS # if not created yet
sudo mount /dev/loop9 /mnt/devOS

```
