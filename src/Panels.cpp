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
// $Revision: 1.3 $
// $Date: 2004/06/22 04:58:53 $
//

#include <wx/cursor.h>
#include "Panels.hpp"
#include "Ids.hpp"

ListPanel::ListPanel (wxWindow *parent, wxWindowID id)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);

	TopSizer->Add (new wxStaticText (this, -1, _("Process list")),
		       0,
		       wxEXPAND |
		       wxLEFT | wxRIGHT | wxTOP,
		       7);

	ProcessList = new wxListCtrl (this, Ids::ProcessSelected, wxDefaultPosition,
				      wxDefaultSize,
				      wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL);

	ProcessList->InsertColumn (0, "", wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

	//ProcessList->SetItemState (0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
	//ProcessList->SetFocus ();

	ProcessList->SetCursor (wxCursor (wxCURSOR_HAND));

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

	InfoList->InsertColumn (0, _("Name"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);
	InfoList->InsertColumn (1, _("Value"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE);

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

void InfoPanel::SetInfo (wxString info)
{
	wxString name, value;
	unsigned int i, j, eq_pos = 0, start, item;

	InfoList->DeleteAllItems ();

	for (i = 0, item = 0; i <= info.Length () - 1; ++i, ++item)
	{
		start = i;
		eq_pos = 0;
		for (j = i; info.GetChar (j) != '\n'; ++j)
		{
			++i;
			if (info.GetChar (j) == '=')
				eq_pos = (eq_pos == 0) ? j : eq_pos;
			if (info.GetChar (j) == '\0')
				continue;
		}

		if (eq_pos)
		{
			name = info.Mid (start, eq_pos - start);
			value = info.Mid (eq_pos + 1, j - eq_pos - 1);
			InfoList->InsertItem (item, name);
			InfoList->SetItem (item, 1, value);
		}
	}

	InfoList->SetColumnWidth (0, wxLIST_AUTOSIZE);
	InfoList->SetColumnWidth (1, wxLIST_AUTOSIZE);
}
