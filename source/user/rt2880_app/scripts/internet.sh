#!/bin/sh
#
# $Id: internet.sh,v 1.139.2.8 2012-03-07 07:04:49 steven Exp $
#
# usage: internet.sh
#

. /sbin/config.sh
. /sbin/global.sh

genDevNode()
{
#Linux2.6 uses udev instead of devfs, we have to create static dev node by myself.
if [ "$CONFIG_USB_EHCI_HCD" != "" -o "$CONFIG_DWC_OTG" != "" -a "$CONFIG_HOTPLUG" == "y" ]; then
	mounted=`mount | grep mdev | wc -l`
	if [ $mounted -eq 0 ]; then
	mount -t ramfs mdev /dev
	mkdir /dev/pts
	mount -t devpts devpts /dev/pts
        mdev -s

        mknod   /dev/video0      c       81      0
        mknod   /dev/spiS0       c       217     0
        mknod   /dev/i2cM0       c       218     0
        mknod   /dev/flash0      c       200     0
        mknod   /dev/swnat0      c       210     0
        mknod   /dev/hwnat0      c       220     0
        mknod   /dev/acl0        c       230     0
        mknod   /dev/ac0         c       240     0
        mknod   /dev/mtr0        c       250     0
        mknod   /dev/nvram       c       251     0
        mknod   /dev/gpio        c       252     0
        mknod   /dev/rdm0        c       253     0
        mknod   /dev/pcm0        c       233     0
        mknod   /dev/i2s0        c       234     0	
        mknod   /dev/cls0        c       235     0
	
	fi
	echo "# <device regex> <uid>:<gid> <octal permissions> [<@|$|*> <command>]" > /etc/mdev.conf
        echo "# The special characters have the meaning:" >> /etc/mdev.conf
        echo "# @ Run after creating the device." >> /etc/mdev.conf
        echo "# $ Run before removing the device." >> /etc/mdev.conf
        echo "# * Run both after creating and before removing the device." >> /etc/mdev.conf
        echo "sd[a-z][1-9] 0:0 0660 */sbin/automount.sh \$MDEV" >> /etc/mdev.conf
        echo "sd[a-z] 0:0 0660 */sbin/automount.sh \$MDEV" >> /etc/mdev.conf
	if [ "$CONFIG_USB_SERIAL" = "y" ] || [ "$CONFIG_USB_SERIAL" = "m" ]; then
		echo "ttyUSB0 0:0 0660 @/sbin/autoconn3G.sh connect" >> /etc/mdev.conf
	fi
	if [ "$CONFIG_BLK_DEV_SR" = "y" ] || [ "$CONFIG_BLK_DEV_SR" = "m" ]; then
		echo "sr0 0:0 0660 @/sbin/autoconn3G.sh connect" >> /etc/mdev.conf
	fi
	if [ "$CONFIG_USB_SERIAL_HSO" = "y" ] || [ "$CONFIG_USB_SERIAL_HSO" = "m" ]; then
		echo "ttyHS0 0:0 0660 @/sbin/autoconn3G.sh connect" >> /etc/mdev.conf
	fi

        #enable usb hot-plug feature
        echo "/sbin/mdev" > /proc/sys/kernel/hotplug

fi
}

genDevNode

lan_ip=`nvram_get 2860 lan_ipaddr`
stp_en=`nvram_get 2860 stpEnabled`
nat_en=`nvram_get 2860 natEnabled`
radio_off1=`nvram_get 2860 RadioOff`
if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
	radio_off2=`nvram_get rtdev RadioOff`
fi
wifi_off=`nvram_get 2860 WiFiOff`
ra_Bssidnum=`nvram_get 2860 BssidNum`
if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
	rai_Bssidnum=`nvram_get rtdev BssidNum`
fi

