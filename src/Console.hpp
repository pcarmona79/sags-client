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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Console.hpp,v $
// $Revision: 1.5 $
// $Date: 2004/05/29 21:47:23 $
//

#ifndef __CONSOLE_HPP__
#define __CONSOLE_HPP__

#include <wx/wx.h>

class Console : public wxPanel
{
private:
	wxTextCtrl *Output;
	wxTextCtrl *Input;
	wxButton *SendButton;
	bool last_had_newline;
	wxString last_input;
	wxTextAttr *OutputStyle;
	wxTextAttr *InputStyle;

public:
	Console (wxWindow *parent, wxWindowID id, wxFont ConsoleFont);
	~Console ();

	friend class MainWindow;

	void Add (wxString text, bool memorize = FALSE);
	void SetInputStyle (void);
	void SetOutputStyle (void);
	const wxFont& GetConsoleFont (void);
	void SetConsoleFont (wxFont newfont);
	bool SaveConsoleToFile (const wxString& filename);
	void ClearOutput (void);
	void ClearInput (void);
};

#endif // __CONSOLE_HPP__
