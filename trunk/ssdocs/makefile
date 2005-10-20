
BASENAME = SSDoc

INPUTFILE = $(BASENAME).tex


DVIFILE = ./dvi/$(BASENAME).dvi
PDFFILE = ./pdf/$(BASENAME).pdf
HTMLFILE = ./pdf/$(BASENAME).html

$(DVIFILE):
	latex --output-directory=./dvi $(INPUTFILE)
	latex --output-directory=./dvi $(INPUTFILE)

#$(PDFFILE): $(DVIFILE)
#	dvipdfmx -o $(PDFFILE) $(DVIFILE)

$(PDFFILE):
	pdflatex --output-directory=./pdf $(INPUTFILE)
	pdflatex --output-directory=./pdf $(INPUTFILE)

$(HTMLFILE):
	latex2html -dir ./html $(INPUTFILE)



dvi: $(DVIFILE)

pdf: $(PDFFILE)

html: $(HTMLFILE)


clean:
	rm -f ./pdf/*
	rm -f ./dvi/*
	rm -f ./html/*

backup:
	perl MakeBackup.pl -n SSDoc -z 7zip -d ../Backup





