INSTALL = install
INSTALLSTRIP =
BINDIR = /usr/local/bin
SHAREDIR = /usr/local/share
ETCDIR = /usr/local/etc
BINS = lost_penguins slvextract

default:
	+$(MAKE) -C src
	+$(MAKE) -C tools

lost_penguins:
	+$(MAKE) -C src

tools:
	+$(MAKE) -C tools

docs:
	+$(MAKE) -C docs

clean:
	rm -f $(BINS)
	+$(MAKE) -C src clean
	+$(MAKE) -C tools clean
	+$(MAKE) -C docs clean

distclean: clean
	+$(MAKE) -C src distclean
	+$(MAKE) -C tools distclean
	+$(MAKE) -C docs distclean
	rm -f *~

install: lost_penguins
	$(INSTALL) $(INSTALLSTRIP) -m 755 -o root lost_penguins $(BINDIR)
	if test ! -d $(SHAREDIR)/lost_penguins; then mkdir -p $(SHAREDIR)/lost_penguins; fi
	$(INSTALL) -m 644 -o root data/* $(SHAREDIR)/lost_penguins/
	if test ! -d $(ETCDIR); then mkdir -p $(ETCDIR); fi
	$(INSTALL) -m 644 -o root lost_penguins.conf $(ETCDIR)

uninstall:
	rm -f $(BINDIR)/lost_penguins
	rm -rf $(SHAREDIR)/lost_penguins/
	rm -f $(ETCDIR)/lost_penguins.conf
