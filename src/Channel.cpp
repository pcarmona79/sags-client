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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Channel.cpp,v $
// $Revision: 1.2 $
// $Date: 2004/08/10 03:17:15 $
//

#include "Channel.hpp"
#include "Ids.hpp"
#include <wx/fontdlg.h>
#include <wx/notebook.h>

Channel::Channel (wxWindow *parent, wxWindowID id, Network *N, wxConfig *AppCfg,
		  unsigned int idx)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *InputSizer = new wxBoxSizer (wxHORIZONTAL);

	Net = N;
	AppConfig = AppCfg;
	index = idx;
	ParentNB = (wxNotebook *) parent;

	Output = new wxTextCtrl (this,
				 -1,
				 "",
				 wxDefaultPosition,
				 wxDefaultSize,
				 wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY | wxTE_LINEWRAP);

	// extraemos de la configuración el nombre de la
	// fuente a usar en la consola
	wxString FontName;
	long FontSize;

#ifdef __WXMSW__
	if (!AppConfig->Read ("/Channel/FontName", &FontName, "Courier New"))
		AppConfig->Write ("/Channel/FontName", "Courier New");
#else
	if (!AppConfig->Read ("/Channel/FontName", &FontName, "fixed"))
		AppConfig->Write ("/Channel/FontName", "fixed");
#endif

	if (!AppConfig->Read ("/Channel/FontSize", &FontSize, 12))
		AppConfig->Write ("/Channel/FontSize", 12);
	
	// creamos un wxFont con los valores leídos
	wxFont ChannelFont (FontSize, wxDEFAULT, wxNORMAL,
			    wxNORMAL, FALSE, FontName);

	OutputStyle = new wxTextAttr (wxNullColour, wxNullColour, ChannelFont);
	ChannelFont.SetWeight (wxBOLD);
	InputStyle = new wxTextAttr (wxNullColour, wxNullColour, ChannelFont);

	Output->SetDefaultStyle (*OutputStyle);
	Output->Refresh ();

	Input = new wxTextCtrl (this,
				Ids::Input,
				"",
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_RICH | wxTE_PROCESS_ENTER);

	SendButton = new wxButton (this, Ids::SendButton, _("Send"));

	TopSizer->Add (Output,
		       1,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       5);

	InputSizer->Add (Input,
			 1,
			 wxALIGN_CENTER_VERTICAL |
			 wxALIGN_CENTER_HORIZONTAL |
			 wxLEFT |
			 wxRIGHT |
			 wxBOTTOM,
			 5);

	InputSizer->Add (SendButton,
			 0,
			 wxALIGN_CENTER_VERTICAL |
			 wxALIGN_CENTER_HORIZONTAL |
			 wxRIGHT |
			 wxBOTTOM,
			 5);

	TopSizer->Add (InputSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL |
		       wxEXPAND |
		       wxALL,
		       0);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);

	last_input.Empty ();
	last_had_newline = FALSE;

	// el foco debe estar en el widget de entrada
	Input->SetFocus ();

	// señales de la consola
	Connect (Ids::Input, wxEVT_COMMAND_TEXT_ENTER,
		 (wxObjectEventFunction) &Channel::OnSend);
	Connect (Ids::SendButton, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &Channel::OnSend);
}

Channel::~Channel ()
{

}

