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

## Exploring CTI Folders

This section describes the purpose of each directory in the QNX CTI (Custom Target Image) repository.

---

### Top-Level Overview

```
qnxCTI/
├── apk/
├── assets/
├── boot/
├── build/
├── mkqnximage/
├── options_file
├── private/
├── qnx800/
├── snippets/
├── src/
├── system/
├── targets/
└── Makefile
```

---

### `apk/`

**Purpose:** Manages APK (Alpine Package Keeper) packages that are installed into the QNX image at build time.

This is one of the most complex parts of the build system. It handles two categories of packages:

- **QSC packages** — Official QNX packages downloaded from `www.qnx.com` via the QNX Software Center CLI. These are converted from `.qpkg` format into local APK packages that `mkqnximage` can consume.
- **OSS packages** — Open-source packages from `repo.oss.qnx.com`, defined in `world.preinstall_aarch64`.

```
apk/
├── Makefile                        # Drives the entire APK stage
├── build-qnx-apks                  # Script that downloads and converts QSC packages
├── generate-apk-root-snippets      # Generates snippets from the installed APK root
├── world.preinstall_aarch64        # List of OSS APK packages to install (e.g. qt6-base, ncurses)
└── stage/
    ├── apk_root/                   # The fully installed APK filesystem root (usr/, etc/, var/)
    ├── .abuild/                    # APK signing keys and abuild configuration
    ├── abuild/                     # Cloned abuild tool (Alpine build system)
    ├── apk-tools/                  # Cloned APK tools (the apk package manager)
    ├── linux/                      # Host-side APK/abuild binaries (x86_64 Linux)
    ├── qsc-apk/                    # Cloned QSC-to-APK conversion scripts
    ├── snippets/                   # Auto-generated snippets from apk_root contents
    └── fakeroot.save               # Fakeroot state for simulating root ownership
```

**Key files:**
- `world.preinstall_aarch64` — add package names here to install OSS packages (e.g. `qt6-base=6.10.0-r1`)
- `apk_root/` — this is what gets packed into the image; it contains all installed libraries, headers, and binaries

---

### `assets/`

**Purpose:** Stores static assets that are embedded directly into the image — backgrounds, fonts, and icons used by the UI or applications running on the target.

```
assets/
├── Makefile
├── backgrounds/    # Wallpaper or background images
├── fonts/          # Font files (.ttf, .otf)
└── icons/          # Application or system icons
```

These are copied into the image as-is via snippets. No compilation happens here.

---

### `boot/`

**Purpose:** Contains bootloader configuration and files needed to boot the QNX image on the target hardware.

```
boot/
├── Makefile
└── build/
    └── rpi5/       # RPi5-specific boot artifacts (config.txt, bootcode, dtb files, etc.)
```

This directory handles everything before the QNX kernel starts — the GPU firmware, device tree blobs, and bootloader config. For the Raspberry Pi 5, this includes the `config.txt` that controls early hardware initialization.

---

### `build/`

**Purpose:** The output directory where the final image and all intermediate build artifacts are placed. This directory is generated by the build system and should not be manually edited.

```
build/
└── rpi5/
    ├── rpi5.img                # The final flashable image (sparse, ~12GB logical)
    ├── output/                 # Intermediate image layout files
    ├── local/
    │   └── snippets/           # All snippets merged from snippets/, apk/stage/snippets/, and targets/rpi5/snippets/
    ├── bsp/                    # Board Support Package artifacts
    ├── output.txt              # Build log from mkqnximage
    ├── root_authorized_keys    # SSH public keys for passwordless root login
    └── cti_build_version.txt   # Git commit hash + build date stamp
```

The `rpi5.img` is a **sparse file** — its logical size is ~12GB but actual disk usage is only ~5GB. Tools like `dd conv=sparse` handle this correctly when flashing.

---

### `mkqnximage/`

**Purpose:** Contains extensions and overrides for the `mkqnximage` tool — the QNX image generation utility that assembles all the pieces (kernel, drivers, APK packages, snippets) into the final `.img` file.

```
mkqnximage/
└── ...     # Custom hooks and config extensions for mkqnximage
```

The path to this directory is passed to the image builder via `MKQNXIMAGE_EXTRAS`.

---

