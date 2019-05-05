#   Rock Dodger! Avoid the rocks as long as you can!
#   Copyright (C) 2001  Paul Holt <pad@pcholt.com>

#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.

#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

VERSION=0.6
PACKAGENAME=rockdodger
NEWD=$(PACKAGENAME)-$(VERSION)
TMP=/tmp
OPTIONS=-DVERSION=\"$(VERSION)\" -DCOMPILEDATE=\"28/7/02\"
SOUNDSOURCE=sound
EXENAME=rockdodger

#SOUNDLIBRARIES=-lvorbisfile -lvorbis -logg
#SOUNDLIBRARIES=
SOUNDLIBRARIES=-lSDL_mixer

LIBRARIES=`sdl-config --libs` -lSDL_image $(SOUNDLIBRARIES)

all:	$(EXENAME)

.c.o:
	cc -c -g $? `sdl-config --cflags` $(OPTIONS)

$(EXENAME):	SFont.o $(SOUNDSOURCE).o main.o
	cc -o $(EXENAME) $+ $(LIBRARIES)

clean:
	rm *.o $(EXENAME)

package: $(EXENAME)
	mkdir -p $(TMP)/$(NEWD)/data
	cp *.c *.h Makefile COPYING *.xpm $(TMP)/$(NEWD)
	cp data/*.{png,bmp,wav,mod} $(TMP)/$(NEWD)/data
	( cd $(TMP);  tar zcf $(NEWD).tar.gz $(NEWD) )
	rm -rf $(TMP)/$(NEWD)
	mv $(TMP)/$(NEWD).tar.gz .

install:	all
	# rhk made the point the next line must be a copy, in case /usr/games is on a different partition
	cp $(EXENAME) /usr/games
	mkdir -p /usr/share/rockdodger/data
	cp data/*.{bmp,png,wav,mod} /usr/share/rockdodger/data
	chmod a+r /usr/share/rockdodger/data/*
	chmod a+r /usr/share/rockdodger/data
	touch /usr/share/rockdodger/.highscore
	chmod a+rw /usr/share/rockdodger/.highscore