void Channel::Add (wxString text, bool memorize)
{
	bool have_newline;
	wxString text_copy;
	int n = 0;
	unsigned char c;
	char str[2];

	if (text.Last () == '\n')
		have_newline = TRUE;
	else
		have_newline = FALSE;

	// borramos todos los CR's
	text.Replace ("\r", "");

	// borramos el texto repetido
	text_copy = text;
	if (!memorize && !last_input.IsEmpty ())
		n = text.Replace (last_input, "", FALSE);

	// borramos el primer y el último LF
	if (text.GetChar (0) == '\n')
		text.Remove (0, 1);
	if (text.Last () == '\n')
		text.RemoveLast ();

	// borrar caracteres especiales emitidos por servers de Q2
	for (c = 1; c < 32; ++c)
	{
		str[0] = c;
		str[1] = '\0';

		if (str[0] == '\n')
			continue;

		text.Replace (str, "*");
	}
	for (c = 128; c < 160; ++c)
	{
		str[0] = c;
		str[1] = '\0';
		text.Replace (str, "*");
	}

#ifdef __WXMSW__
	// en win98 el widget de texto no hace bien el scroll
	// por lo que lo forzaremos cambiando el foco antes
	// de poner el texto
	Output->SetFocus ();
#endif

	if (!text.IsEmpty ())
	{
		if (last_had_newline)
			Output->AppendText ("\n");
		Output->AppendText (text);
	}

#ifdef __WXMSW__
	// el foco debe estar en el widget de entrada, pero
	// de la consola que esta seccionada
	if (ParentNB->GetPageCount () > 0)
	{
		if (!MenuItemShowLogs->IsChecked () || ParentNB->GetPageCount () != 1)
			((Channel *)(ParentNB->GetPage (0)))->InputSetFocus ();
		else
			Input->SetFocus ();
	}
	else
			Input->SetFocus ();
#endif

	if (memorize)
		last_input = text_copy;
	else
		if (n > 0)
			last_input.Empty ();

	last_had_newline = have_newline;
}

void Channel::SetInputStyle (void)
{
	Output->SetDefaultStyle (*InputStyle);
}

void Channel::SetOutputStyle (void)
{
	Output->SetDefaultStyle (*OutputStyle);
}

const wxFont& Channel::GetChannelFont (void)
{
	return (Output->GetDefaultStyle ()).GetFont ();
}

void Channel::SetChannelFont (wxFont newfont)
{
	wxTextAttr *LastStyle = NULL;
	wxTextAttr *NewOutputStyle = new wxTextAttr (wxNullColour, wxNullColour, newfont);
	newfont.SetWeight (wxBOLD);
	wxTextAttr *NewInputStyle = new wxTextAttr (wxNullColour, wxNullColour, newfont);

	LastStyle = OutputStyle;
	OutputStyle = NewOutputStyle;
	delete LastStyle;
	LastStyle = InputStyle;
	InputStyle = NewInputStyle;
	delete LastStyle;

	Output->SetDefaultStyle (*OutputStyle);
	Output->SetStyle (0, Output->GetLastPosition (), *OutputStyle);
}

bool Channel::SaveChannelToFile (const wxString& filename)
{
	return Output->SaveFile (filename);
}

void Channel::ClearOutput (void)
{
	Output->Clear ();
	last_input.Empty ();
	last_had_newline = FALSE;
}

void Channel::ClearInput (void)
{
	Input->Clear ();
}

void Channel::ChangeChannelFont (void)
{
	wxFontData ActualFontData, NewFontData;

	// obtenemos la fuente actual usada en ConsoleOutput
	ActualFontData.SetInitialFont (GetChannelFont ());
	wxFontDialog *ChannelFontDialog = new wxFontDialog (this, ActualFontData);

	if (ChannelFontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente a ConsoleOutput
		NewFontData = ChannelFontDialog->GetFontData ();
		SetChannelFont (NewFontData.GetChosenFont ());

		// los nuevos valores deben ser guardados en la configuración
		AppConfig->Write ("/Channel/FontName",
				  (NewFontData.GetChosenFont ()).GetFaceName ());
		AppConfig->Write ("/Channel/FontSize",
				  (NewFontData.GetChosenFont ()).GetPointSize ());
	}
}

void Channel::ScrollToBottom (void)
{
	Output->ShowPosition (Output->GetLastPosition ());
}

void Channel::OnSend (wxCommandEvent& WXUNUSED(event))
{
	wxString data = Input->GetValue ();

	ClearInput ();

	if (data.Length () > 0 && Net != NULL)
	{
		data += "\n";
		SetInputStyle ();
		Add (data, TRUE);
		SetOutputStyle ();

		Net->AddBufferOut (index, Session::ConsoleInput, data.c_str ());
		Net->Send (); // esto bloquea la GUI?
	}
}

void Channel::InputSetFocus (void)
{
	Input->SetFocus ();
}

void Channel::OutputSetFocus (void)
{
	Output->SetFocus ();
}

unsigned int Channel::GetIndex (void)
{
	return index;
}