set_vlan_map()
{
	# vlan priority tag => skb->priority mapping
	vconfig set_ingress_map $1 0 0
	vconfig set_ingress_map $1 1 1
	vconfig set_ingress_map $1 2 2
	vconfig set_ingress_map $1 3 3
	vconfig set_ingress_map $1 4 4
	vconfig set_ingress_map $1 5 5
	vconfig set_ingress_map $1 6 6
	vconfig set_ingress_map $1 7 7

	# skb->priority => vlan priority tag mapping
	vconfig set_egress_map $1 0 0
	vconfig set_egress_map $1 1 1
	vconfig set_egress_map $1 2 2
	vconfig set_egress_map $1 3 3
	vconfig set_egress_map $1 4 4
	vconfig set_egress_map $1 5 5
	vconfig set_egress_map $1 6 6
	vconfig set_egress_map $1 7 7
}

ifRaxWdsxDown()
{
	num=16
	while [ "$num" -gt 0 ]
	do
		num=`expr $num - 1`
		ifconfig ra$num down 1>/dev/null 2>&1
	done

	ifconfig wds0 down 1>/dev/null 2>&1
	ifconfig wds1 down 1>/dev/null 2>&1
	ifconfig wds2 down 1>/dev/null 2>&1
	ifconfig wds3 down 1>/dev/null 2>&1

	ifconfig apcli0 down 1>/dev/null 2>&1

	ifconfig mesh0 down 1>/dev/null 2>&1
	echo -e "\n##### disable 1st wireless interface #####"
}

ifRaixWdsxDown()
{
	num=16
	while [ "$num" -gt 0 ]
	do
		num=`expr $num - 1`
		ifconfig rai$num down 1>/dev/null 2>&1
	done

	ifconfig wdsi0 down 1>/dev/null 2>&1
	ifconfig wdsi1 down 1>/dev/null 2>&1
	ifconfig wdsi2 down 1>/dev/null 2>&1
	ifconfig wdsi3 down 1>/dev/null 2>&1
	echo -e "\n##### disable 2nd wireless interface #####"
}

addBr0()
{
	brctl addbr br0

# configure stp forward delay
if [ "$wan_if" = "br0" -o "$lan_if" = "br0" ]; then
	stp=`nvram_get 2860 stpEnabled`
	if [ "$stp" = "1" ]; then
		brctl setfd br0 15
		brctl stp br0 1
	else
		brctl setfd br0 1
		brctl stp br0 0
	fi
	enableIPv6dad br0 2
fi

}

addRax2Br0()
{
	num=1 
	brctl addif br0 ra0
        while [ $num -lt $ra_Bssidnum ] 
        do 
                ifconfig ra$num 0.0.0.0 1>/dev/null 2>&1
                brctl addif br0 ra$num 
                num=`expr $num + 1` 
        done 
}

addWds2Br0()
{
	wds_en=`nvram_get 2860 WdsEnable`
	if [ "$wds_en" != "0" ]; then
		ifconfig wds0 up 1>/dev/null 2>&1
		ifconfig wds1 up 1>/dev/null 2>&1
		ifconfig wds2 up 1>/dev/null 2>&1
		ifconfig wds3 up 1>/dev/null 2>&1
		brctl addif br0 wds0
		brctl addif br0 wds1
		brctl addif br0 wds2
		brctl addif br0 wds3
	fi
}

addMesh2Br0()
{
	meshenabled=`nvram_get 2860 MeshEnabled`
	if [ "$meshenabled" = "1" ]; then
		ifconfig mesh0 up 1>/dev/null 2>&1
		brctl addif br0 mesh0
		meshhostname=`nvram_get 2860 MeshHostName`
		iwpriv mesh0 set  MeshHostName="$meshhostname"
	fi
}

addRaix2Br0()
{
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RTDEV" == "" ]; then
		return
	fi
	brctl addif br0 rai0
	num=1
	while [ "$num" -lt "$rai_Bssidnum" ]
	do
		ifconfig rai$num up 1>/dev/null 2>&1
		brctl addif br0 rai$num
		num=`expr $num + 1`
	done
	echo -e "\n##### enable 2nd wireless interface #####"
}

