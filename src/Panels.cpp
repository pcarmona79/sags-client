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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Panels.cpp,v $
// $Revision: 1.1 $
// $Date: 2004/06/19 05:28:08 $
//

#include "Panels.hpp"

ListPanel::ListPanel (wxWindow *parent, wxWindowID id)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);

	TopSizer->Add (new wxStaticText (this, -1, _("Process list")),
		       0,
		       wxEXPAND |
		       wxLEFT | wxRIGHT | wxTOP,
		       7);

	ProcessList = new wxListCtrl (this, -1, wxDefaultPosition, wxDefaultSize,
				      wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL);

	ProcessList->InsertColumn (0, "", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

	ProcessList->InsertItem (0, "Server1");
	ProcessList->InsertItem (1, "Server2");
	ProcessList->InsertItem (2, "Liga1");
	ProcessList->InsertItem (3, "Liga2");
	ProcessList->InsertItem (4, "Publico1");
	ProcessList->InsertItem (5, "Publico2");

	ProcessList->SetItemState (0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	ProcessList->SetFocus ();

	TopSizer->Add (ProcessList,
		       1,
		       wxEXPAND |
		       wxLEFT | wxRIGHT | wxTOP,
		       5);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);
}

ListPanel::~ListPanel ()
{
	
}

InfoPanel::InfoPanel (wxWindow *parent, wxWindowID id)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);

	TopSizer->Add (new wxStaticText (this, -1, _("Process's information")),
		       0,
		       wxEXPAND |
		       wxLEFT | wxRIGHT | wxTOP,
		       7);

	InfoList = new wxListCtrl (this, -1, wxDefaultPosition, wxDefaultSize,
				   wxLC_REPORT);

	InfoList->InsertColumn (0, "Variable", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
	InfoList->InsertColumn (1, "Value", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

	InfoList->InsertItem (0, "Server1"); InfoList->SetItem (0, 1, "Value1");
	InfoList->InsertItem (1, "Server2"); InfoList->SetItem (1, 1, "Value2");
	InfoList->InsertItem (2, "Liga1"); InfoList->SetItem (2, 1, "Value3");
	InfoList->InsertItem (3, "Liga2"); InfoList->SetItem (3, 1, "Value4");
	InfoList->InsertItem (4, "Publico1"); InfoList->SetItem (4, 1, "Value5");
	InfoList->InsertItem (5, "Publico2"); InfoList->SetItem (5, 1, "Value6");

	TopSizer->Add (InfoList,
		       1,
		       wxEXPAND |
		       wxLEFT | wxRIGHT | wxTOP | wxBOTTOM,
		       5);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);
}

InfoPanel::~InfoPanel ()
{
	
}
