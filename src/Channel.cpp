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
// $Revision: 1.7 $
// $Date: 2004/08/17 02:29:48 $
//

#include "Channel.hpp"
#include "Ids.hpp"
#include <wx/fontdlg.h>
#include <wx/notebook.h>
#include <wx/colour.h>

#ifdef __WXMSW__
#  include "Console.hpp"
#endif

Channel::Channel (wxWindow *parent, wxWindowID id, wxConfig *AppCfg,
		  unsigned int idx)
	: wxPanel (parent, id)
{
	wxBoxSizer *TopSizer = new wxBoxSizer (wxVERTICAL);
	wxBoxSizer *TopicSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *OutputSizer = new wxBoxSizer (wxHORIZONTAL);
	wxBoxSizer *InputSizer = new wxBoxSizer (wxHORIZONTAL);

	Net = NULL;
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
	TextStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	ActionStyle = new wxTextAttr (wxColour ("ORANGE"), wxNullColour, ChannelFont);
	NoticeStyle = new wxTextAttr (wxColour ("PURPLE"), wxNullColour, ChannelFont);
	JoinLeaveStyle = new wxTextAttr (wxColour ("BLUE"), wxNullColour, ChannelFont);
	NickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	MyNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	ChannelFont.SetWeight (wxBOLD);
	LimiterStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour, ChannelFont);
	MyLimiterStyle = new wxTextAttr (wxColour ("GREEN"), wxNullColour, ChannelFont);

	Input = new wxTextCtrl (this,
				Ids::ChatInput,
				"",
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_RICH | wxTE_PROCESS_ENTER);

	UserList = new wxListCtrl (this,
				   -1,
				   wxDefaultPosition,
				   wxSize (100, 1),
				   wxLC_REPORT | wxLC_NO_HEADER |
				   wxLC_SINGLE_SEL | wxLC_HRULES);

	UserList->InsertColumn (0, "", wxLIST_FORMAT_LEFT, 100);

	Topic = new wxTextCtrl (this,
				-1,
				"Here comes the channel's topic",
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_RICH | wxTE_READONLY);

	SendButton = new wxButton (this, Ids::ChatSend, _("Send"));

	TopicSizer->Add (new wxStaticText (this, -1, _("Channel's topic:")),
			 0,
			 wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP,
			 5);

	TopicSizer->Add (Topic,
			 1,
			 wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP,
			 5);

	TopSizer->Add (TopicSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL,
		       0);

	OutputSizer->Add (Output,
			  1,
			  wxALIGN_CENTER_VERTICAL | wxEXPAND |
			  wxLEFT | wxRIGHT | wxTOP | wxBOTTOM,
			  5);

	OutputSizer->Add (UserList,
			  0,
			  wxALIGN_CENTER_VERTICAL | wxEXPAND |
			  wxRIGHT | wxTOP | wxBOTTOM,
			  5);

	TopSizer->Add (OutputSizer,
		       1,
		       wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL,
		       0);

	InputSizer->Add (Input,
			 1,
			 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
			 wxLEFT | wxRIGHT | wxBOTTOM,
			 5);

	InputSizer->Add (SendButton,
			 0,
			 wxALIGN_CENTER_VERTICAL | wxALIGN_CENTER_HORIZONTAL |
			 wxRIGHT | wxBOTTOM,
			 5);

	TopSizer->Add (InputSizer,
		       0,
		       wxALIGN_CENTER_VERTICAL | wxEXPAND | wxALL,
		       0);

	SetAutoLayout (TRUE);
	SetSizer (TopSizer);
	TopSizer->Fit (this);
	TopSizer->SetSizeHints (this);

	// el foco debe estar en el widget de entrada
	Input->SetFocus ();

	// señales
	Connect (Ids::ChatInput, wxEVT_COMMAND_TEXT_ENTER,
		 (wxObjectEventFunction) &Channel::OnSend);
	Connect (Ids::ChatSend, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &Channel::OnSend);
}

Channel::~Channel ()
{

}

