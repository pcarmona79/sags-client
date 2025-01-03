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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Window.hpp,v $
// $Revision: 1.17 $
// $Date: 2005/02/15 22:16:05 $
//

#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <wx/wx.h>
#include <wx/config.h>
#include <wx/menuitem.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include "Panels.hpp"
#include "Logs.hpp"
#include "Ids.hpp"
#include "Network.hpp"
#include "Process.hpp"
#include "Channel.hpp"

class MainWindow : public wxFrame
{
private:
	wxConfig *AppConfig;
	Network *Net;

	Logs *LoggingTab;
	wxNotebook *MainNotebook;

	wxSplitterWindow *PanelsWindow;
	wxSplitterWindow *NotebookWindow;

	ListPanel *ProcListPanel;
	InfoPanel *ProcInfoPanel;

	ProcessTree ProcList;
	Process *ProcessShown;

	wxMenuItem *MenuItemConnect;
	wxMenuItem *MenuItemDisconnect;
	wxMenuItem *MenuItemShowLogs;
	wxMenuItem *MenuItemProcessKill;
	wxMenuItem *MenuItemProcessLaunch;
	wxMenuItem *MenuItemProcessRestart;
	wxMenuItem *MenuItemProcessMaintenance;
	wxMenuItem *MenuItemProcessSave;

	Channel *GeneralChannel;

	wxString HelpLanguage;

public:
	MainWindow (const wxString& title,
		    const wxPoint& position,
		    const wxSize& size,
		    const wxString& help_lang);
	~MainWindow ();
	void Disconnect (void);

	void ProtoSync (Packet *Pkt);
	void ProtoAuth (Packet *Pkt);
	void ProtoSession (Packet *Pkt);
	void ProtoError (Packet *Pkt);

	// manejadores de señales
	void OnConnect (wxCommandEvent& event);
	void OnDisconnect (wxCommandEvent& event);
	void OnClose (wxCommandEvent& event);
	void OnQuit (wxCommandEvent& event);
	void OnHelp (wxCommandEvent& event);
	void OnAbout (wxCommandEvent& event);
	void OnConsoleSave (wxCommandEvent& event);
	void OnProcessSelected (wxListEvent& event);
	void OnShowLogs (wxCommandEvent& event);
	void OnPreferences (wxCommandEvent& event);
	void OnProcessKill (wxCommandEvent& event);
	void OnProcessLaunch (wxCommandEvent& event);
	void OnProcessRestart (wxCommandEvent& event);
	void OnProcessMaintenance (wxCommandEvent& event);
	void OnSaveChat (wxCommandEvent& event);
	void OnSaveLogs (wxCommandEvent& event);

	// señales de sockets
	void OnSocketConnected (wxCommandEvent& event);
	void OnSocketRead (wxCommandEvent& event);
	void OnSocketFailConnect (wxCommandEvent& event);
	void OnSocketFailRead (wxCommandEvent& event);
};

#endif // __WINDOW_HPP__
