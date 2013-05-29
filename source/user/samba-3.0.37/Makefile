all:
	$(MAKE) -C source

romfs: all
	$(ROMFSINST) source/bin/CP850.so /lib/CP850.so
	$(ROMFSINST) source/bin/CP437.so /lib/CP437.so
	$(ROMFSINST) source/bin/nmbd /sbin/nmbd
	$(ROMFSINST) source/bin/smbd /sbin/smbd
	$(ROMFSINST) source/bin/smbpasswd /sbin/smbpasswd
	$(ROMFSINST) /bin/samba.sh
	$(ROMFSINST) /bin/samba_add_dir.sh

clean:
	$(MAKE) -C source clean
