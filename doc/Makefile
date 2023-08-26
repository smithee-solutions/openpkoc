# make file for markdown documents in doc directory of openpkoc

PUBLISHED=pkoc-osdp-acu.pdf

all:	${PUBLISHED}

clean:
	rm -f ${PUBLISHED}

pkoc-osdp-acu.pdf:	../spec-src/pkoc-osdp-acu.md
	pandoc --toc -o pkoc-osdp-acu.pdf ../spec-src/pkoc-osdp-acu.md
