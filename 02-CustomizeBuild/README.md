# Customize IFS Image for Raspberry Pi 5 — Adding a C program

---

## 0. Source QNX environment
```bash
source /home/ehab/qnx800/qnxsdp-env.sh
```

---


## 1. Create & Crosscompile your C Program

### nano hello_qnx.c
```c
#include <stdio.h>

int main(){
    printf("Hello QNX!\n");

    return 0;
}
```

### Cross compile
```sh
qcc -Vgcc_ntoaarch64le hello_qnx.c -o hello_qnx
# verify
file hello_qnx
# Expected: hello_qnx: ELF 64-bit LSB executable, ARM aarch64 ...
```

---

## 2. Customize your buildfile

```sh
cd /home/ehab/qnx800/bsp/BSP_raspberrypi-bcm2712-rpi5_be-800_SVN1024006_JBN381/images/
make clean
nano rpi5.build
# [uid=0 gid=0 perms=0755] /bin/hello_qnx=/home/ehab/qnx_dev/hello_qnx
cd ..
make
# or if you would like use: mkifs without the need of make clean
```

> This will generate the bootable QNX image `ifs-rpi5.bin`.

```
images/
├── definitions.m4
├── ifs-rpi5.bin     <== the ifs image (will be loaded in bootable partition)
├── Makefile
├── procnto-smp-instr.sym   <== proconto
├── rpi5.build              <== you can edit the buildfile
└── startup-bcm2712-rpi5.sym    <== startup script
```

---

## 3. Prepare microSD Card

### 3.1 Insert SD Card and Identify Device

```bash
lsblk
```

### 3.2 Unmount if Mounted

```bash
sudo umount /dev/sdX*
```

### 3.3 Partition the SD Card

```bash
sudo cfdisk /dev/sdX
```

Inside `cfdisk`:

1. Delete all existing partitions
2. Create:
   - **Partition 1:**
     - Size: `200M`
     - Type: `FAT16`
     - Mark as **Bootable**
   - **Partition 2:**
     - Will not be used for now
3. Select:
   - `Write` then `Quit`

### 3.4 Format Partitions

```bash
sudo mkfs.vfat -F 16 -n boot /dev/sdb1
```

---

## 4. Mount the Boot Partition

```bash
mkdir -p ~/rpi5_boot
sudo mount /dev/sdX1 ~/rpi5_boot
```

---

## 5. Copy Boot Files

Copy required files from BSP `images` directory:

```bash
cp ifs-rpi5.bin ~/rpi5_boot/
```

``` 
FAT partition should look like this:
├── bcm2712d0-rpi-5-b.dtb
├── bcm2712-rpi-5-b.dtb
├── config.txt
├── fixup4.dat
├── ifs-rpi5.bin
├── overlays
└── start4.elf
```

---

## 6. Sync and Unmount

```bash
sync
sudo umount ~/rpi5_boot
```

---

## 7. Boot Raspberry Pi 5

1. Insert SD card into Raspberry Pi 5
2. Connect:
   - USB To TTL
3. Power on