### `qnx800/`

**Purpose:** The QNX Software Development Platform (SDP) installation. This is the QNX 8.0 toolchain, headers, libraries, and target filesystem needed to build and link QNX software.

```
qnx800/
├── qnxsdp-env.sh               # Environment setup script — must be sourced before building
├── host/
│   └── linux/x86_64/
│       └── usr/bin/            # Cross-compilation tools: qcc, q++, aarch64-unknown-nto-qnx8.0.0-ld, etc.
├── target/
│   └── qnx/
│       ├── aarch64le/
│       │   └── usr/lib/        # QNX target libraries: libc++.so, libc.so, libm.so, etc.
│       └── usr/include/        # QNX system headers
└── jre/                        # Java runtime (used by QNX IDE tools)
```

**This directory is never modified** — it is an installed product from QNX. The `qnxsdp-env.sh` script sets `QNX_HOST`, `QNX_TARGET`, and `MAKEFLAGS` which all downstream Makefiles depend on.

---

### `snippets/`

**Purpose:** Global snippet files that are included in **every** target image regardless of the target platform. Snippets are the mechanism that tells `mkqnximage` which files to include in the image and where to place them.

```
snippets/
├── system_files.custom.xxx     # Files placed in the root filesystem partition
├── data_files.custom.xxx       # Files placed in the /data partition
└── ...
```

**Snippet syntax:**
```
[type=file uid=0 gid=0 perms=0755]
usr/local/bin/hello=${SRC}/stage/nto/aarch64le/usr/local/bin/hello

[type=dir uid=0 gid=0 perms=0755]
data/apps/myapp
```

The format is `destination_in_image=source_on_host`. Three sets of snippets are merged at build time:

1. `snippets/` — global (this directory)
2. `apk/stage/snippets/` — auto-generated from APK packages
3. `targets/<TARGET>/snippets/` — target-specific

---

### `src/`

**Purpose:** The main source build directory. This is where all open-source and custom projects are downloaded, cross-compiled, and staged for inclusion in the image.

```
src/
├── Makefile                        # Main build driver — defines PKGS, -ready and -built targets
├── qconf-override.mk               # QNX build system arch override
├── patches/                        # Patch files and CMake toolchain files
│   ├── aarch64-qnx.cmake           # CMake toolchain for QNX aarch64
│   ├── x86_64-qnx.cmake            # CMake toolchain for QNX x86_64
│   ├── meson/                      # Meson cross-compilation config files
│   └── *.patch                     # Patches applied to various upstream projects
├── local/                          # Projects developed inside the repo (not fetched from internet)
│   └── lottie-screen_thorvg/       # Example local project
├── stage/                          # Staged build outputs (installed here, then packed into image)
│   └── nto/
│       ├── aarch64le/
│       │   └── usr/
│       │       ├── bin/            # Cross-compiled target binaries
│       │       ├── lib/            # Cross-compiled target libraries
│       │       ├── include/        # Headers from built packages
│       │       └── local/bin/      # Installed binaries (bash, hello, lottie-player, etc.)
│       └── usr/
│           └── local/include/      # Architecture-neutral headers (e.g. bash headers)
├── source/                         # Downloaded and built project sources
│   ├── bash/                       # bash source code
│   ├── bash-ready                  # Stamp file — bash downloaded
│   ├── bash-built-aarch64          # Stamp file — bash cross-compiled
│   ├── build-files/                # QNX Ports build-files repo
│   ├── build-files-ready           # Stamp file
│   ├── pattern-race/               # Game source
│   ├── sayhi/                      # Simple hello world app
│   └── ...                         # All other cloned project sources + stamp files
└── qnxuser_projects/               # Sample projects for QNX IDE users
    ├── simple-terminal/
    ├── Maelstrom/
    └── ...
```

The `stage/` directory is the critical output — everything that ends up in the image must pass through here first.

---

### `system/`

**Purpose:** Contains static system configuration files that are embedded into the image as-is — things like SSH config, startup scripts, and other files that need to live in `/etc` or similar locations on the target.

```
system/
└── etc/
    ├── passwd.vncserv      # Pre-generated VNC password file
    └── ...                 # Other system config files
```

These are referenced in snippets using the `${SYSTEM}` variable.

---

