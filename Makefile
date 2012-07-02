PWD:=$(shell pwd)

WATCHDOG_CTL:=watchdog-ctl
WATCHDOG_CTL_PATH:=$(PWD)/src/$(WATCHDOG_CTL)
WATCHDOG_CTL_DESTINATION_PATH:=/usr/bin/$(WATCHDOG_CTL)

all:
	cd src && $(MAKE) $@

run:
	cd src && $(MAKE) $@

clean:
	cd src && $(MAKE) $@

install:
	cp $(WATCHDOG_CTL_PATH) $(WATCHDOG_CTL_DESTINATION_PATH)

uninstall:
	rm $(WATCHDOG_CTL_DESTINATION_PATH)

update: uninstall install
