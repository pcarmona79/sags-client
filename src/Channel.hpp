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
// $Revision: 1.6 $
// $Date: 2004/08/17 02:29:48 $
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

	wxTextAttr *TextStyle;
	wxTextAttr *ActionStyle;
	wxTextAttr *NoticeStyle;
	wxTextAttr *JoinLeaveStyle;

	wxTextAttr *NickStyle;
	wxTextAttr *MyNickStyle;
	wxTextAttr *LimiterStyle;
	wxTextAttr *MyLimiterStyle;

	Network *Net;
	wxConfig *AppConfig;
	unsigned int index;
	wxNotebook *ParentNB;

	wxString Username;
	bool last_had_newline;

public:
	Channel (wxWindow *parent, wxWindowID id, wxConfig *AppCfg,
		 unsigned int idx = 0);
	~Channel ();

	void Add (wxString text);
	void AddMessage (wxString usr, wxString txt);
	void AddAction (wxString usr, wxString txt);
	void AddNotice (wxString usr, wxString txt);
	void AddJoin (wxString usr);
	void AddLeave (wxString usr);

	void SetTextStyle (void);
	void SetActionStyle (void);
	void SetNoticeStyle (void);
	void SetJoinLeaveStyle (void);
	void SetNickStyle (void);
	void SetMyNickStyle (void);
	void SetLimiterStyle (void);
	void SetMyLimiterStyle (void);

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

	void ProcessMessage (Packet *Pkt);
	wxString ExtractHeader (wxString msg);
	wxString ExtractBody (wxString msg);
	wxString GetValueFromHeader (wxString hdr, wxString name);

	void OnSend (wxCommandEvent& event);
};

#endif // __CHANNEL_HPP__