### `targets/`

**Purpose:** Per-target configuration — each subdirectory corresponds to a buildable target platform and contains all the platform-specific settings, package lists, snippets, and rules for that target.

```
targets/
└── rpi5/
    ├── mkqnximage.config           # Main image generation config for RPi5
    ├── variables.mk                # Target-specific Makefile variables (QNX_ARCH, QNX_ARCHDIR, etc.)
    ├── rules.mk                    # Build rules specific to this target
    ├── world.preinstall_aarch64    # Target-specific APK packages to install
    ├── qsc_install_packages.list   # QSC packages specific to this target
    └── snippets/                   # Target-specific snippets (RPi5 drivers, board config, etc.)
```

To add a new target platform (e.g. `qemu`), you would create a new directory under `targets/` with its own `mkqnximage.config`, `variables.mk`, and `snippets/`.

The target is selected at build time with `make TARGET=rpi5`.

---

### `Makefile` (root)

**Purpose:** The top-level orchestrator. Running `make TARGET=rpi5` from the repo root triggers this file, which sequentially calls:

1. `make -Capk TARGET=rpi5` — build the APK stage
2. `make -Csrc TARGET=rpi5` — cross-compile all packages
3. `make -Cassets` — process assets
4. `make -Cboot TARGET=rpi5` — prepare boot files
5. `make_image.sh` — assemble everything into the final `.img`

---

### Build Flow Summary

```
make TARGET=rpi5
       │
       ├── apk/Makefile
       │     ├── Download & convert QSC packages → apk/stage/apk_root/
       │     ├── Install OSS APK packages → apk/stage/apk_root/
       │     └── Generate snippets → apk/stage/snippets/
       │
       ├── src/Makefile
       │     ├── Clone & build each PKGS entry
       │     └── Install outputs → src/stage/nto/aarch64le/
       │
       ├── assets/Makefile
       │     └── Process backgrounds, fonts, icons
       │
       ├── boot/Makefile
       │     └── Prepare bootloader files → boot/build/rpi5/
       │
       └── make_image.sh
             ├── Merge all snippets → build/rpi5/local/snippets/
             └── Run mkqnximage → build/rpi5/rpi5.img
```

---

## QNX CTI Build System — Adding a New Package
 
This document explains the `source/<project>-ready` and `source/<project>-built-$(QNX_ARCH)` pattern used in `src/Makefile` to add new software packages to the QNX image.
 
---
 
### Overview
 
Every package follows a three-step pattern:
 
```
1. source/<project>-ready        → Download the source code
2. source/<project>-built-ARCH   → Cross-compile for the target
3. PKGS += <project>             → Register it with the build system
```
 
The build system iterates over the `PKGS` variable and ensures both targets are executed for each entry.
 
---
 
### Step 1 — The Download Target (`source/<project>-ready`)
 
```makefile
BASH_SHA = e1c760dfe65a770fb5cc32f8c4c75d52a1b93ee5
 
source/bash-ready:
    mkdir -p source
    cd source && git clone https://github.com/qnx-ports/bash.git
    cd source/bash && git checkout $(BASH_SHA)
    touch $@
```
 
#### What each line does
 
| Line | Purpose |
|------|---------|
| `mkdir -p source` | Creates the `source/` directory if it doesn't exist. `-p` means no error if it already exists. |
| `git clone <url>` | Downloads the full repository onto your build host. |
| `git checkout $(BASH_SHA)` | Pins the working tree to an exact commit SHA defined at the top of the Makefile. |
| `touch $@` | Creates an empty stamp file named `source/bash-ready`. This tells Make the target is complete. |
 
#### The stamp file trick
 
Make targets normally track completion by checking if an output file exists. Since `git clone` doesn't produce a single named output file, `touch $@` creates an empty file with the target's name (`$@` expands to the target name). Next time you run `make`, it sees the file exists and **skips the entire block** — avoiding redundant re-downloads.
 
#### Pinned SHAs
 
Using a specific commit SHA (`git checkout $(BASH_SHA)`) instead of a branch name guarantees **bit-for-bit reproducible builds** — everyone on the team gets the exact same source code regardless of when they build.
 
#### Variants
 
