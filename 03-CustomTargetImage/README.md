# Get Started with QNX Custom Target Image (CTI)

## Supported targets:
- Raspberry pi 4 (64-bit)
- Raspberry pi 5
- Qemu x86 (64-bit)

---

## Hardware requirements
For either the PI4 or PI5 targets you'll need:

- Raspberry Pi 4 - 2GB model or higher OR a Raspberry Pi 5
- Micro SD card - 8GB or more
- (Optional) USB keyboard
- (Optional) USB mouse
- (Optional) HDMI display and micro HDMI to HDMI cable (or touchscreen and
- micro HDMI to HDMI and USB dual cable)
- (Optional) USB-TTL converter
- (Optional) Camera

## Clone & Generate Image for your target

```sh
### First you will QNX Software center installed as its used to downloads the packages

# Ubuntu 22.04
sudo apt install qemu qemu-system-x86 qemu-kvm libvirt-daemon-system libvirt-clients bridge-utils

# Ubuntu 24.04
sudo apt install qemu-system qemu-utils qemu-user qemu-user-binfmt libvirt-daemon-system libvirt-clients bridge-utils

# install other dependencies
sudo apt install automake bridge-utils cmake curl g++ git imagemagick libglib2.0-bin libglib2.0-dev libssl-dev libtool libwayland-bin libzstd-dev lua-zlib lua-zlib-dev make ncat ninja-build pax-utils pkg-config python3-pip sassc scdoc texinfo unzip wget
pip3 install gi-docgen markdown packaging pygments strenum toml tomli typogrify

# Clone repo
git clone https://gitlab.com/qnx/custom-target-image-builds/repo.git

# export the qnx software center path
export SC_CLT_PATH=$HOME/qnx/qnxsoftwarecenter/qnxsoftwarecenter_clt

# Create a file called "options_file"
cd repo
touch options_file

# there is a syntax error in this file: snippets/slm.~60.custom.network, its fixed in the repo above

# Building RPi4
make TARGET=rpi4

# Building RPi5
make TARGET=rpi5

# Building QEMU
make TARGET=qemu

# If the build is successful, it will produce the image file
# in: build/rpi5/rpi5.img
```

---

