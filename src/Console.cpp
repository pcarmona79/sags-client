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
// $Revision: 1.10 $
// $Date: 2004/05/29 19:59:23 $
//

#include "Console.hpp"
#include "Ids.hpp"
#include "Network.hpp"

Console::Console (wxWindow *parent, wxWindowID id, wxFont ConsoleFont)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *InputSizer = new wxBoxSizer (wxHORIZONTAL);

	Output = new wxTextCtrl (this,
				 -1,
				 "",
				 wxDefaultPosition,
				 wxDefaultSize,
				 wxTE_RICH | wxTE_MULTILINE | wxTE_READONLY | wxTE_LINEWRAP);

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
		       10);

	InputSizer->Add (Input,
			 1,
			 wxALIGN_CENTER_VERTICAL |
			 wxALIGN_CENTER_HORIZONTAL |
			 wxLEFT |
			 wxRIGHT |
			 wxBOTTOM,
			 10);

	InputSizer->Add (SendButton,
			 0,
			 wxALIGN_CENTER_VERTICAL |
			 wxALIGN_CENTER_HORIZONTAL |
			 wxRIGHT |
			 wxBOTTOM,
			 10);

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
}

Console::~Console ()
{

}

void Console::Add (wxString text, bool memorize)
{
	bool have_newline;
	wxString text_copy, rstr;
	int n = 0;
	char c, str[2];

	if (text.Last () == '\n')
		have_newline = TRUE;
	else
		have_newline = FALSE;

	// borramos todos los CR's
	text.Replace ("\r\n", "\n");

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

		rstr.Printf ("[%02x]", c);
		text.Replace (str, rstr.c_str ());
	}

	// en win98 el widget de texto no hace bien el scroll
	// por lo que lo forzaremos cambiando el foco antes
	// de poner el texto
	Output->SetFocus ();

	if (!text.IsEmpty ())
	{
		if (last_had_newline)
			Output->AppendText ("\n");
		Output->AppendText (text);
	}

	// el foco debe estar en el widget de entrada
	Input->SetFocus ();

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