**From QNX Ports (most common):**
```makefile
source/bash-ready:
    mkdir -p source
    cd source && git clone https://github.com/qnx-ports/bash.git
    cd source/bash && git checkout $(BASH_SHA)
    touch $@
```
 
**With a patch applied before building:**
```makefile
source/SDL_net-ready:
    mkdir -p source
    cd source && git clone https://github.com/libsdl-org/SDL_net.git
    cd source/SDL_net && git checkout $(SDL_NET_VERSION)
    cd source/SDL_net && git apply $(MYDIR)/patches/SDL_net.patch
    touch $@
```
 
**Local project inside the repo (not fetched from internet):**
```makefile
source/lottie-screen_thorvg-ready:
    mkdir -p source
    cp -r $(MYDIR)/local/lottie-screen_thorvg source/
    touch $@
```
 
---
 
### Step 2 — The Build Target (`source/<project>-built-$(QNX_ARCH)`)
 
```makefile
source/bash-built-$(QNX_ARCH): source/bash-ready source/build-files-ready
    QCONF_OVERRIDE=$(MYDIR)/qconf-override.mk \
    CPULIST=$(QNX_ARCH) \
    QNX_ARCH=$(QNX_ARCH) \
    QNX_PROJECT_ROOT="$(MYDIR)/source/bash" make -C source/build-files/ports/bash install -j4
    touch $@
```
 
#### What each part does
 
**Target name with `$(QNX_ARCH)`:**
 
```makefile
source/bash-built-$(QNX_ARCH):
```
 
`$(QNX_ARCH)` expands based on your `TARGET`:
- `make TARGET=rpi5` → `source/bash-built-aarch64`
- `make TARGET=qemu` → `source/bash-built-x86_64`
This makes the stamp file **architecture-aware** — Make treats them as separate targets, so you can build for multiple architectures without one overwriting the other's stamp.
 
**Dependencies (after the colon):**
 
```makefile
source/bash-built-$(QNX_ARCH): source/bash-ready source/build-files-ready
```
 
Make will **not** execute this block until both `source/bash-ready` AND `source/build-files-ready` exist. This enforces correct build ordering automatically.
 
**Environment variables passed to the sub-make:**
 
| Variable | Purpose |
|----------|---------|
| `QCONF_OVERRIDE` | Points to a file that overrides QNX build config to target your specific arch |
| `CPULIST=$(QNX_ARCH)` | Restricts compilation to only `aarch64` — no point building x86 when targeting RPi5 |
| `QNX_ARCH=$(QNX_ARCH)` | Passes the arch down explicitly to the sub-make |
| `QNX_PROJECT_ROOT` | Tells the QNX Ports build system where the source code lives |
 
**The build command:**
 
```makefile
make -C source/build-files/ports/bash install -j4
```
 
- `-C source/build-files/ports/bash` — runs make inside the QNX Ports build-files directory for bash
- `install` — the QNX Ports `install` target compiles and copies outputs to the `stage/` directory
- `-j4` — use 4 parallel jobs to speed up compilation
#### Variants
 
**QNX Ports project (uses build-files):**
```makefile
source/bash-built-$(QNX_ARCH): source/bash-ready source/build-files-ready
    QCONF_OVERRIDE=$(MYDIR)/qconf-override.mk \
    CPULIST=$(QNX_ARCH) \
    QNX_ARCH=$(QNX_ARCH) \
    QNX_PROJECT_ROOT="$(MYDIR)/source/bash" make -C source/build-files/ports/bash install -j4
    touch $@
```
 
**CMake-based project:**
```makefile
source/hello-built-$(QNX_ARCH): source/hello-ready
    mkdir -p source/hello/build-$(QNX_ARCH)
    cd source/hello/build-$(QNX_ARCH) && cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=$(MYDIR)/patches/$(QNX_ARCH)-qnx.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$(STAGE_TARGET)/usr/local
    cd source/hello/build-$(QNX_ARCH) && make -j4
    cd source/hello/build-$(QNX_ARCH) && make install
    touch $@
```
 
