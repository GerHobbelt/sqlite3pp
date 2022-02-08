MODULE = sqlite3pp

FILES=sqlite3ppext.h sqlite3ppext.ipp sqlite3pp.h sqlite3pp.ipp

include $(shell echo $${PREFIX-/usr})/share/smartmet/devel/makefile.inc

.PHONY: rpm

all:

clean:
	rm -f *~
	rm -f $(MODULE).tar.gz

install:
	mkdir -p $(includedir)/sqlite3pp
	@list='$(FILES)'; \
	for file in $$list; do \
	 echo $(INSTALL_DATA) headeronly_src/$$file $(includedir)/sqlite3pp/$$file; \
	 $(INSTALL_DATA) headeronly_src/$$file $(includedir)/sqlite3pp/$$file; \
	done

rpm:	clean
	rm -f $(MODULE).tar.gz
	tar -czvf $(MODULE).tar.gz --exclude-vcs --transform "s,^,$(MODULE)/," *
	rpmbuild --target noarch -tb $(MODULE).tar.gz
	rm -f $(MODULE).tar.gz

