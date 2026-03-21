# make file for openpkoc

# (C)2023-2026 Smithee Solutions LLC

all:
	echo Configured for crypto subsytem WolfSSL
	(cd src; make)

clean:
	echo Configured for crypto subsytem WolfSSL
	(cd src; make clean)
	(cd package; make clean)
	rm -rf opt *.deb

package:	all
	(cd package; make package)

