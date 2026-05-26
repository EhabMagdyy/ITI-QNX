# QNX 8 CTI - Cannot Connect to Wi-Fi Network on RPi4b

## Run the script manually or put it in the `/system/etc/startup/post_startup.sh` file.
 
```sh
# Put the wifi.sh in /system

# in the post startup script, run the script
/system/wifi &
```

### Fix the missing symlinks for dhcpcd (if needed)
QNX places dhcpcd config files under `/usr/etc/dhcpcd/` but dhcpcd looks for them in
`/etc/` and `/sbin/`.
Create the symlinks to bridge this:
```sh
ln -s /usr/etc/dhcpcd/dhcpcd.conf /etc/dhcpcd.conf
ln -s /usr/etc/dhcpcd/dhcpcd-run-hooks /sbin/dhcpcd-run-hooks
```