**Native QNX make project:**
```makefile
source/rpi-gpio-built-$(QNX_ARCH): source/rpi-gpio-ready
    cd source/rpi-gpio && \
    QCONF_OVERRIDE=$(MYDIR)/qconf-override.mk \
    QNX_ARCH=$(QNX_ARCH) \
    make hinstall
    cd source/rpi-gpio && \
    EXTRA_INCVPATH=$(STAGE_TARGET)/usr/include \
    EXTRA_LIBVPATH=$(STAGE_TARGET)/usr/lib \
    MY_STAGE=$(STAGE_ROOT) make
    cd source/rpi-gpio && \
    QCONF_OVERRIDE=$(MYDIR)/qconf-override.mk \
    QNX_ARCH=$(QNX_ARCH) \
    EXTRA_INCVPATH=$(STAGE_TARGET)/usr/include \
    EXTRA_LIBVPATH=$(STAGE_TARGET)/usr/lib \
    MY_STAGE=$(STAGE_ROOT) make install
    touch $@
```
 
---
 
### Step 3 — Register in `PKGS`
 
```makefile
PKGS = thorvg lottie-screen_thorvg simple-terminal bash hello
```
 
The top-level `all` target expands this into:
 
```makefile
source/bash-built-$(QNX_ARCH)
source/hello-built-$(QNX_ARCH)
# ... etc
```
 
For platform-specific packages (RPi only):
 
```makefile
ifneq ($(filter rpi4 rpi5,$(TARGET)),)
PKGS += rpi-gpio rpi-mailbox rpi-thermal
endif
```
 
---
 
### Step 4 — Add a Snippet to Include in the Image
 
After the binary is staged, tell `mkqnximage` to pack it into the `.img`:
 
**For a single binary:**
```
[type=file uid=0 gid=0 perms=0755]
usr/local/bin/hello=${SRC}/stage/nto/aarch64le/usr/local/bin/hello
```
 
**For a binary with assets:**
```
[type=dir uid=0 gid=0 perms=0755]
data/apps/myapp
 
[type=file uid=0 gid=0 perms=0755]
data/apps/myapp/myapp=${SRC}/stage/nto/aarch64le/usr/bin/myapp/myapp
 
[type=dir uid=0 gid=0 perms=0755]
data/apps/myapp/assets
```
 
The format is `destination_in_image=source_on_host`.
 
---
 
### Key Variables Reference
 
| Variable | Expands to (rpi5 example) |
|----------|--------------------------|
| `$(QNX_ARCH)` | `aarch64` |
| `$(QNX_ARCHDIR)` | `aarch64le` |
| `$(MYDIR)` | `/home/ehab/qnxCTI/src/` |
| `$(STAGE_ROOT)` | `/home/ehab/qnxCTI/src/stage/nto` |
| `$(STAGE_TARGET)` | `/home/ehab/qnxCTI/src/stage/nto/aarch64le` |
| `$(APK_STAGE_ROOT)` | `/home/ehab/qnxCTI/apk/stage/apk_root` |
| `$@` | The current target name (used in `touch $@`) |
 
---
 
### Complete Example — Adding a Simple C App
 
**1. Add SHA variable at the top of `src/Makefile`:**
```makefile
HELLO_SHA = 02304dac1b655e592f1d290347c6f7948254d1aa
```
 
**2. Add download target:**
```makefile
source/sayhi-ready:
    mkdir -p source
    cd source && git clone https://github.com/EhabMagdyy/Hello-QNX.git sayhi
    cd source/sayhi && git checkout $(HELLO_SHA)
    touch $@
```
 
**3. Add build target:**
```makefile
source/sayhi-built-$(QNX_ARCH): source/sayhi-ready
    mkdir -p source/sayhi/build-$(QNX_ARCH)
    cd source/sayhi/build-$(QNX_ARCH) && cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=$(MYDIR)/patches/$(QNX_ARCH)-qnx.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=$(STAGE_TARGET)/usr/local
    cd source/sayhi/build-$(QNX_ARCH) && make -j4
    cd source/sayhi/build-$(QNX_ARCH) && make install
    touch $@
```
 
**4. Add to PKGS:**
```makefile
PKGS = thorvg lottie-screen_thorvg simple-terminal bash sayhi
```
 
**5. Add snippet (`snippets/sayhi.snippet`):**
```
[type=file uid=0 gid=0 perms=0755]
usr/local/bin/hello=${SRC}/stage/nto/aarch64le/usr/local/bin/hello
```
 