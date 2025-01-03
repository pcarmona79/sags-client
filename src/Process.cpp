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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Process.cpp,v $
// $Revision: 1.4 $
// $Date: 2005/02/03 22:03:40 $
//

#include "Process.hpp"

Process::Process (unsigned int idx)
{
	index = idx;
	ProcConsole = NULL;
	MaintenanceMode = FALSE;
}

Process::Process (Process &P)
{
	index = P.index;
	ProcConsole = NULL;
	MaintenanceMode = P.MaintenanceMode;
}

Process::~Process ()
{

}

wxString Process::GetName (void)
{
	int i, pos = InfoString.Find("Name");
	wxString name;

	if (pos != wxNOT_FOUND)
	{
		for (i = pos + 5; InfoString.GetChar (i) != '\n'; ++i)
			name += InfoString.Mid (i, 1);
		return name;
	}

	return "";
}

wxString Process::GetType (void)
{
	int i, pos = InfoString.Find("Type");
	wxString name;

	if (pos != wxNOT_FOUND)
	{
		for (i = pos + 5; InfoString.GetChar (i) != '\n'; ++i)
			name += InfoString.Mid (i, 1);
		return name;
	}

	return "";
}

bool Process::operator== (Process &P)
{
	return (this->index == P.index);
}

Process *ProcessTree::Index (unsigned int idx)
{
	Process Searched (idx);
	return TheList.Find (Searched);
};

Process *ProcessTree::operator[] (unsigned int n)
{
	return TheList[n];
}
