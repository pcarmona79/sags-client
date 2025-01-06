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
// $Revision: 1.13 $
// $Date: 2005/02/25 22:50:24 $
//

#include "Channel.hpp"
#include "Ids.hpp"
#include <wx/fontdlg.h>
#include <wx/notebook.h>
#include <wx/colour.h>
#include <wx/splitter.h>

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
	wxSplitterWindow *SeparatorWindow = new wxSplitterWindow (this, -1,
								  wxDefaultPosition,
								  wxDefaultSize,
								  wxSP_NOBORDER |
								  wxSP_LIVE_UPDATE);
	Net = NULL;
	AppConfig = AppCfg;
	index = idx;
	ParentNB = (wxNotebook *) parent;
	last_had_newline = FALSE;
	admin_mode = FALSE;

	Output = new wxTextCtrl (SeparatorWindow,
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
	if (!AppConfig->Read ("/Channel/FontName", &FontName, "Courier New"))
		AppConfig->Write ("/Channel/FontName", "Courier New");
	if (!AppConfig->Read ("/Channel/FontSize", &FontSize, 9))
		AppConfig->Write ("/Channel/FontSize", 9);
#else
	if (!AppConfig->Read ("/Channel/FontName", &FontName, "fixed"))
		AppConfig->Write ("/Channel/FontName", "fixed");
	if (!AppConfig->Read ("/Channel/FontSize", &FontSize, 13))
		AppConfig->Write ("/Channel/FontSize", 13);
#endif

	// creamos un wxFont con los valores leídos
	wxFont ChannelFont (FontSize, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
			    wxFONTWEIGHT_NORMAL, FALSE, FontName);
	TextStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	ActionStyle = new wxTextAttr (wxColour ("ORANGE"), wxNullColour, ChannelFont);
	NoticeStyle = new wxTextAttr (wxColour ("PURPLE"), wxNullColour, ChannelFont);
	JoinLeaveStyle = new wxTextAttr (wxColour ("BLUE"), wxNullColour, ChannelFont);
	TopicStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour, ChannelFont);
	NickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	MyNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, ChannelFont);
	ChannelFont.SetWeight (wxFONTWEIGHT_BOLD);
	LimiterStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour, ChannelFont);
	MyLimiterStyle = new wxTextAttr (wxColour ("GREEN"), wxNullColour, ChannelFont);
	PrivateStyle = new wxTextAttr (wxColour ("RED"), wxNullColour, ChannelFont);

	Input = new wxTextCtrl (this,
				Ids::ChatInput,
				"",
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_RICH | wxTE_PROCESS_ENTER);

	UserList = new wxListCtrl (SeparatorWindow,
				   -1,
				   wxDefaultPosition,
				   wxDefaultSize,
				   wxLC_REPORT | wxLC_NO_HEADER |
				   wxLC_SINGLE_SEL | wxLC_HRULES);

	UserList->InsertColumn (0, "", wxLIST_FORMAT_LEFT, 80);

	StatusIcons = new StatusIconList;
	UserList->SetImageList (StatusIcons->GetImageList (), wxIMAGE_LIST_SMALL);

	Topic = new wxTextCtrl (this,
				Ids::ChatTopic,
				"",
				wxDefaultPosition,
				wxDefaultSize,
				wxTE_RICH | wxTE_READONLY | wxTE_PROCESS_ENTER);

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

	SeparatorWindow->SetMinimumPaneSize (20);
	SeparatorWindow->SplitVertically (Output, UserList, 470);

	OutputSizer->Add (SeparatorWindow,
			  1,
			  wxALIGN_CENTER_VERTICAL | wxEXPAND |
			  wxLEFT | wxRIGHT | wxTOP | wxBOTTOM,
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
	Connect (Ids::ChatTopic, wxEVT_COMMAND_TEXT_ENTER,
		 (wxObjectEventFunction) &Channel::OnTopic);
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

void Channel::AddPrivMessage (wxString usr, wxString txt)
{
	SetPrivateStyle ();
	Add ("=(");
	SetNickStyle ();
	Add (usr);
	SetPrivateStyle ();
	Add (")=");
	SetTextStyle ();
	Add (" " + txt + "\n");
}

void Channel::AddPrivAction (wxString usr, wxString txt)
{
	SetPrivateStyle ();
	Add ("=(");
	SetNickStyle ();
	Add (usr);
	SetPrivateStyle ();
	Add (")=");
	SetActionStyle ();
	Add (" * " + usr + " " + txt + "\n");
	SetTextStyle ();
}

void Channel::AddPrivNotice (wxString usr, wxString txt)
{
	SetPrivateStyle ();
	Add ("=(");
	SetNickStyle ();
	Add (usr);
	SetPrivateStyle ();
	Add (")=");
	SetNoticeStyle ();
	Add (" -" + usr + "- " + txt + "\n");
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

void Channel::SetTopic (wxString newtopic)
{
	Topic->SetValue (newtopic);
	SetTopicStyle ();
	Add (wxString::Format (_(">>> The channel's topic is: %s\n"), newtopic.c_str ()));
	SetTextStyle ();
}

void Channel::ChangeTopic (wxString who, wxString newtopic)
{
	Topic->SetValue (newtopic);
	SetTopicStyle ();
	Add (wxString::Format (_(">>> %s sets topic to: %s\n"), who.c_str (),
			       newtopic.c_str ()));
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

void Channel::SetTopicStyle (void)
{
	Output->SetDefaultStyle (*TopicStyle);
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

void Channel::SetPrivateStyle (void)
{
	Output->SetDefaultStyle (*PrivateStyle);
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
	wxTextAttr *NewTopicStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour,
						    newfont);
	wxTextAttr *NewNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, newfont);
	wxTextAttr *NewMyNickStyle = new wxTextAttr (wxColour ("BLACK"), wxNullColour, newfont);
	newfont.SetWeight (wxFONTWEIGHT_BOLD);
	wxTextAttr *NewLimiterStyle = new wxTextAttr (wxColour ("MEDIUM BLUE"), wxNullColour,
						      newfont);
	wxTextAttr *NewMyLimiterStyle = new wxTextAttr (wxColour ("GREEN"), wxNullColour,
							newfont);
	wxTextAttr *NewPrivateStyle = new wxTextAttr (wxColour ("RED"), wxNullColour,
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

	LastStyle = TopicStyle;
	TopicStyle = NewTopicStyle;
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

	LastStyle = PrivateStyle;
	PrivateStyle = NewPrivateStyle;
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
	wxString to, data = Input->GetValue ();
	int to_end;

	ClearInput ();

	if (data.Length () > 0 && Net != NULL)
	{
		// buscamos comandos /me y /notice
		if (data.First ("/me ") == 0)
		{
			data = data.Mid(4);
			AddAction (Username, data);
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatAction, data.c_str ());
		}
		else if (data.First ("/notice ") == 0)
		{
			data = data.Mid(8);
			AddNotice (Username, data);
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatNotice, data.c_str ());
		}
		else if (data.First ("/topic ") == 0)
		{
			data = data.Mid(7);
			data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
			Net->AddBufferOut (index, Session::ChatTopicChange, data.c_str ());
		}
		else if (data.First ("/to ") == 0)
		{
			data = data.Mid(4);
			to_end = data.First (" ");
			to = data.Mid(0, to_end);
			data = data.Mid(to_end + 1);
			AddPrivMessage (_("To: ") + to, data);
			data = "To: " + to + "\n" +
			       "Content-Type: text/plain; charset=UTF-8\n\n" +
			       data;
			Net->AddBufferOut (index, Session::ChatPrivMessage, data.c_str ());
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

int Channel::InputGetInsertionPoint (void)
{
	return Input->GetInsertionPoint ();
}

void Channel::InputSetInsertionPoint (int p)
{
	Input->SetInsertionPoint (p);
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
	if (Net != NULL)
		Username = Net->GetUsername ();
}

void Channel::ProcessMessage (Packet *Pkt)
{
	wxString header, body, from_user, all, nick, stat;
	static wxString newuserlist, newtopic, newmessage, newprivate;
	int colon_pos;

	switch (Pkt->GetCommand ())
	{
	case Session::ChatUserList:

		newuserlist += Pkt->GetData ();

		if (Pkt->GetSequence () == 1)
		{
			SetUserList (newuserlist);
			newuserlist.Empty ();
		}

		break;

	case Session::ChatTopic:

		newtopic += Pkt->GetData ();

		if (Pkt->GetSequence () == 1)
		{
			header = ExtractHeader (newtopic);
			body = ExtractBody (newtopic);
			from_user = GetValueFromHeader (header, "From");

			if (from_user.IsEmpty ())
				SetTopic (body);
			else
				ChangeTopic (from_user, body);

			newtopic.Empty ();
		}

		break;

	case Session::ChatJoin:

		all = Pkt->GetData ();
		colon_pos = all.Find (':');

		if (colon_pos >= 0)
		{
			nick = all.Mid(0, colon_pos);
			stat = all.Mid(colon_pos + 1);
		}
		else
		{
			nick = all;
			stat = "normal";
		}

		AddUser (nick, stat);
		AddJoin (nick);

		break;

	case Session::ChatLeave:

		RemoveUser (Pkt->GetData ());
		AddLeave (Pkt->GetData ());
		break;

	case Session::ChatMessage:
	case Session::ChatAction:
	case Session::ChatNotice:

		newmessage += Pkt->GetData ();

		if (Pkt->GetSequence () == 1)
		{
			header = ExtractHeader (newmessage);
			body = ExtractBody (newmessage);
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

			newmessage.Empty ();
		}

		break;

	case Session::ChatPrivMessage:
	case Session::ChatPrivAction:
	case Session::ChatPrivNotice:

		newprivate += Pkt->GetData ();

		if (Pkt->GetSequence () == 1)
		{
			header = ExtractHeader (newprivate);
			body = ExtractBody (newprivate);
			from_user = GetValueFromHeader (header, "From");

			switch (Pkt->GetCommand ())
			{
			case Session::ChatPrivMessage:
				AddPrivMessage (from_user, body);
				break;
			case Session::ChatPrivAction:
				AddPrivAction (from_user, body);
				break;
			case Session::ChatPrivNotice:
				AddPrivNotice (from_user, body);
				break;
			}

			newprivate.Empty ();
		}

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

		header = msg.Mid(0, n + 1);
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

		body = msg.Mid(n + 2);
		return body;
	}

	return wxEmptyString;
}

wxString Channel::GetValueFromHeader (wxString hdr, wxString name)
{
	int n, i, newlinepos, colonpos = 0;

	n = hdr.First (name + ":");

	if (n < 0)
		return wxEmptyString;
	else if (n > 0 && hdr.GetChar (n - 1) != '\n')
		return wxEmptyString;

	for (i = n; hdr.GetChar (i) != '\n' && hdr.GetChar (i) != '\0'; ++i)
		if (hdr.GetChar (i) == ':')
			colonpos = i;

	newlinepos = i;

	return hdr.Mid(colonpos + 2, newlinepos - colonpos - 2);
}

void Channel::SetUserList (wxString newlist)
{
	wxString all, nick, stat;
	int i, last_pos = 0, item = -1, colon_pos;
	wxListItem newitem;

	UserList->DeleteAllItems ();

	// Desde la versión 0.4.1 del servidor, la lista es enviada
	// con el estado del usuario.

	for (i = 0; i <= (int) newlist.Length () - 1; ++i)
	{
		if (newlist.GetChar (i) == '\n')
		{
			all = newlist.Mid(last_pos, i - last_pos);
			colon_pos = all.Find (':');

			if (colon_pos >= 0)
			{
				nick = all.Mid(0, colon_pos);
				stat = all.Mid(colon_pos + 1);
			}
			else
			{
				nick = all;
				stat = "normal";
			}

			newitem.m_itemId = ++item;
			newitem.m_text = nick;
			newitem.m_image = StatusIcons->GetIconIndex (stat);
			newitem.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;

			UserList->InsertItem (newitem);
			last_pos = i + 1;
		}

		if (newlist.GetChar (i) == '\n' &&
		    newlist.GetChar (i + 1) == '\n')
			break;
	}
}

void Channel::AddUser (wxString usr, wxString status)
{
	long item = UserList->GetItemCount ();
	wxListItem newitem;

	newitem.m_itemId = item;
	newitem.m_text = usr;
	newitem.m_image = StatusIcons->GetIconIndex (status);
	newitem.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_IMAGE;

	UserList->InsertItem (newitem);
}

void Channel::RemoveUser (wxString usr)
{
	long item = UserList->FindItem (-1, usr);
	UserList->DeleteItem (item);
}

void Channel::SetAdminMode (bool mode)
{
	if (admin_mode != mode)
	{
		admin_mode = mode;
		Topic->SetEditable (admin_mode);
	}
}

void Channel::OnTopic (wxCommandEvent& WXUNUSED(event))
{
	wxString data = Topic->GetValue ();

	if (admin_mode && data.Length () > 0 && Net != NULL)
	{
		// TODO: forzar data a UTF-8!
		data = "Content-Type: text/plain; charset=UTF-8\n\n" + data;
		Net->AddBufferOut (index, Session::ChatTopicChange, data.c_str ());
		Net->Send ();
	}
}
