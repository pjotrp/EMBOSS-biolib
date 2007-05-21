#
#
!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF

#
# Setup directories
#
PLDIR = ..\plplot
PLTMP = $(PLDIR)\tmp
PLLIB = $(PLDIR)\lib
PLBIN = $(PLDIR)\bin

#
# Setup devices
WIN3 = PLD_win3
LJ_HPGL = PLD_lj_hpgl
PLMETA = PLD_plmeta
PS = PLD_ps
XFIG = PLD_xfig
PDATA = PLD_data
PNG = PLD_png

.IGNORE:
default: copfiles common libraries

prepare: copfiles common

copfiles:
	if not exist "$(PLTMP)/$(NULL)" mkdir "$(PLTMP)"
	@echo ``
	@echo `Copying source files to $(PLTMP)`
	@echo ``
	@copy $(PLDIR)\*.c $(PLTMP)\*.c
	@copy $(PLDIR)\*.h $(PLTMP)\*.h
#
# Copy the local source files to tmp
#
	@copy .\win3.cpp $(PLTMP)
	@copy .\plconsole.cpp $(PLTMP)
	@copy .\plstub.cpp $(PLTMP)
	@copy .\resource.h $(PLTMP)
	@copy .\plplot.h $(PLTMP)
	@copy .\plConfig.h $(PLTMP)
	@copy .\plDevs.h $(PLTMP)
	@copy .\gd\include\*.h $(PLTMP)

common:
	if not exist "$(PLLIB)/$(NULL)" mkdir "$(PLLIB)"
	if not exist "$(PLBIN)/$(NULL)" mkdir "$(PLBIN)"

libraries:
	@nmake -f release.mak
	@nmake -f debug.mak
