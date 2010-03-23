/* include/plDevs.h.  Generated by configure.  */
/* -*-C-*- */
/* $Id: plDevs.h,v 1.6 2010/03/23 17:13:29 rice Exp $

    Maurice LeBrun
    IFS, University of Texas at Austin
    18-Jul-1994

    Contains macro definitions that determine what device drivers are
    compiled into the PLplot library.  On a Unix system, typically the
    configure script builds plDevs.h from plDevs.h.in.  Elsewhere, it's
    best to hand-configure a plDevs.h file and keep it with the
    system-specific files.

    Copyright (C) 2004  Andrew Roach
    Copyright (C) 2005  Thomas J. Duck
    Copyright (C) 2006  Andrew Ross

    This file is part of PLplot.

    PLplot is free software; you can redistribute it and/or modify
    it under the terms of the GNU Library General Public License as published
    by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    PLplot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with PLplot; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

*/

#ifndef __PLDEVS_H__
#define __PLDEVS_H__

/* #undef PDL_aqt */
#define PLD_plmeta 
#define PLD_null 
#define PLD_xterm
#define PLD_tek4010
#define PLD_tek4010f
#define PLD_tek4107
#define PLD_tek4107f
#define PLD_mskermit
#define PLD_vlt
#define PLD_versaterm
#define PLD_conex
/* #undef PLD_linuxvga */
#undef PLD_dg300
/* #undef PLD_png */ /* pmr: set by build */
/* #define PLD_jpeg */ /* ajb: see gd.c compilation comments for gif below   */
#ifdef PLD_png
#define PLD_gif /* ajb: Only set this if PNG is selected or gd gets compiled */
                /* regardless and will fail on most platforms if PNG isn't   */
                /* there */
#endif
/* #undef PLD_cgm */
#define PLD_ps 
#define PLD_xfig 
#define PLD_ljiip
#define PLD_ljii
#define PLD_lj_hpgl 
#define PLD_hp7470 
#define PLD_hp7580 
#define PLD_imp
/* #undef PLD_tk */
#define PLD_pbm 
/* #undef PLD_gcw */
/* #undef PLD_gnome */
/* #undef PLD_pstex */
/* #undef PLD_psttf */
/* #undef PLD_ntk */
/* #undef PLD_tkwin */
/* #undef PLD_mem */
/* #undef PLD_wingcc */
/* #undef PLD_wxwidgets */
#define PLD_data

#endif	/* __PLDEVS_H__ */
