# (C)2023-2025 Smithee Solutions LLC

all:
	echo Configured for crypto subsytem cyclone
	cp include/ob-crypto_cyclone.h include/ob-crypto.h
	(cd src; make)

clean:
	echo Configured for crypto subsytem cyclone
	rm -f include/ob-crypto.h
	(cd src; make clean)
	(cd package; make clean)
	rm -rf opt *.deb

package:	all
	(cd package; make package)

