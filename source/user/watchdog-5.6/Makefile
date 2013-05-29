all:
	$(MAKE) -C src

romfs:
	$(MAKE) -C src romfs
	$(ROMFSINST)  /etc_ro/watchdog.conf
	$(ROMFSINST)  /sbin/wdg.sh

clean:
	$(MAKE) -C src clean
	
