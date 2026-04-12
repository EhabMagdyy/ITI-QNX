# QNX on Raspberry Pi 5 — Full Setup & Flashing Guide

This guide walks through:

- Installing the BSP
- Building the image
- Preparing the SD card
- Flashing and booting on Raspberry Pi 5

---

## 1. Install QNX BSP (Raspberry Pi 5)

1. Open **QNX Software Center**
2. Go to:
   - `Add Installation`
   - Search for: `BSP Raspberry Pi 5`
3. Click:
   - **Install** => **Finish**

---

## 2. Extract the BSP

Navigate to your QNX installation:

```bash
cd /home/ehab/qnx800/bsp
```

Extract the BSP archive:

```bash
unzip BSP_raspberrypi-bcm2712-rpi5_be-800_SVN1024006_JBN381.zip
```

---

## 3. Build the BSP Image

Go to the BSP directory:

```bash
cd BSP_raspberrypi-bcm2712-rpi5_be-800_SVN1024006_JBN381
```



Then:

```bash
source /home/ehab/qnx800/qnxsdp-env.sh
cd images
make clean
# you can edit the buildfile: images/rpi5.build
cd ..
make
```

> This will generate the bootable QNX image.

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

## 4. Prepare microSD Card

### 4.1 Insert SD Card and Identify Device

```bash
lsblk
```

### 4.2 Unmount if Mounted

```bash
sudo umount /dev/sdX*
```

### 4.3 Partition the SD Card

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

### 4.4 Format Partitions

```bash
sudo mkfs.vfat -F 16 -n boot /dev/sdb1
```

---

## 5. Mount the Boot Partition

```bash
mkdir -p ~/rpi5_boot
sudo mount /dev/sdX1 ~/rpi5_boot
```

---

## 6. Copy Boot Files

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

## 7. Sync and Unmount

```bash
sync
sudo umount ~/rpi5_boot
```

---

## 8. Boot Raspberry Pi 5

1. Insert SD card into Raspberry Pi 5
2. Connect:
   - USB To TTL
3. Power on

---

## Final Summary

| Step | Action |
|------|--------|
| BSP | Provides board-specific support |
| `make` | Builds the QNX IFS image |
| SD Card | Must be partitioned with FAT16 boot partition |
| Copy files | Pi boots directly from SD |
