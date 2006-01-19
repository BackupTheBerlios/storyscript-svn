#This makefile is a little dubious.
#I'm not very good at writing makefiles yet. ;)

BASENAME = SSDoc

INPUTFILE = $(BASENAME).tex




DVIFILE = ./dvi/$(BASENAME).dvi
PDFFILE = ./pdf/$(BASENAME).pdf
PSFILE = ./ps/$(BASENAME).ps
HTMLFILE = ./pdf/$(BASENAME).html
BIGHTMLFILE = ./bightml/$(BASENAME).html

$(DVIFILE):
	latex --output-directory=./dvi $(INPUTFILE)
	latex --output-directory=./dvi $(INPUTFILE)

$(PSFILE): $(DVIFILE)
	dvips -o ps/$(BASENAME).ps $(DVIFILE)

#$(PDFFILE): $(DVIFILE)
#	dvipdfmx -o $(PDFFILE) $(DVIFILE)

$(PDFFILE):
	pdflatex --output-directory=./pdf $(INPUTFILE)
	pdflatex --output-directory=./pdf $(INPUTFILE)

$(HTMLFILE): $(DVIFILE)
	latex2html -html_version 4.0,table -dir ./html -external_file ./dvi/$(BASENAME).aux -antialias -antialias_text $(INPUTFILE)
	
$(BIGHTMLFILE): $(DVIFILE)
	latex2html -html_version 4.0,table -dir ./bightml -external_file ./dvi/$(BASENAME).aux -antialias -antialias_text -split 0 $(INPUTFILE)



dvi: $(DVIFILE)

pdf: $(PDFFILE)

html: $(HTMLFILE)

bightml: $(BIGHTMLFILE)

ps: $(PSFILE)


clean:
	rm -f ./pdf/*
	rm -f ./dvi/*
	rm -f ./html/*
	rm -f ./bightml/*
	rm -f ./ps/*
	rm -f *.aux
	rm -f *~
	rm -f *.backup
	rm -f *.log
	rm -f *.bak

#backup:
#	perl MakeBackup.pl -n SSDoc -z 7zip -d ../Backup



