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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Config.cpp,v $
// $Revision: 1.4 $
// $Date: 2005/02/25 22:50:25 $
//

#include "Config.hpp"
#include "Ids.hpp"
#include <wx/fontdlg.h>

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#  include "../pixmaps/sagscl.xpm"
#endif

ConfigDialog::ConfigDialog (wxConfig *AppCfg, ProcessTree *ProcLst, Channel *GenCh,
			    wxWindow *parent, wxWindowID id, const wxString& title,
			    const wxPoint& pos, const wxSize& size, long style)
	: wxDialog (parent, id, title, pos, size, style)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxFlexGridSizer *DataSizer = new wxFlexGridSizer (2);
	wxBoxSizer *ButtonSizer = new wxBoxSizer (wxHORIZONTAL);
	wxButton *CloseButton = new wxButton (this, wxID_OK, _("Close"));

	// objetos de la ventana padre usados por este diálogo
	AppConfig = AppCfg;
	ProcList = ProcLst;
	GeneralChannel = GenCh;

#ifdef __WXMSW__
	SetIcon (wxICON(A));
#else
	SetIcon (wxIcon (sagscl_xpm));
#endif

	wxString ConsoleFontName, ChannelFontName;
	long ConsoleFontSize, ChannelFontSize;

#ifdef __WXMSW__
	if (!AppConfig->Read ("/Console/FontName", &ConsoleFontName, "Courier New"))
		AppConfig->Write ("/Console/FontName", "Courier New");
	if (!AppConfig->Read ("/Channel/FontName", &ChannelFontName, "Courier New"))
		AppConfig->Write ("/Channel/FontName", "Courier New");

	if (!AppConfig->Read ("/Console/FontSize", &ConsoleFontSize, 9))
		AppConfig->Write ("/Console/FontSize", 9);
	if (!AppConfig->Read ("/Channel/FontSize", &ChannelFontSize, 9))
		AppConfig->Write ("/Channel/FontSize", 9);
#else
	if (!AppConfig->Read ("/Console/FontName", &ConsoleFontName, "fixed"))
		AppConfig->Write ("/Console/FontName", "fixed");
	if (!AppConfig->Read ("/Channel/FontName", &ChannelFontName, "fixed"))
		AppConfig->Write ("/Channel/FontName", "fixed");

	if (!AppConfig->Read ("/Console/FontSize", &ConsoleFontSize, 13))
		AppConfig->Write ("/Console/FontSize", 13);
	if (!AppConfig->Read ("/Channel/FontSize", &ChannelFontSize, 13))
		AppConfig->Write ("/Channel/FontSize", 13);
#endif

	// creamos un wxFont con los valores leídos
	wxFont ConsoleFont (ConsoleFontSize, wxDEFAULT, wxNORMAL,
			    wxNORMAL, FALSE, ConsoleFontName);
	ConsoleFontData.SetInitialFont (ConsoleFont);

	wxFont ChannelFont (ChannelFontSize, wxDEFAULT, wxNORMAL,
			    wxNORMAL, FALSE, ChannelFontName);
	ChannelFontData.SetInitialFont (ChannelFont);

	ButtonConsoleFont = new wxButton (this, Ids::ConfigConsoleFont, ConsoleFontName +
					  wxString::Format (" %ld", ConsoleFontSize));
	ButtonChannelFont = new wxButton (this, Ids::ConfigChannelFont, ChannelFontName +
					  wxString::Format (" %ld", ChannelFontSize));

	DataSizer->Add (new wxStaticText (this, -1, _("Console's font")),
			1,
			wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT |
			wxALL,
			5);

	DataSizer->Add (ButtonConsoleFont,
			1,
			wxALIGN_CENTER_VERTICAL | wxEXPAND |
			wxALL,
			5);

	DataSizer->Add (new wxStaticText (this, -1, _("Chat channel's font")),
			1,
			wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT |
			wxALL,
			5);

	DataSizer->Add (ButtonChannelFont,
			1,
			wxALIGN_CENTER_VERTICAL | wxEXPAND |
			wxALL,
			5);

	TopSizer->Add (DataSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL | wxEXPAND |
		       wxALL,
		       5);

	ButtonSizer->Add (CloseButton,
			  0,
			  wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
			  wxALL,
			  5);

	TopSizer->Add (ButtonSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
		       wxLEFT | wxBOTTOM | wxRIGHT,
		       5);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);

	CloseButton->SetDefault ();
	Centre ();

	Connect (Ids::ConfigConsoleFont, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &ConfigDialog::OnButtonConsoleFont);
	Connect (Ids::ConfigChannelFont, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &ConfigDialog::OnButtonChannelFont);
}

ConfigDialog::~ConfigDialog ()
{
	
}

wxString ConfigDialog::GetConsoleFontName (void)
{
	return (ConsoleFontData.GetChosenFont ()).GetFaceName ();
}

int ConfigDialog::GetConsoleFontSize (void)
{
	return (ConsoleFontData.GetChosenFont ()).GetPointSize ();
}

wxString ConfigDialog::GetChannelFontName (void)
{
	return (ChannelFontData.GetChosenFont ()).GetFaceName ();
}

int ConfigDialog::GetChannelFontSize (void)
{
	return (ChannelFontData.GetChosenFont ()).GetPointSize ();
}

void ConfigDialog::OnButtonConsoleFont (wxCommandEvent& WXUNUSED(event))
{
	int i;
	wxFontData NewFontData;
	wxFontDialog *FontDialog = new wxFontDialog (this, ConsoleFontData);

	if (FontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente
		NewFontData = FontDialog->GetFontData ();

		// los nuevos valores deben ser guardados en la configuración
		AppConfig->Write ("/Console/FontName",
				  (NewFontData.GetChosenFont ()).GetFaceName ());
		AppConfig->Write ("/Console/FontSize",
				 (NewFontData.GetChosenFont ()).GetPointSize ());

		for (i = 0; i <= (int) ProcList->TheList.GetCount () - 1; ++i)
			(*ProcList)[i]->ProcConsole->SetConsoleFont (NewFontData.GetChosenFont ());

		// actualizamos la etiqueta del botón
		ButtonConsoleFont->SetLabel ((NewFontData.GetChosenFont ()).GetFaceName () +
					     wxString::Format (" %d",
					     (NewFontData.GetChosenFont ()).GetPointSize ()));

		// guardamos los valores para usarlos después
		ConsoleFontData = NewFontData;
	}

	FontDialog->Destroy ();
}

void ConfigDialog::OnButtonChannelFont (wxCommandEvent& WXUNUSED(event))
{
	wxFontData NewFontData;
	wxFontDialog *FontDialog = new wxFontDialog (this, ChannelFontData);

	if (FontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente
		NewFontData = FontDialog->GetFontData ();

		// los nuevos valores deben ser guardados en la configuración
		AppConfig->Write ("/Channel/FontName",
				  (NewFontData.GetChosenFont ()).GetFaceName ());
		AppConfig->Write ("/Channel/FontSize",
				 (NewFontData.GetChosenFont ()).GetPointSize ());

		GeneralChannel->SetChannelFont (NewFontData.GetChosenFont ());

		// actualizamos la etiqueta del botón
		ButtonChannelFont->SetLabel ((NewFontData.GetChosenFont ()).GetFaceName () +
					     wxString::Format (" %d",
					     (NewFontData.GetChosenFont ()).GetPointSize ()));

		// guardamos los valores para usarlos después
		ChannelFontData = NewFontData;
	}

	FontDialog->Destroy ();
}