addInicWds2Br0()
{
	if [ "$CONFIG_RT2880_INIC" == "" -a "$CONFIG_RTDEV" == "" ]; then
		return
	fi
	wds_en=`nvram_get rtdev WdsEnable`
	if [ "$wds_en" != "0" ]; then
		ifconfig wdsi0 up 1>/dev/null 2>&1
		ifconfig wdsi1 up 1>/dev/null 2>&1
		ifconfig wdsi2 up 1>/dev/null 2>&1
		ifconfig wdsi3 up 1>/dev/null 2>&1
		brctl addif br0 wdsi0
		brctl addif br0 wdsi1
		brctl addif br0 wdsi2
		brctl addif br0 wdsi3
	fi
}

addRaL02Br0()
{
	if [ "$CONFIG_RT2561_AP" != "" ]; then
		brctl addif br0 raL0
	fi
}

#
#	ipv6 config#
#	$1:	interface name
#	$2:	dad_transmit number
#
enableIPv6dad()
{
	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		echo "2" > /proc/sys/net/ipv6/conf/$1/accept_dad
		echo $2 > /proc/sys/net/ipv6/conf/$1/dad_transmits
	fi
}

disableIPv6dad()
{
	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		echo "0" > /proc/sys/net/ipv6/conf/$1/accept_dad
	fi
}

#
# Enlarge the system memory reclaim threshold to prevent from OOM kernel message
# when WiFi concurrent + storage full loading situation.
enlargeFreeMemReclaim()
{
	if [ "$CONFIG_USER_STORAGE" == "y" ]; then
		echo "Enlarge min_free_kbytes...."
		echo 8192 > /proc/sys/vm/min_free_kbytes
	fi
}

genSysFiles()
{
	login=`nvram_get 2860 Login`
	pass=`nvram_get 2860 Password`
	if [ "$login" != "" -a "$pass" != "" ]; then
	echo "$login::0:0:Adminstrator:/:/bin/sh" > /etc/passwd
	echo "$login:x:0:$login" > /etc/group
		chpasswd.sh $login $pass
	fi
	if [ "$CONFIG_PPPOL2TP" == "y" ]; then
	echo "l2tp 1701/tcp l2f" > /etc/services
	echo "l2tp 1701/udp l2f" >> /etc/services
	fi
}

