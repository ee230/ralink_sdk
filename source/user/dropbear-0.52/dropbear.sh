#!/bin/sh

	mkdir -p /etc/dropbear

#Check for the Dropbear RSA key
if [ ! -f /etc/dropbear/dropbear_rsa_host_key ] ; then
        echo Generating RSA Key...
        dropbearkey -t rsa -f /etc/dropbear/dropbear_rsa_host_key
fi

# Check for the Dropbear DSS key
if [ ! -f /etc/dropbear/dropbear_dss_host_key ] ; then
        echo Generating DSS Key...
        dropbearkey -t dss -f /etc/dropbear/dropbear_dss_host_key
fi

dropbear
