pkgdata_DATA = AlignApplet.java GroutSequenceJPanel.java \
AlignJFrame.java                             Matrix.java \
ColourJFrame.java                            MatrixJFrame.java \
ColourMenu.java                              PatternJFrame.java \
Consensus.java                               PlotConsensus.java \
GraphicSequenceCollection.java               PrintAlignment.java \
GroutAlignJFrame.java                        PrintAlignmentImage.java \
GroutAlignJFrameEvent.java                   Sequence.java \
GroutAlignJFrameListener.java                SequenceJPanel.java \
GroutGraphicSequenceCollection.java          SequenceNameJButton.java \
GroutGraphicSequenceCollectionEvent.java     SequenceProperties.java \
GroutGraphicSequenceCollectionListener.java  SequenceReader.java \
GroutSequence.java                           SequenceSaver.java 


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
