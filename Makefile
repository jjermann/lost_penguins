default:
	+$(MAKE) -C src
	+$(MAKE) -C tools

lost_vikings:
	+$(MAKE) -C src

tools:
	+$(MAKE) -C tools

clean:
	+$(MAKE) -C src clean
	+$(MAKE) -C tools clean

distclean:
	+$(MAKE) -C src distclean
	+$(MAKE) -C tools distclean
	rm -f *~

install:
	cp lost_vikings /usr/local/bin
	mkdir -p /usr/local/share/lost_vikings/
	mkdir -p /usr/local/etc/
	cp lost_vikings.conf /usr/local/etc/
	cp data/* /usr/local/share/lost_vikings/

uninstall:
	rm -f /usr/local/bin/lost_vikings
	rm -rf /usr/local/share/lost_vikings/
	rm -f /usr/local/etc/lost_vikings.conf
