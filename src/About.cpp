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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/About.cpp,v $
// $Revision: 1.2 $
// $Date: 2004/05/23 21:12:50 $
//

#include "About.hpp"
#include <wx/image.h>

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#  include "../pixmaps/sagscl.xpm"
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

AboutDialog::AboutDialog (wxWindow *parent, const wxString& title, const wxString& bigtext,
			  const wxString& message, const wxPoint& pos, const wxSize& size,
			  long style)
	: wxDialog (parent, -1, title, pos, size, style)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *ContentsSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *MessagesSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *ButtonSizer = new wxBoxSizer (wxHORIZONTAL);
	wxButton *OkButton = new wxButton (this, wxID_OK, _("OK"));

#ifdef __WXMSW__
	SetIcon (wxICON(A));
#else
	SetIcon (wxIcon (sagscl_xpm));
#endif

	// cargar sagscl.png y agregar a ContentsSizer
	wxInitAllImageHandlers ();
	wxBitmap AppIcon (PACKAGE_PIXMAPS_DIR "/sagscl.png", wxBITMAP_TYPE_PNG);

	ContentsSizer->Add (new wxStaticBitmap (this, -1, AppIcon),
			    0,
			    wxALIGN_TOP |
			    wxALL,
			    10);

	wxStaticText *BigLabel = new wxStaticText (this, -1, bigtext);
	wxFont LastFont = BigLabel->GetFont ();
#ifdef __WXMSW__
	wxFont BigFont (LastFont.GetPointSize () + 2, wxDEFAULT, wxNORMAL, wxBOLD, FALSE);
#else
	wxFont BigFont (LastFont.GetPointSize () + 6, wxDEFAULT, wxNORMAL, wxBOLD, FALSE);
#endif
	BigLabel->SetFont (BigFont);
	
	MessagesSizer->Add (BigLabel,
			    0,
			    wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT |
			    wxBOTTOM,
			    10);

	MessagesSizer->Add (new wxStaticText (this, -1, message),
			    0,
			    wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT |
			    wxALL,
			    0);

	ContentsSizer->Add (MessagesSizer,
			    1,
			    wxALIGN_CENTER_VERTICAL |
			    wxTOP | wxBOTTOM | wxRIGHT,
			    10);

	TopSizer->Add (ContentsSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL | wxEXPAND |
		       wxALL,
		       0);

	ButtonSizer->Add (OkButton,
			  0,
			  wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
			  wxALL,
			  0);

	TopSizer->Add (ButtonSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
		       wxALL,
		       10);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);

	OkButton->SetDefault ();
	Centre ();
}

AboutDialog::~AboutDialog ()
{

}
