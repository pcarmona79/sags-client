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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Channel.hpp,v $
// $Revision: 1.4 $
// $Date: 2004/08/13 00:55:37 $
//

#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include <wx/wx.h>
#include <wx/config.h>
#include <wx/listctrl.h>
#include "Network.hpp"

class Channel : public wxPanel
{
private:
	wxTextCtrl *Output;
	wxTextCtrl *Input;
	wxTextCtrl *Topic;
	wxListCtrl *UserList;

	wxButton *SendButton;
	bool last_had_newline;
	wxString last_input;
	wxTextAttr *OutputStyle;
	wxTextAttr *InputStyle;
	Network *Net;
	wxConfig *AppConfig;
	unsigned int index;
	wxNotebook *ParentNB;

	wxString Username;

public:
	Channel (wxWindow *parent, wxWindowID id, wxConfig *AppCfg,
		 unsigned int idx = 0);
	~Channel ();

	void Add (wxString text, bool memorize = FALSE);
	void SetInputStyle (void);
	void SetOutputStyle (void);
	const wxFont& GetChannelFont (void);
	void SetChannelFont (wxFont newfont);
	bool SaveChannelToFile (const wxString& filename);
	void ClearOutput (void);
	void ClearInput (void);
	void ScrollToBottom (void);
	void InputSetFocus (void);
	void OutputSetFocus (void);
	unsigned int GetIndex (void);
	void SetNetwork (Network *N);

	void OnSend (wxCommandEvent& event);
};

#endif // __CHANNEL_HPP__
