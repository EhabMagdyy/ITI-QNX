# Toggle RPi GPIO via C Program

## 1. QNX Momentics IDE

1. File > New > QNX Project > QNX Executable
2. Enter Project name & From CPU Varients: Select aarch64le
3. Modify `Makefile` with the one in the `repo` to include `gpio` libs
4. Write your Code logic & Build
> the executable will be at: `cntrlgpio/build/aarch64le-debug/cntrlgpio`

## 2. Include the binary in your image & make it start on startup

- Create `system_files.xxxx` file to add you binary in the image
```sh
# [type=file uid=0 gid=0 perms=0755 search="directory of the executable"] Destination location & name in the image = file    
[type=file uid=0 gid=0 perms=0755 search=/home/ehab/Documents/ITI_9Months/QNX/cntrlgpio/build/aarch64le-debug/] usr/bin/cntrlgpio=cntrlgpio
```

- Create `post_start.xxxx` file to add your command that run the binary:
```sh
/system/usr/bin/cntrlgpio &
```