configVIF()
{
	ifconfig eth2 0.0.0.0
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		ifconfig eth3 up
	fi

	if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
		vconfig rem eth2.1
		vconfig rem eth2.2
		if [ "$CONFIG_RAETH_SPECIAL_TAG" == "y" ]; then
			vconfig rem eth2.3
			vconfig rem eth2.4
			vconfig rem eth2.5
		fi
		rmmod 8021q
		insmod -q 8021q
		vconfig add eth2 1
		set_vlan_map eth2.1
		vconfig add eth2 2
		set_vlan_map eth2.2
		if [ "$CONFIG_RAETH_SPECIAL_TAG" == "y" ]; then
			vconfig add eth2 3
			set_vlan_map eth2.3
			vconfig add eth2 4
			set_vlan_map eth2.4
			vconfig add eth2 5
			set_vlan_map eth2.5

			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				ifconfig eth2.1 down
				wan_mac=`nvram_get 2860 WAN_MAC_ADDR`
				if [ "$wan_mac" != "FF:FF:FF:FF:FF:FF" ]; then
					ifconfig eth2.1 hw ether $wan_mac
				fi
				enableIPv6dad eth2.1 1
			else
				ifconfig eth2.5 down
				wan_mac=`nvram_get 2860 WAN_MAC_ADDR`
				if [ "$wan_mac" != "FF:FF:FF:FF:FF:FF" ]; then
					ifconfig eth2.5 hw ether $wan_mac
				fi
				enableIPv6dad eth2.5 1
			fi
		else
			ifconfig eth2.2 down
			wan_mac=`nvram_get 2860 WAN_MAC_ADDR`
			if [ "$wan_mac" != "FF:FF:FF:FF:FF:FF" ]; then
				ifconfig eth2.2 hw ether $wan_mac
			fi
			enableIPv6dad eth2.2 1
		fi

		ifconfig eth2.1 0.0.0.0
		ifconfig eth2.2 0.0.0.0
		if [ "$CONFIG_RAETH_SPECIAL_TAG" == "y" ]; then
			ifconfig eth2.3 0.0.0.0
			ifconfig eth2.4 0.0.0.0
			ifconfig eth2.5 0.0.0.0
		fi

	elif [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
		#remove ip alias
		# it seems busybox has no command to remove ip alias...
		ifconfig eth2:1 0.0.0.0 1>&2 2>/dev/null
	fi
}

# opmode adjustment:
#   if AP client was not compiled and operation mode was set "3" -> set $opmode "1"
#   if Station was not compiled and operation mode was set "2" -> set $opmode "1"
if [ "$opmode" = "3" -a "$CONFIG_RT2860V2_AP_APCLI$CONFIG_RT3090_AP_APCLI$CONFIG_RT5392_AP_APCLI$CONFIG_RT5592_AP_APCLI$CONFIG_RT3593_AP_APCLI$CONFIG_RT3572_AP_APCLI$CONFIG_RT5572_AP_APCLI$CONFIG_RT3680_iNIC_AP_APCLI$CONFIG_RTPCI_AP_APCLI" == "" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi
if [ "$opmode" = "2" -a "$CONFIG_RT2860V2_STA" == "" ]; then
	nvram_set 2860 OperationMode 1
	opmode="1"
fi

genSysFiles

if [ "$opmode" != "0" ]; then
	echo 1 > /proc/sys/net/ipv4/ip_forward
else
	echo 0 > /proc/sys/net/ipv4/ip_forward
fi

# disable ipv6 DAD on all interfaces by default
if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
	echo "0" > /proc/sys/net/ipv6/conf/default/accept_dad
fi

if [ "$CONFIG_DWC_OTG" == "m" ]; then
usbmod_exist=`lsmod | grep dwc_otg | wc -l`
if [ $usbmod_exist == 0 ]; then
insmod -q lm
insmod -q dwc_otg
fi
fi

if [ "$CONFIG_USB_EHCI_HCD" == "m" ]; then
usbmod_exist=`lsmod | grep ehci-hcd | wc -l`
if [ $usbmod_exist == 0 ]; then
insmod -q ehci-hcd
fi
fi

if [ "$CONFIG_USB_OHCI_HCD" == "m" ]; then
usbmod_exist=`lsmod | grep ohci-hcd | wc -l`
if [ $usbmod_exist == 0 ]; then
insmod -q ohci-hcd
fi
fi

if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
	ifconfig eth3 down
fi
killall -9 watchdog 1>/dev/null 2>&1
rmmod ralink_wdt
rmmod cls
rmmod hw_nat
rmmod raeth

# insmod all
insmod -q bridge
insmod -q mii
insmod -q raeth

# avoid eth2 doing ipv6 DAD
disableIPv6dad eth2


ifRaxWdsxDown
if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
	ifRaixWdsxDown
fi
if [ "$CONFIG_RT2860V2_AP" != "" ]; then
	rmmod rt2860v2_ap_net
	rmmod rt2860v2_ap
	rmmod rt2860v2_ap_util
fi
if [ "$CONFIG_RT2860V2_STA" != "" ]; then
	rmmod rt2860v2_sta_net
	rmmod rt2860v2_sta
	rmmod rt2860v2_sta_util
fi
if [ "$RT2880v2_INIC_PCI" != "" ]; then
	rmmod iNIC_pci
fi
if [ "$CONFIG_RT3090_AP" != "" ]; then
	rmmod RT3090_ap_net
	rmmod RT3090_ap
	rmmod RT3090_ap_util
fi
if [ "$CONFIG_RT5392_AP" != "" ]; then
	rmmod RT5392_ap
fi
if [ "$CONFIG_RT5592_AP" != "" ]; then
	rmmod RT5592_ap
fi
if [ "$CONFIG_RT3593_AP" != "" ]; then
	rmmod RT3593_ap
fi
if [ "$CONFIG_RTPCI_AP" != "" ]; then
	rmmod RTPCI_ap
fi
if [ "$CONFIG_RT3572_AP" != "" ]; then
	rmmod RT3572_ap_net
	rmmod RT3572_ap
	rmmod RT3572_ap_util
fi
if [ "$CONFIG_RT5572_AP" != "" ]; then
	rmmod RT5572_ap_net
	rmmod RT5572_ap
	rmmod RT5572_ap_util
fi
if [ "$RT305x_INIC_USB" != "" ]; then
	rmmod iNIC_usb
fi
if [ "$CONFIG_RT3680_iNIC_AP" != "" ]; then
	rmmod RT3680_ap
fi
if [ "$CONFIG_RT2561_AP" != "" ]; then
	rmmod rt2561ap
fi

if [ "$CONFIG_RT2860V2_AP_WAPI$CONFIG_RT3090_AP_WAPI$CONFIG_RT5392_AP_WAPI$CONFIG_RT5592_AP_WAPI$CONFIG_RT3593_AP_WAPI$CONFIG_RT3572_AP_WAPI$CONFIG_RT5572_AP_WAPI$CONFIG_RT3680_iNIC_AP_WAPI$CONFIG_RTPCI_AP_WAPI" != "" ]; then
	ralink_init gen wapi
fi
ralink_init make_wireless_config rt2860
if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
	ralink_init make_wireless_config rtdev
fi
if [ "$CONFIG_RT2860V2_AP_DFS$CONFIG_RT5592_AP_DFS$CONFIG_RT3593_AP_DFS$CONFIG_RT3572_AP_DFS$CONFIG_RT5572_AP_DFS$CONFIG_RTPCI_AP_DFS" != "" ]; then
	insmod -q rt_timer
fi
if [ "$CONFIG_RT2860V2_STA" != "" -a "$stamode" = "y" ]; then
	insmod -q rt2860v2_sta_util
	insmod -q rt2860v2_sta
	insmod -q rt2860v2_sta_net

	if [ "$CONFIG_RT2860V2_STA_WPA_SUPPLICANT" == "y" ]; then
		ralink_init gen cert
	fi
elif [ "$CONFIG_RT2860V2_AP" != "" ]; then
	insmod -q rt2860v2_ap_util
	insmod -q rt2860v2_ap
	insmod -q rt2860v2_ap_net
fi
# RTDEV_PCI support
if [ "$RT2880v2_INIC_PCI" != "" ]; then
	insmod -q iNIC_pci 
fi
if [ "$CONFIG_RT3090_AP" != "" ]; then
	insmod -q RT3090_ap_util
	insmod -q RT3090_ap
	insmod -q RT3090_ap_net
fi
if [ "$CONFIG_RT5392_AP" != "" ]; then
	insmod -q RT5392_ap
fi
if [ "$CONFIG_RT5592_AP" != "" ]; then
	insmod -q RT5592_ap
fi
if [ "$CONFIG_RT3593_AP" != "" ]; then
	insmod -q RT3593_ap
fi
if [ "$CONFIG_RTPCI_AP" != "" ]; then
	insmod -q RTPCI_ap
fi
# RTDEV_USB support
if [ "$RT305x_INIC_USB" != "" ]; then
	insmod -q iNIC_usb 
fi
if [ "$CONFIG_RT3572_AP" != "" ]; then
	insmod -q RT3572_ap_util
	insmod -q RT3572_ap
	insmod -q RT3572_ap_net
fi
if [ "$CONFIG_RT5572_AP" != "" ]; then
	insmod -q RT5572_ap_util
	insmod -q RT5572_ap
	insmod -q RT5572_ap_net
fi
if [ "$CONFIG_RT3680_iNIC_AP" != "" ]; then
	insmod -q RT3680_ap
fi

# RT2561(Legacy) support
if [ "$CONFIG_RT2561_AP" != "" ]; then
	insmod -q rt2561ap
fi
if [ "$CONFIG_RTDEV_PLC" != "" ]; then
	ifconfig plc0 up
fi
vpn-passthru.sh

# config interface
ifconfig ra0 0.0.0.0 1>/dev/null 2>&1
if [ "$ethconv" = "y" ]; then
	iwpriv ra0 set EthConvertMode=hybrid
	iwpriv ra0 set EthCloneMac=`nvram_get 2860 ethConvertMAC`
fi
if [ "$radio_off1" = "1" ]; then
	iwpriv ra0 set RadioOn=0
fi
if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
	ifconfig rai0 0.0.0.0 1>/dev/null 2>&1
	if [ "$radio_off2" = "1" ]; then
		iwpriv rai0 set RadioOn=0
	fi
fi

ifconfig lo 127.0.0.1
ifconfig br0 down
brctl delbr br0

# stop all
iptables --flush
iptables --flush -t nat
iptables --flush -t mangle


#
# init ip address to all interfaces for different OperationMode:
#   0 = Bridge Mode
#   1 = Gateway Mode
#   2 = Ethernet Converter Mode
#   3 = AP Client
#
if [ "$opmode" = "0" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		echo "##### restore IC+ to dump switch #####"
		config-vlan.sh 0 0
	elif [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		echo "##### restore Vtss to dump switch #####"
		config-vlan.sh 1 0
		if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
			sleep 3
		fi
	elif [ "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_P5_RGMII_TO_MAC_MODE" = "y" ]; then
			echo "##### restore Ralink ESW to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				config-vlan.sh 3 0
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				config-vlan.sh 4 0
			else
				config-vlan.sh 2 0
			fi
			echo "##### restore External Switch to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				echo "initialize external switch"
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then				
				echo "initialize external switch"
			else
				config-vlan.sh 1 0
			fi
		else
			echo "##### restore Ralink ESW to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				config-vlan.sh 3 0
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				config-vlan.sh 4 0
			else
				config-vlan.sh 2 0
			fi
		fi
	fi
	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		sleep 2
	fi
	configVIF
	addBr0
	addRax2Br0
	addWds2Br0
	addMesh2Br0
	APCLI=`nvram_get 2860 apClient`
	if [ "$CONFIG_RT2860V2_AP_APCLI$CONFIG_RT3090_AP_APCLI$CONFIG_RT5392_AP_APCLI$CONFIG_RT5592_AP_APCLI$CONFIG_RT3593_AP_APCLI$CONFIG_RT3572_AP_APCLI$CONFIG_RT5572_AP_APCLI$CONFIG_RT3680_iNIC_AP_APCLI$CONFIG_RTPCI_AP_APCLI" != "" -a "$APCLI" = "1" ]; then
		ifconfig apcli0 up
		brctl addif br0 apcli0
	fi
	brctl addif br0 eth2
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		brctl addif br0 eth3
	fi

# RTDEV_MII support: start mii iNIC after network interface is working
	if [ "$CONFIG_RTDEV_MII" != "" ]; then
		rmmod iNIC_mii
		iNIC_Mii_en=`nvram_get rtdev InicMiiEnable`
		if [ "$iNIC_Mii_en" == "1" ]; then
			ifconfig rai0 down 1>/dev/null 2>&1
			insmod -q iNIC_mii miimaster=eth2
			ifconfig rai0 up 1>/dev/null 2>&1
		fi
	fi

	if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
		addRaix2Br0
		addInicWds2Br0
		addRaL02Br0
		enlargeFreeMemReclaim
	fi
	wan.sh
	lan.sh
elif [ "$opmode" = "1" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
			echo '##### config IC+ vlan partition (WLLLL) #####'
			config-vlan.sh 0 WLLLL
		else
			echo '##### config IC+ vlan partition (LLLLW) #####'
			config-vlan.sh 0 LLLLW
		fi
	fi
	if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
			echo '##### config Vtss vlan partition (WLLLL) #####'
			config-vlan.sh 1 WLLLL
		else
			echo '##### config Vtss vlan partition (LLLLW) #####'
			config-vlan.sh 1 LLLLW
		fi
		if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
			sleep 3
		fi
	fi
	if [ "$CONFIG_RT_3052_ESW" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		if [ "$CONFIG_P5_RGMII_TO_MAC_MODE" = "y" ]; then
			echo "##### restore Ralink ESW to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				config-vlan.sh 3 0 
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				config-vlan.sh 4 0
			else
				config-vlan.sh 2 0
			fi
			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				echo '##### config External Switch vlan partition (WLLLL) #####'
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					echo "initialize external switch (WLLLL)"
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					echo "initialize external switch (WLLLL)"
				else
					config-vlan.sh 1 WLLLL
				fi
			else
				echo '##### config External Switch vlan partition (LLLLW) #####'
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					echo "initialize external switch (LLLLW)"
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					echo "initialize external switch (LLLLW)"
				else
					config-vlan.sh 1 LLLLW
				fi
			fi
		else
			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				echo '##### config Ralink ESW vlan partition (WLLLL) #####'
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					config-vlan.sh 3 WLLLL
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					echo "Do we need to support this mode since we only have two ports?"
				else
					config-vlan.sh 2 WLLLL
				fi
			else
				echo '##### config Ralink ESW vlan partition (LLLLW) #####'
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					config-vlan.sh 3 LLLLW
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					echo "Do we need to support this mode since we only have two ports?"
				else
					config-vlan.sh 2 LLLLW
				fi
			fi
		fi
	fi

	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		sleep 2
	fi

	configVIF
	addBr0
	addRax2Br0
	addWds2Br0
	addMesh2Br0
	if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_RAETH_SPECIAL_TAG" == "y" ]; then
			if [ "$CONFIG_WAN_AT_P4" = "y" ]; then
				brctl addif br0 eth2.1
			fi
			brctl addif br0 eth2.2
			brctl addif br0 eth2.3
			brctl addif br0 eth2.4
			if [ "$CONFIG_WAN_AT_P0" = "y" ]; then
				brctl addif br0 eth2.5
			fi
		else
			brctl addif br0 eth2.1
		fi
	fi

	# IC+ 100 PHY (one port only)
	if [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
		echo '##### connected to one port 100 PHY #####'
		#
		# setup ip alias for user to access web page.
		#
		ifconfig eth2:1 172.32.1.254 netmask 255.255.255.0 up
	fi
	if [ "$CONFIG_GE1_RGMII_AN" = "y" -a "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		echo "##### connected to two Giga PHY port #####"
		brctl addif br0 eth2
	fi
	if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
		addRaix2Br0
		addInicWds2Br0
		addRaL02Br0
		enlargeFreeMemReclaim
	fi
	wan.sh
	lan.sh
	nat.sh

	# set the global ipv6 address for LAN/WAN, enable ipv6 forwarding,
	# enable ecmh(multicast) daemon
elif [ "$opmode" = "2" ]; then
	# if (-1 == initStaProfile())
	#   error(E_L, E_LOG, T("internet.c: profiles in nvram is broken"));
	# else
	#   initStaConnection();

	if [ "$CONFIG_RAETH_ROUTER" = "y" -a "$CONFIG_LAN_WAN_SUPPORT" = "y" ]; then
		echo "##### restore IC+ to dump switch #####"
		config-vlan.sh 0 0
	fi
	if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
		echo "##### restore External Switch to dump switch #####"
		config-vlan.sh 1 0
	fi
	if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_P5_RGMII_TO_MAC_MODE" = "y" ]; then
			echo "##### restore Ralink ESW to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				config-vlan.sh 3 0
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				config-vlan.sh 4 0
			else
				config-vlan.sh 2 0
			fi
			echo "##### restore External Switch to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				echo "initialize external switch"
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				echo "initialize external switch"
			else
				config-vlan.sh 1 0
			fi
		else
			echo "##### restore Ralink ESW to dump switch #####"
			if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
				config-vlan.sh 3 0
			elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
				config-vlan.sh 4 0
			else
				config-vlan.sh 2 0
			fi
		fi
	fi
	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		sleep 2
	fi
	configVIF
	addBr0
	brctl addif br0 eth2
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		brctl addif br0 eth3
	fi
	enableIPv6dad $lan_if 1
	enableIPv6dad $wan_if 1

	if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
		addRaix2Br0
		addInicWds2Br0
		addRaL02Br0
		enlargeFreeMemReclaim
	fi
	wan.sh
	lan.sh
	nat.sh
elif [ "$opmode" = "3" ]; then
	if [ "$CONFIG_RAETH_ROUTER" = "y" -o "$CONFIG_MAC_TO_MAC_MODE" = "y" -o "$CONFIG_RT_3052_ESW" = "y" ]; then
		if [ "$CONFIG_RAETH_ROUTER" = "y" ]; then
			echo "##### restore IC+ to dump switch #####"
			config-vlan.sh 0 0
		fi
		if [ "$CONFIG_MAC_TO_MAC_MODE" = "y" ]; then
			echo "##### restore Vtss to dump switch #####"
			config-vlan.sh 1 0
		fi
		if [ "$CONFIG_RT_3052_ESW" = "y" ]; then
			if [ "$CONFIG_P5_RGMII_TO_MAC_MODE" = "y" ]; then
				echo "##### restore Ralink ESW to dump switch #####"
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					config-vlan.sh 3 0
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					config-vlan.sh 4 0
				else
					config-vlan.sh 2 0
				fi
				echo "##### restore External Switch to dump switch #####"
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					echo "initialize external switch"
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					echo "initialize external switch"
				else
					config-vlan.sh 1 0
				fi
			else
				echo "##### restore Ralink ESW to dump switch #####"
				if [ "$CONFIG_RALINK_RT6855" = "y" -o "$CONFIG_RALINK_RT6855A" = "y" -o "$CONFIG_RALINK_RT6352" = "y" ]; then
					config-vlan.sh 3 0
				elif [ "$CONFIG_RALINK_RT71100" = "y" ]; then
					config-vlan.sh 4 0
				else
					config-vlan.sh 2 0
				fi
			fi
		fi
	fi
	if [ "$CONFIG_IPV6" == "y" -o "$CONFIG_IPV6" == "m" ]; then
		sleep 2
	fi
	configVIF
	addBr0
	addRax2Br0
	brctl addif br0 eth2
	if [ "$CONFIG_GE2_RGMII_AN" = "y" ]; then
		brctl addif br0 eth3
	fi
	if [ "$CONFIG_RTDEV" != "" -o "$CONFIG_RT2561_AP" != "" ]; then
		addRaix2Br0
		addInicWds2Br0
		addRaL02Br0
		enlargeFreeMemReclaim
	fi
	wan.sh
	lan.sh
	nat.sh
else
	echo "unknown OperationMode: $opmode"
	exit 1
fi

# INIC support
#if [ "$CONFIG_RT2880_INIC" != "" ]; then
#	ifconfig rai0 down
#	rmmod rt_pci_dev
#	ralink_init make_wireless_config rtdev
#	insmod -q rt_pci_dev
#	ifconfig rai0 up
#	RaAP&
#	sleep 3
#fi

# in order to use broadcast IP address in L2 management daemon
if [ "$CONFIG_ICPLUS_PHY" = "y" ]; then
	route add -host 255.255.255.255 dev $wan_if
else
	route add -host 255.255.255.255 dev $lan_if
fi


m2uenabled=`nvram_get 2860 M2UEnabled`
if [ "$m2uenabled" = "1" ]; then
	iwpriv ra0 set IgmpSnEnable=1
	echo "iwpriv ra0 set IgmpSnEnable=1"
fi

if [ "$wifi_off" = "1" ]; then
	ifRaxWdsxDown
	reg s b0180000
	reg w 400 0x1080
	reg w 1204 8
	reg w 1004 3
fi

RVT=`nvram_get 2860 RVT`
if [ "$RVT" = "1" ]; then
	insmod -q cls
fi

HWNAT=`nvram_get 2860 hwnatEnabled`
if [ "$HWNAT" = "1" ]; then
	insmod -q hw_nat
fi

WDGE=`nvram_get 2860 WatchDogEnable`
if [ "$WDGE" = "1" ]; then
	insmod -q ralink_wdt
	wdg.sh
	watchdog
fi

#restart8021XDaemon(RT2860_NVRAM);
#firewall_init();
#management_init();
