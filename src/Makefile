#
# SAGS - Secure Administrator of Game Servers
# Copyright (C) 2004 Pablo Carmona Amigo
# 
# This file is part of SAGS Client.
#
# SAGS Client is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# SAGS Client is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Library General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Attic/Makefile,v $
# $Revision: 1.1 $
# $Date: 2004/04/13 22:01:53 $
#

CC = g++ -Wall -g -O2
CFLAGS += `wx-config --cxxflags`
LDFLAGS += `wx-config --libs` -lssl
#CFLAGS += `wxgtk2-2.5-config --cxxflags`
#LDFLAGS += `wxgtk2-2.5-config --libs` -lssl

OBJS = \
	Console.o  \
	Login.o    \
	Logs.o     \
	Main.o     \
	Network.o  \
	Packet.o   \
	Protocol.o \
	Window.o

TARGET = sagscl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

.cpp.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f *~ core* $(OBJS) $(TARGET)
