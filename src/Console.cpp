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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Console.cpp,v $
// $Revision: 1.21 $
// $Date: 2004/08/10 03:17:15 $
//

#include "Console.hpp"
#include "Ids.hpp"
#include <wx/fontdlg.h>
#include <wx/notebook.h>

Console::Console (wxWindow *parent, wxWindowID id, Network *N, wxConfig *AppCfg,
		  wxMenuItem *ShowLogs, unsigned int idx)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *InputSizer = new wxBoxSizer (wxHORIZONTAL);

	Net = N;
	AppConfig = AppCfg;
	index = idx;
	ParentNB = (wxNotebook *) parent;
	MenuItemShowLogs = ShowLogs;

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
	if (!AppConfig->Read ("/Console/FontName", &FontName, "Courier New"))
		AppConfig->Write ("/Console/FontName", "Courier New");
#else
	if (!AppConfig->Read ("/Console/FontName", &FontName, "fixed"))
		AppConfig->Write ("/Console/FontName", "fixed");
#endif

	if (!AppConfig->Read ("/Console/FontSize", &FontSize, 12))
		AppConfig->Write ("/Console/FontSize", 12);
	
	// creamos un wxFont con los valores leídos
	wxFont ConsoleFont (FontSize, wxDEFAULT, wxNORMAL,
			    wxNORMAL, FALSE, FontName);

	OutputStyle = new wxTextAttr (wxNullColour, wxNullColour, ConsoleFont);
	ConsoleFont.SetWeight (wxBOLD);
	InputStyle = new wxTextAttr (wxNullColour, wxNullColour, ConsoleFont);

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
		 (wxObjectEventFunction) &Console::OnSend);
	Connect (Ids::SendButton, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &Console::OnSend);
}

Console::~Console ()
{

}

void Console::Add (wxString text, bool memorize)
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
	//if (text.GetChar (0) == '\n')
	//	text.Remove (0, 1);
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
			((Console *)(ParentNB->GetPage (0)))->InputSetFocus ();
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

void Console::SetInputStyle (void)
{
	Output->SetDefaultStyle (*InputStyle);
}

void Console::SetOutputStyle (void)
{
	Output->SetDefaultStyle (*OutputStyle);
}

const wxFont& Console::GetConsoleFont (void)
{
	return (Output->GetDefaultStyle ()).GetFont ();
}

void Console::SetConsoleFont (wxFont newfont)
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

bool Console::SaveConsoleToFile (const wxString& filename)
{
	return Output->SaveFile (filename);
}

void Console::ClearOutput (void)
{
	Output->Clear ();
	last_input.Empty ();
	last_had_newline = FALSE;
}

void Console::ClearInput (void)
{
	Input->Clear ();
}

void Console::ChangeConsoleFont (void)
{
	wxFontData ActualFontData, NewFontData;

	// obtenemos la fuente actual usada en ConsoleOutput
	ActualFontData.SetInitialFont (GetConsoleFont ());
	wxFontDialog *ConsoleFontDialog = new wxFontDialog (this, ActualFontData);

	if (ConsoleFontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente a ConsoleOutput
		NewFontData = ConsoleFontDialog->GetFontData ();
		SetConsoleFont (NewFontData.GetChosenFont ());

		// los nuevos valores deben ser guardados en la configuración
		AppConfig->Write ("/Console/FontName",
				  (NewFontData.GetChosenFont ()).GetFaceName ());
		AppConfig->Write ("/Console/FontSize",
				  (NewFontData.GetChosenFont ()).GetPointSize ());
	}
}

void Console::ScrollToBottom (void)
{
	Output->ShowPosition (Output->GetLastPosition ());
}

void Console::OnSend (wxCommandEvent& WXUNUSED(event))
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

void Console::InputSetFocus (void)
{
	Input->SetFocus ();
}

void Console::OutputSetFocus (void)
{
	Output->SetFocus ();
}

unsigned int Console::GetIndex (void)
{
	return index;
}
