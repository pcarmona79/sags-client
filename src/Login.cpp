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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Login.cpp,v $
// $Revision: 1.1 $
// $Date: 2004/04/13 22:01:53 $
//

#include "Login.hpp"

LoginDialog::LoginDialog (wxWindow *parent, wxWindowID id, const wxString& title,
			  const wxPoint& pos, const wxSize& size, long style)
	: wxDialog (parent, id, title, pos, size, style)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *ServerSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *UsernameSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *PasswordSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *ButtonSizer = new wxBoxSizer (wxHORIZONTAL);

	Server = new wxComboBox (this,
				 -1,
				 "127.0.0.1",
				 wxDefaultPosition,
				 wxDefaultSize);

	Port = new wxTextCtrl (this,
			       -1,
			       "47777",
			       wxDefaultPosition,
			       wxDefaultSize);

	Username = new wxTextCtrl (this,
				   -1,
				   "",
				   wxDefaultPosition,
				   wxDefaultSize);

	Password = new wxTextCtrl (this,
				   -1,
				   "",
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxTE_PASSWORD);

	ServerSizer->Add (new wxStaticText (this, -1, _("Server:")),
			  0,
			  wxALIGN_CENTER_VERTICAL |
			  wxALIGN_RIGHT |
			  wxALL,
			  5);

	ServerSizer->Add (Server,
			  1,
			  wxALIGN_CENTER_VERTICAL |
			  wxALL,
			  5);

	ServerSizer->Add (new wxStaticText (this, -1, _("Port:")),
			  0,
			  wxALIGN_CENTER_VERTICAL |
			  wxALIGN_RIGHT |
			  wxALL,
			  5);

	ServerSizer->Add (Port,
			  0,
			  wxALIGN_CENTER_VERTICAL |
			  wxALL,
			  5);

	TopSizer->Add (ServerSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       0);

	UsernameSizer->Add (new wxStaticText (this, -1, _("Username:")),
			    0,
			    wxALIGN_CENTER_VERTICAL |
			    wxALIGN_RIGHT |
			    wxALL,
			    5);

	UsernameSizer->Add (Username,
			    1,
			    wxALIGN_CENTER_VERTICAL |
			    wxALL,
			    5);

	TopSizer->Add (UsernameSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       0);

	PasswordSizer->Add (new wxStaticText (this, -1, _("Password:")),
			    0,
			    wxALIGN_CENTER_VERTICAL |
			    wxALIGN_RIGHT |
			    wxALL,
			    5);

	PasswordSizer->Add (Password,
			    1,
			    wxALIGN_CENTER_VERTICAL |
			    wxALL,
			    5);

	TopSizer->Add (PasswordSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       0);

	ButtonSizer->Add (new wxButton (this, wxID_OK, _("OK")),
			  0,
			  wxALIGN_CENTER_VERTICAL |
			  wxALIGN_CENTER_HORIZONTAL |
			  wxALL,
			  5);

	ButtonSizer->Add (new wxButton (this, wxID_CANCEL, _("Cancel")),
			  0,
			  wxALIGN_CENTER_VERTICAL |
			  wxALIGN_CENTER_HORIZONTAL |
			  wxALL,
			  5);

	TopSizer->Add (ButtonSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL |
		       wxALIGN_CENTER_HORIZONTAL |
		       wxALL,
		       0);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);
	Centre ();
}

LoginDialog::~LoginDialog ()
{

}

wxString LoginDialog::GetServer (void)
{
	return Server->GetValue ();
}

wxString LoginDialog::GetPort (void)
{
	return Port->GetValue ();
}

wxString LoginDialog::GetUsername (void)
{
	return Username->GetValue ();
}

wxString LoginDialog::GetPassword (void)
{
	return Password->GetValue ();
}
