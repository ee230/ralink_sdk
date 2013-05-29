#!/bin/sh
gcc -O1 -Wall -o usb_modeswitch usb_modeswitch.c -lusb
strip usb_modeswitch