void Channel::Add (wxString text)
{
	bool have_newline;

	if (text.Last () == '\n')
		have_newline = TRUE;
	else
		have_newline = FALSE;

	// borramos todos los CR's
	text.Replace ("\r", "");

	// borramos el primer y el último LF
	if (text.GetChar (0) == '\n')
		text.Remove (0, 1);
	if (text.Last () == '\n')
		text.RemoveLast ();

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
	wxNotebookPage *CurrentNBP;

	if (ParentNB->GetPageCount () > 0)
	{
		CurrentNBP = ParentNB->GetPage (ParentNB->GetSelection ());

		if (CurrentNBP->GetId () != Ids::WindowChat &&
		    CurrentNBP->GetId () != Ids::WindowLogs)
			((Console *)CurrentNBP)->InputSetFocus ();
		else
			Input->SetFocus ();
	}
	else
		Input->SetFocus ();
#endif

	last_had_newline = have_newline;
}

void Channel::AddMessage (wxString usr, wxString txt)
{
	if (usr == Username)
	{
		SetMyLimiterStyle ();
		Add ("<");
		SetMyNickStyle ();
		Add (usr);
		SetMyLimiterStyle ();
	}
	else
	{
		SetLimiterStyle ();
		Add ("<");
		SetNickStyle ();
		Add (usr);
		SetLimiterStyle ();
	}

	Add (">");
	SetTextStyle ();
	Add (" " + txt + "\n");
}

void Channel::AddAction (wxString usr, wxString txt)
{
	SetActionStyle ();
	Add ("* " + usr + " " + txt + "\n");
	SetTextStyle ();
}

void Channel::AddNotice (wxString usr, wxString txt)
{
	SetNoticeStyle ();
	Add ("-" + usr + "- " + txt + "\n");
	SetTextStyle ();
}

void Channel::AddJoin (wxString usr)
{
	SetJoinLeaveStyle ();
	Add (wxString::Format (_("---> %s has joined the channel\n"), usr.c_str ()));
	SetTextStyle ();
}

void Channel::AddLeave (wxString usr)
{
	SetJoinLeaveStyle ();
	Add (wxString::Format (_("<--- %s leaves the channel\n"), usr.c_str ()));
	SetTextStyle ();
}

void Channel::SetTextStyle (void)
{
	Output->SetDefaultStyle (*TextStyle);
}

void Channel::SetActionStyle (void)
{
	Output->SetDefaultStyle (*ActionStyle);
}

void Channel::SetNoticeStyle (void)
{
	Output->SetDefaultStyle (*NoticeStyle);
}

void Channel::SetJoinLeaveStyle (void)
{
	Output->SetDefaultStyle (*JoinLeaveStyle);
}

void Channel::SetNickStyle (void)
{
	Output->SetDefaultStyle (*NickStyle);
}

void Channel::SetMyNickStyle (void)
{
	Output->SetDefaultStyle (*MyNickStyle);
}

void Channel::SetLimiterStyle (void)
{
	Output->SetDefaultStyle (*LimiterStyle);
}

void Channel::SetMyLimiterStyle (void)
{
	Output->SetDefaultStyle (*MyLimiterStyle);
}

const wxFont& Channel::GetChannelFont (void)
{
	return (Output->GetDefaultStyle ()).GetFont ();
}

void Channel::SetChannelFont (wxFont newfont)
{
	wxTextAttr *LastStyle = NULL;
	wxTextAttr *NewTextStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, newfont);
	wxTextAttr *NewActionStyle = new wxTextAttr (wxColour ("ORANGE"), wxNullColour,
						     newfont);
	wxTextAttr *NewNoticeStyle = new wxTextAttr (wxColour ("PURPLE"), wxNullColour,
						     newfont);
	wxTextAttr *NewJoinLeaveStyle = new wxTextAttr (wxColour ("BLUE"), wxNullColour,
							newfont);
	wxTextAttr *NewNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, newfont);
	wxTextAttr *NewMyNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, newfont);
	newfont.SetWeight (wxBOLD);
	wxTextAttr *NewLimiterStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour,
						      newfont);
	wxTextAttr *NewMyLimiterStyle = new wxTextAttr (wxColour ("GREEN"), wxNullColour,
							newfont);

	LastStyle = TextStyle;
	TextStyle = NewTextStyle;
	delete LastStyle;

	LastStyle = ActionStyle;
	ActionStyle = NewActionStyle;
	delete LastStyle;

	LastStyle = NoticeStyle;
	NoticeStyle = NewNoticeStyle;
	delete LastStyle;

	LastStyle = JoinLeaveStyle;
	JoinLeaveStyle = NewJoinLeaveStyle;
	delete LastStyle;

	LastStyle = NickStyle;
	NickStyle = NewNickStyle;
	delete LastStyle;

	LastStyle = MyNickStyle;
	MyNickStyle = NewMyNickStyle;
	delete LastStyle;

	LastStyle = LimiterStyle;
	LimiterStyle = NewLimiterStyle;
	delete LastStyle;

	LastStyle = MyLimiterStyle;
	MyLimiterStyle = NewMyLimiterStyle;
	delete LastStyle;

	Output->SetDefaultStyle (*TextStyle);
	//Output->SetStyle (0, Output->GetLastPosition (), *TextStyle);
}

