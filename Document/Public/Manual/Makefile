###############################################################################
# M7M02 (RVM) User Manual (English) -- build rules
###############################################################################

LATEXMK ?= latexmk
DOC      = rvm-manual
OUTDIR   = ../..
TARGET   = $(OUTDIR)/EN_M7M02_R3T1_User_Manual.pdf
SOURCES  = $(DOC).tex rvm-style.sty $(wildcard Chapter/*.tex) \
           $(wildcard Figure/*)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(LATEXMK) -xelatex -interaction=nonstopmode -halt-on-error \
		-outdir=Build $(DOC).tex
	cp Build/$(DOC).pdf $(TARGET)

clean:
	-$(LATEXMK) -C -outdir=Build $(DOC).tex
	-rm -rf Build
	-rm -f $(TARGET)
