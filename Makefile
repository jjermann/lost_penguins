BINS = lost_penguins slvextract

default:
	+$(MAKE) -C src
	+$(MAKE) -C tools

lost_penguins:
	+$(MAKE) -C src

tools:
	+$(MAKE) -C tools

clean:
	rm -f $(BINS)
	+$(MAKE) -C src clean
	+$(MAKE) -C tools clean

distclean: clean
	+$(MAKE) -C src distclean
	+$(MAKE) -C tools distclean
	rm -f *~

install:
	cp lost_penguins /usr/local/bin
	mkdir -p /usr/local/share/lost_penguins/
	mkdir -p /usr/local/etc/
	cp lost_penguins.conf /usr/local/etc/
	cp data/* /usr/local/share/lost_penguins/

uninstall:
	rm -f /usr/local/bin/lost_penguins
	rm -rf /usr/local/share/lost_penguins/
	rm -f /usr/local/etc/lost_penguins.conf
