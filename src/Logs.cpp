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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Logs.cpp,v $
// $Revision: 1.3 $
// $Date: 2004/06/19 23:59:23 $
//

#include "Logs.hpp"
#include "Ids.hpp"
#include <wx/datetime.h>

Logs::Logs (wxWindow *parent, wxWindowID id)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);

	Output = new wxTextCtrl (this,
				 -1,
				 "",
				 wxDefaultPosition,
				 wxDefaultSize,
				 wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_LINEWRAP);

	TopSizer->Add (Output,
		       1,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       5);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);
}

Logs::~Logs ()
{

}

void Logs::Append (const wxString& text)
{
	wxDateTime Now = wxDateTime::Now ();
	wxString Time = "[" + Now.FormatISODate () + " " + Now.FormatISOTime () + "] ";

	if (Output->GetLastPosition () > 0)
		Output->AppendText ("\n");
		
	Output->AppendText (Time);
	Output->AppendText (text);
}
