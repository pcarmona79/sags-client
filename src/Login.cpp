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
// $Revision: 1.4 $
// $Date: 2004/04/21 04:45:46 $
//

#include <wx/combobox.h>

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
	wxButton *OkButton = new wxButton (this, wxID_OK, _("OK"));

	Server = new wxComboBox (this,
				 -1,
				 "",
				 wxDefaultPosition,
				 wxSize (200, 30));

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

	ButtonSizer->Add (OkButton,
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

	OkButton->SetDefault ();
	Centre ();
}

LoginDialog::~LoginDialog ()
{

}

wxString LoginDialog::GetServer (void)
{
	wxString server = "", all = Server->GetValue ();
	int colon;
	bool is_ipv6 = FALSE;

	if (all.Find ('[') != -1 && all.Find (']') != -1)
		is_ipv6 = TRUE;

	if (is_ipv6)
	{
		colon = all.Find (']', TRUE);
		server = all.Mid (1, colon - 1);
	}
	else
	{
		colon = all.Find (':', TRUE);
		if (colon == -1)
			server = all;
		else
			server = all.Mid (0, colon);
	}

	return server;
}

wxString LoginDialog::GetPort (void)
{
	wxString port = "", all = Server->GetValue ();
	int colon;
	bool is_ipv6 = FALSE;

	if (all.Find ('[') != -1 && all.Find (']') != -1)
		is_ipv6 = TRUE;

	if (is_ipv6)
	{
		colon = all.Find (':', TRUE);
		if (colon < all.Find (']'))
			return "47777";  // el puerto por defecto
		else
			port = all.Mid (colon + 1);
	}
	else
	{
		colon = all.Find (':', TRUE);
		if (colon == -1)
			return "47777";  // el puerto por defecto
		else
			port = all.Mid (colon + 1);
	}

	return port;
}

wxString LoginDialog::GetUsername (void)
{
	return Username->GetValue ();
}

wxString LoginDialog::GetPassword (void)
{
	return Password->GetValue ();
}

void LoginDialog::AddServer (wxString& newserver)
{
	Server->Append (newserver);
}

void LoginDialog::SetServerValue (const wxString& text)
{
	Server->SetValue (text);
}

wxString LoginDialog::GetServerValue (void)
{
	return Server->GetValue ();
}
