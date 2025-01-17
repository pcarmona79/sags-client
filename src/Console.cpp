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
// $Revision: 1.27 $
// $Date: 2005/02/25 22:50:25 $
//

#include "Console.hpp"
#include "Ids.hpp"
#include "Channel.hpp"
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
				 wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP);

	// extraemos de la configuración el nombre de la
	// fuente a usar en la consola
	wxString FontName;
	long FontSize;

#ifdef __WXMSW__
	if (!AppConfig->Read ("/Console/FontName", &FontName, "Courier New"))
		AppConfig->Write ("/Console/FontName", "Courier New");
	if (!AppConfig->Read ("/Console/FontSize", &FontSize, 9))
		AppConfig->Write ("/Console/FontSize", 9);
#else
	if (!AppConfig->Read ("/Console/FontName", &FontName, "fixed"))
		AppConfig->Write ("/Console/FontName", "fixed");
	if (!AppConfig->Read ("/Console/FontSize", &FontSize, 13))
		AppConfig->Write ("/Console/FontSize", 13);
#endif

	// creamos un wxFont con los valores leídos
	wxFont ConsoleFont (FontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
			    wxFONTWEIGHT_NORMAL, FALSE, FontName);

	OutputStyle = new wxTextAttr (wxNullColour, wxNullColour, ConsoleFont);
	ConsoleFont.SetWeight (wxFONTWEIGHT_BOLD);
	InputStyle = new wxTextAttr (wxNullColour, wxNullColour, ConsoleFont);

	Output->SetDefaultStyle (*OutputStyle);
	Output->Refresh ();

	Input = new wxComboBox (this,
				Ids::Input,
				"",
				wxDefaultPosition,
				wxDefaultSize);

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
	// de poner el texto (memorizando la posicion del cursor
	// de la consola que esta seleccionada)
	wxNotebookPage *CurrentNBP = NULL;
	int pos;

	if (ParentNB->GetPageCount () > 0)
	{
		CurrentNBP = ParentNB->GetPage (ParentNB->GetSelection ());

		if (CurrentNBP->GetId () != Ids::WindowChat &&
		    CurrentNBP->GetId () != Ids::WindowLogs)
			pos = ((Console *)CurrentNBP)->InputGetInsertionPoint ();
		else if (CurrentNBP->GetId () == Ids::WindowChat)
			pos = ((Channel *)CurrentNBP)->InputGetInsertionPoint ();
		else
			pos = InputGetInsertionPoint ();
	}
	else
		pos = InputGetInsertionPoint ();

	OutputSetFocus ();
#endif

	if (!text.IsEmpty ())
	{
		if (last_had_newline)
			Output->AppendText ("\n");
		Output->AppendText (text);
	}

#ifdef __WXMSW__
	// el foco debe estar en el widget de entrada, pero
	// de la consola que esta seleccionada
	if (ParentNB->GetPageCount () > 0)
	{
		if (CurrentNBP->GetId () != Ids::WindowChat &&
		    CurrentNBP->GetId () != Ids::WindowLogs)
		{
			((Console *)CurrentNBP)->InputSetFocus ();
			((Console *)CurrentNBP)->InputSetInsertionPoint (pos);
		}
		else if (CurrentNBP->GetId () == Ids::WindowChat)
		{
			((Channel *)CurrentNBP)->InputSetFocus ();
			((Channel *)CurrentNBP)->InputSetInsertionPoint (pos);
		}
		else
		{
			InputSetFocus ();
			InputSetInsertionPoint (pos);
		}
	}
	else
	{
		InputSetFocus ();
		InputSetInsertionPoint (pos);
	}
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

void Console::SetConsoleFont (wxFont newfont)
{
	wxTextAttr *LastStyle = NULL;
	wxTextAttr *NewOutputStyle = new wxTextAttr (wxNullColour, wxNullColour, newfont);
	newfont.SetWeight (wxFONTWEIGHT_BOLD);
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
	Input->SetValue ("");
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
		// agregamos data al combobox
		AddInputCommand (data);

		data += "\n";
		SetInputStyle ();
		Add (data, TRUE);
		SetOutputStyle ();

		Net->AddBufferOut (index, Session::ConsoleInput, data.c_str ());
		Net->Send (); // esto bloquea la GUI?
	}
}

int Console::InputGetInsertionPoint (void)
{
	return Input->GetInsertionPoint ();
}

void Console::InputSetInsertionPoint (int p)
{
	Input->SetInsertionPoint (p);
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

void Console::AddInputCommand (wxString com)
{
	if (Input->FindString (com) < 0)
		Input->Append (com);
}

void Console::SetNetwork (Network *N)
{
	Net = N;
}
