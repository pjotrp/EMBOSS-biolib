#!/bin/csh
/bin/rm Package-org *html stylesheet.css org/emboss/jemboss/* org/emboss/jemboss/gui/* org/emboss/jemboss/soap/* org/emboss/jemboss/programs/* org/emboss/jemboss/parser/* org/emboss/jemboss/gui/form/* org/emboss/jemboss/gui/sequenceChooser/* org/emboss/jemboss/parser/acd/* org/emboss/jemboss/gui/filetree/* org/emboss/jemboss/gui/startup/*  org/emboss/jemboss/editor/*

setenv LIB ../lib

javadoc -author -version -windowtitle "Jemboss API" -classpath $LIB/axis/axis.jar:$LIB/axis/jaxrpc.jar:$LIB/axis/saaj.jar:$LIB/axis/commons-logging.jar:$LIB/axis/commons-discovery.jar:$LIB/jakarta-regexp-1.2.jar:$LIB/jalview.jar:$LIB/jnet.jar:$LIB/jcert.jar:$LIB/jsse.jar:../:$LIB/axis/servlet.jar:$LIB/xerces.jar: -private \
 org/emboss/jemboss/Jemboss  \
 org/emboss/jemboss/JembossJarUtil  \
 org/emboss/jemboss/JembossLogo  \
 org/emboss/jemboss/JembossParams  \
 org/emboss/jemboss/editor/AlignApplet  \
 org/emboss/jemboss/editor/AlignJFrame  \
 org/emboss/jemboss/editor/ColourJFrame  \
 org/emboss/jemboss/editor/ColourMenu  \
 org/emboss/jemboss/editor/Consensus  \
 org/emboss/jemboss/editor/GraphicSequenceCollection  \
 org/emboss/jemboss/editor/Matrix  \
 org/emboss/jemboss/editor/MatrixJFrame  \
 org/emboss/jemboss/editor/PatternJFrame  \
 org/emboss/jemboss/editor/PlotConsensus  \
 org/emboss/jemboss/editor/PrintAlignment  \
 org/emboss/jemboss/editor/PrintAlignmentImage  \
 org/emboss/jemboss/editor/Sequence  \
 org/emboss/jemboss/editor/SequenceJPanel  \
 org/emboss/jemboss/editor/SequenceNameJButton  \
 org/emboss/jemboss/editor/SequenceProperties  \
 org/emboss/jemboss/editor/SequenceReader  \
 org/emboss/jemboss/editor/SequenceSaver  \
 org/emboss/jemboss/gui/AdvancedOptions  \
 org/emboss/jemboss/gui/Browser  \
 org/emboss/jemboss/gui/BuildProgramMenu  \
 org/emboss/jemboss/gui/LaunchJalView  \
 org/emboss/jemboss/gui/MemoryComboBox  \
 org/emboss/jemboss/gui/ResultsMenuBar  \
 org/emboss/jemboss/gui/ScrollPanel  \
 org/emboss/jemboss/gui/SequenceData  \
 org/emboss/jemboss/gui/SequenceList  \
 org/emboss/jemboss/gui/SetUpMenuBar  \
 org/emboss/jemboss/gui/ShowResultSet  \
 org/emboss/jemboss/gui/ShowSavedResults  \
 org/emboss/jemboss/gui/SplashThread  \
 org/emboss/jemboss/gui/SwingWorker  \
 org/emboss/jemboss/gui/filetree/DragTree  \
 org/emboss/jemboss/gui/filetree/FileEditorDisplay  \
 org/emboss/jemboss/gui/filetree/FileNode  \
 org/emboss/jemboss/gui/filetree/FileSave  \
 org/emboss/jemboss/gui/filetree/FileSaving  \
 org/emboss/jemboss/gui/filetree/LocalAndRemoteFileTreeFrame  \
 org/emboss/jemboss/gui/filetree/RemoteDragTree  \
 org/emboss/jemboss/gui/filetree/RemoteFileNode  \
 org/emboss/jemboss/gui/filetree/RemoteFileTreePanel  \
 org/emboss/jemboss/gui/form/AlignFormat  \
 org/emboss/jemboss/gui/form/ApplicationNamePanel  \
 org/emboss/jemboss/gui/form/BuildJembossForm  \
 org/emboss/jemboss/gui/form/LabelTextBox  \
 org/emboss/jemboss/gui/form/ListFilePanel  \
 org/emboss/jemboss/gui/form/MultiLineToolTipUI  \
 org/emboss/jemboss/gui/form/PlafMacros  \
 org/emboss/jemboss/gui/form/ReportFormat  \
 org/emboss/jemboss/gui/form/SectionPanel  \
 org/emboss/jemboss/gui/form/Separator  \
 org/emboss/jemboss/gui/form/SetInFileCard  \
 org/emboss/jemboss/gui/form/SetOutFileCard  \
 org/emboss/jemboss/gui/form/TextFieldFloat  \
 org/emboss/jemboss/gui/form/TextFieldInt  \
 org/emboss/jemboss/gui/form/TextFieldSink  \
 org/emboss/jemboss/gui/form/myComboPopup  \
 org/emboss/jemboss/gui/sequenceChooser/CutNPasteTextArea  \
 org/emboss/jemboss/gui/sequenceChooser/FileChooser  \
 org/emboss/jemboss/gui/sequenceChooser/InputSequenceAttributes  \
 org/emboss/jemboss/gui/sequenceChooser/OutputSequenceAttributes  \
 org/emboss/jemboss/gui/sequenceChooser/SequenceFilter  \
 org/emboss/jemboss/gui/sequenceChooser/TextAreaSink  \
 org/emboss/jemboss/gui/startup/Database  \
 org/emboss/jemboss/gui/startup/ProgList  \
 org/emboss/jemboss/parser/AcdFunResolve  \
 org/emboss/jemboss/parser/AcdVarResolve  \
 org/emboss/jemboss/parser/AcdVariableResolve  \
 org/emboss/jemboss/parser/Ajax  \
 org/emboss/jemboss/parser/AjaxUtil  \
 org/emboss/jemboss/parser/Dependent  \
 org/emboss/jemboss/parser/ParseAcd  \
 org/emboss/jemboss/parser/acd/Application  \
 org/emboss/jemboss/parser/acd/ApplicationFields  \
 org/emboss/jemboss/parser/acd/ApplicationParam  \
 org/emboss/jemboss/programs/BatchUpdateTimer  \
 org/emboss/jemboss/programs/JembossProcess  \
 org/emboss/jemboss/programs/ListFile  \
 org/emboss/jemboss/programs/ResultList  \
 org/emboss/jemboss/programs/ResultsUpdateTimer  \
 org/emboss/jemboss/programs/RunEmbossApplication  \
 org/emboss/jemboss/programs/RunEmbossApplication2  \
 org/emboss/jemboss/server/JembossAuthServer  \
 org/emboss/jemboss/server/JembossAuthThread  \
 org/emboss/jemboss/server/JembossFileAuthServer  \
 org/emboss/jemboss/server/JembossFileServer  \
 org/emboss/jemboss/server/JembossServer  \
 org/emboss/jemboss/server/JembossThread  \
 org/emboss/jemboss/server/TestPrivateServer  \
 org/emboss/jemboss/server/TestPublicServer  \
 org/emboss/jemboss/soap/AuthPopup  \
 org/emboss/jemboss/soap/CallAjax  \
 org/emboss/jemboss/soap/FileList  \
 org/emboss/jemboss/soap/FileRoots  \
 org/emboss/jemboss/soap/GetACD  \
 org/emboss/jemboss/soap/GetHelp  \
 org/emboss/jemboss/soap/GetWossname  \
 org/emboss/jemboss/soap/JembossRun  \
 org/emboss/jemboss/soap/JembossSoapException  \
 org/emboss/jemboss/soap/MakeFileSafe  \
 org/emboss/jemboss/soap/PendingResults  \
 org/emboss/jemboss/soap/PrivateRequest  \
 org/emboss/jemboss/soap/PublicRequest  \
 org/emboss/jemboss/soap/ServerSetup  \
 org/emboss/jemboss/soap/ShowDB 





