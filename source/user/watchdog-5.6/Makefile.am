
man_MANS = watchdog.8 wd_keepalive.8 watchdog.conf.5

# This does not work. subdirs are not copied correctly
# for make dist... :(
EXTRA_DIST = $(man_MANS) include examples ChangeLog watchdog.lsm \
		watchdog.conf rc.watchdog.debian rc.watchdog.redhat

SUBDIRS = src 

CONFIG_FILENAME = @CONFIG_FILENAME@

install-etc-local: watchdog.conf
	@if test -e $(DESTDIR)$(CONFIG_FILENAME) ; then \
	        echo "NOT installing $(CONFIG_FILENAME) - exists already"; \
	else \
	        echo "installing $(CONFIG_FILENAME)"; \
		$(mkinstalldirs) `dirname $(DESTDIR)$(CONFIG_FILENAME)`; \
	        $(INSTALL_DATA) watchdog.conf $(DESTDIR)$(CONFIG_FILENAME); \
	fi

install-data-local: install-etc-local
