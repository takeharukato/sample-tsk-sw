BIBLIONAME=bibliography
MASTERNAME=task-monitor
DISTNAME=task-monitor
SOURCE=${MASTERNAME}.tex os-references.bib
FIGS=
EXTRAFILES=
TEX=${MASTERNAME}.tex
IDX=${MASTERNAME}.idx
DVI=${MASTERNAME}.dvi
PDF=${MASTERNAME}.pdf
BBL=${MASTERNAME}.bbl

LATEX=platex
BIBTEX=pbibtex
MAKEINDEX=mendex
DVIPDF=dvipdfmx
PDFVIEW=evince

all: ${PDF} bibliography.pdf

${BIBLIONAME}.pdf:${BIBLIONAME}.dvi
	${DVIPDF} $<

${BIBLIONAME}.dvi: ${BIBLIONAME}.tex  os-references.bib
	${LATEX} ${BIBLIONAME}
	${BIBTEX} ${BIBLIONAME}
	${LATEX} ${BIBLIONAME}
	${LATEX} ${BIBLIONAME}

${PDF}: ${DVI}
	${MAKE} dvipdf

${DVI}: ${SOURCE} ${FIGS}
	${MAKE} tex
	${MAKE} bbl
	${MAKE} idx
	${MAKE} tex
	${MAKE} tex

distclean:

tex:
	${LATEX} ${TEX}

idx:
	${MAKEINDEX} ${IDX}

bbl:
	${BIBTEX} ${MASTERNAME}

dvipdf:
	${DVIPDF} ${DVI}

clean:
	${RM} -f *.aux *~ \#* *.log *.toc *.lof *.lot *.bbl *.blg *.ilg *.ind *.idx *.out ${DVI} \
		${PDF} tmp.txt	

show: ${PDF}
	${PDFVIEW} ${PDF}

cleanarc:
	${RM} -f ${MASTERNAME}.tgz

distclean: cleanarc clean
	${RM} ${PDF}

dist: distclean
	${TAR} cvf ${MASTERNAME}.tar ${SOURCE} ${FIGS} ${EXTRAFILES} Makefile
	${GZIP} -9 ${MASTERNAME}.tar
	${MV} ${MASTERNAME}.tar.gz ${DISTNAME}.tgz


