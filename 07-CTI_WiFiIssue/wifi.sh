#!/bin/sh
# WiFi startup script for QNX 8 on RPi 4B (CTI)
# Run as root: sh wifi_start.sh

# Clean up any existing processes
echo "Cleaning up old processes..."
slay dhcpcd 2>/dev/null
slay wpa_supplicant-2.11 2>/dev/null
sleep 2

# Bounce the interface
ifconfig bcm0 down
sleep 1
ifconfig bcm0 up
sleep 1

# Start wpa_supplicant
echo "Starting wpa_supplicant..."
wpa_supplicant-2.11 -B -D qwdi -i bcm0 -c /data/var/etc/settings/wpa_supplicant.con
f

# Wait until actually associated (up to 30 seconds)
echo "Waiting for WiFi association..."
TRIES=0
while [ $TRIES -lt 30 ]; do
    STATUS=$(ifconfig bcm0 | grep "status:")
    echo "  $STATUS"
    case "$STATUS" in
        *"associated"*) echo "Associated!"; break ;;
    esac
    sleep 1
    TRIES=$((TRIES + 1))
done

if [ $TRIES -eq 30 ]; then
    echo "ERROR: Failed to associate after 30 seconds. Check SSID/password."
    exit 1
fi

# Now request DHCP
echo "Requesting DHCP lease..."
dhcpcd -c /usr/etc/dhcpcd/dhcpcd-run-hooks -f /usr/etc/dhcpcd/dhcpcd.conf bcm0

sleep 5
echo "--- Final interface status ---"
ifconfig bcm0
