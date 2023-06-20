# openpkoc - Make file for this "plug-in" only

# (C)2023 Smithee Solutions LLC

all:
	(cd src; make)

clean:
	(cd src; make clean)
	rm -f opt

