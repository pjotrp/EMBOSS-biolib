pkgdata_DATA = AlignApplet.java AlignJFrame.java \
ColourJFrame.java ColourMenu.java \
Consensus.java GraphicSequenceCollection.java \
Matrix.java MatrixJFrame.java \
PatternJFrame.java PlotConsensus.java \
PrintAlignment.java PrintAlignmentImage.java \
Sequence.java SequenceJPanel.java \
SequenceNameJButton.java SequenceProperties.java \
SequenceReader.java SequenceSaver.java

pkgdatadir=$(prefix)/share/$(PACKAGE)/jemboss/org/emboss/jemboss/editor

install-exec-hook:
	@if test "${JAVA_OK}" != "yes" ; then \
	  echo "No class files to copy"; \
	else \
	  $(top_srcdir)/mkinstalldirs $(prefix)/share/$(PACKAGE)/jemboss/org/emboss/jemboss/editor; \
	for i in *.class; do \
	  ${INSTALL_DATA} $$i $(prefix)/share/$(PACKAGE)/jemboss/org/emboss/jemboss/editor; \
	done; \
	fi