bool Channel::SaveChannelToFile (const wxString& filename)
{
	return Output->SaveFile (filename);
}

void Channel::ClearOutput (void)
{
	Output->Clear ();
}

void Channel::ClearInput (void)
{
	Input->Clear ();
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
		// buscamos comandos /me y /notice
		if (data.First ("/me ") == 0)
		{
			data = data.Mid (4, wxSTRING_MAXLEN);
			AddAction (Username, data);
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatAction, data.c_str ());
		}
		else if (data.First ("/notice ") == 0)
		{
			data = data.Mid (8, wxSTRING_MAXLEN);
			AddNotice (Username, data);
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatNotice, data.c_str ());
		}
		else
		{
			AddMessage (Username, data);
			// TODO: forzar data a UTF-8!
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatMessage, data.c_str ());
		}

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

void Channel::SetNetwork (Network *N)
{
	Net = N;
	Username = Net->GetUsername ();
}

void Channel::ProcessMessage (Packet *Pkt)
{
	wxString header, body, from_user;

	switch (Pkt->GetCommand ())
	{
	case Session::ChatUserList:

		break;

	case Session::ChatTopic:

		break;

	case Session::ChatJoin:

		AddJoin (Pkt->GetData ());
		break;

	case Session::ChatLeave:

		AddLeave (Pkt->GetData ());
		break;

	case Session::ChatMessage:
	case Session::ChatAction:
	case Session::ChatNotice:

		header = ExtractHeader (Pkt->GetData ());
		body = ExtractBody (Pkt->GetData ());
		from_user = GetValueFromHeader (header, "From");

		if (from_user == Username)
			break;

		switch (Pkt->GetCommand ())
		{
		case Session::ChatMessage:
			AddMessage (from_user, body);
			break;
		case Session::ChatAction:
			AddAction (from_user, body);
			break;
		case Session::ChatNotice:
			AddNotice (from_user, body);
			break;
		}

		break;

	case Session::ChatPrivMessage:
	case Session::ChatPrivAction:
	case Session::ChatPrivNotice:

		// no están aun implementados
		break;

	default:
		break;
	}
}

wxString Channel::ExtractHeader (wxString msg)
{
	wxString header;
	int n;

	if (!msg.IsEmpty ())
	{
		for (n = 0; msg.GetChar (n) != '\0'; ++n)
			if (msg.GetChar (n) == '\n' &&
			    msg.GetChar (n + 1) == '\n')
				break;

		//if (msg.GetChar (n) != '\0')
		//	++n;

		header = msg.Mid (0, n + 1);
		return header;
	}

	return wxEmptyString;
}

wxString Channel::ExtractBody (wxString msg)
{
	wxString body;
	int n;

	if (!msg.IsEmpty ())
	{
		for (n = 0; msg.GetChar (n) != '\0'; ++n)
			if (msg.GetChar (n) == '\n' &&
			    msg.GetChar (n + 1) == '\n')
				break;

		body = msg.Mid (n + 2, wxSTRING_MAXLEN);
		return body;
	}

	return wxEmptyString;
}

wxString Channel::GetValueFromHeader (wxString hdr, wxString name)
{
	int n, i, newlinepos, colonpos = 0;

	n = hdr.First (name + ":");

	if (n > 0 && hdr.GetChar (n - 1) != '\n')
		return wxEmptyString;

	for (i = n; hdr.GetChar (i) != '\n' && hdr.GetChar (i) != '\0'; ++i)
		if (hdr.GetChar (i) == ':')
			colonpos = i;

	newlinepos = i;

	return hdr.Mid (colonpos + 2, newlinepos - colonpos - 2);
}
