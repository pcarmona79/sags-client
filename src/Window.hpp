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
// $Revision: 1.4 $
// $Date: 2004/04/21 04:45:46 $
//

#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include <wx/wx.h>
#include <wx/config.h>

#include "Console.hpp"
#include "Logs.hpp"
#include "Ids.hpp"
#include "Network.hpp"

class MainWindow : public wxFrame
{
private:
	Console *ServerConsole;
	Logs *LoggingTab;
	Network *Net;
	wxConfig *AppConfig;

public:
	MainWindow (const wxString& title,
		    const wxPoint& position,
		    const wxSize& size);
	~MainWindow ();

	// manejadores de señales
	void OnConnect (wxCommandEvent& event);
	void OnDisconnect (wxCommandEvent& event);
	void OnClose (wxCommandEvent& event);
	void OnQuit (wxCommandEvent& event);
	void OnHelp (wxCommandEvent& event);
	void OnAbout (wxCommandEvent& event);
	void OnSend (wxCommandEvent& event);
	void OnConsoleFont (wxCommandEvent& event);
	void OnConsoleSave (wxCommandEvent& event);

	// señales de sockets
	void OnConnected (wxCommandEvent& event);
	void OnRead (wxCommandEvent& event);
	void OnFailConnect (wxCommandEvent& event);
	void OnFailRead (wxCommandEvent& event);
};

#endif // __WINDOW_HPP__
