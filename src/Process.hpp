//
// SAGS - Secure Administrator of Game Servers
// Copyright (C) 2004 Pablo Carmona Amigo
// 
// This file is part of SAGS Client.
//
// SAGS Client is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// SAGS Client is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Process.hpp,v $
// $Revision: 1.4 $
// $Date: 2005/02/03 22:03:40 $
//

#ifndef __PROCESS_HPP__
#define __PROCESS_HPP__

#include <wx/wx.h>
#include "Console.hpp"

class Process
{
public:
	Process (unsigned int idx = 0);
	Process (Process &P);
	~Process ();

	Console *ProcConsole;
	unsigned int index;
	wxString InfoString;
	bool MaintenanceMode;

	wxString GetName (void);
	wxString GetType (void);

	bool operator== (Process &P);
};

class ProcessTree
{
public:
	List<Process> TheList;

	Process *Index (unsigned int idx);
	Process *operator[] (unsigned int n);
};

#endif // __PROCESS_HPP__
