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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Window.cpp,v $
// $Revision: 1.13 $
// $Date: 2004/06/17 08:26:37 $
//

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/socket.h>
#include <wx/fontdlg.h>
#include <wx/filedlg.h>

#include "Window.hpp"
#include "Login.hpp"
#include "Packet.hpp"
#include "About.hpp"

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__)
#  include "../pixmaps/sagscl.xpm"
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

MainWindow::MainWindow (const wxString& title,
			const wxPoint& position,
			const wxSize& size)
	: wxFrame ((wxWindow*) NULL, -1, title, position, size)
{
	wxMenuBar *MenuBar = new wxMenuBar;
	wxMenu *MenuSession = new wxMenu;
	wxMenu *MenuEdit = new wxMenu;
	wxMenu *MenuHelp = new wxMenu;

	AppConfig = new wxConfig (PACKAGE);

	// al comienzo no tenemos red
	Net = NULL;

#ifdef __WXMSW__
	SetIcon (wxICON(A));
#else
	SetIcon (wxIcon (sagscl_xpm));
#endif

	// menú Session
	MenuSession->Append (Ids::Connect, _("&Connect..."), _("Connect to server"));
	MenuSession->Append (Ids::Disconnect, _("&Disconnect"), _("Disconnect from server"));
	MenuSession->AppendSeparator ();
	MenuSession->Append (Ids::ConsoleSave, _("&Save to file..."), _("Save console's messages to a file"));
	MenuSession->AppendSeparator ();
	MenuSession->Append (Ids::Quit, _("E&xit"), _("Exit the application"));

	// menú Edit
	MenuEdit->Append (Ids::ConsoleFont, _("&Font..."), _("Change console's font"));

	// menú Help
	MenuHelp->Append (Ids::Help, _("&Contents"), _("Show help contents"));
	MenuHelp->AppendSeparator ();
	MenuHelp->Append (Ids::About, _("&About..."), _("About this application"));

	// agregamos a la barra de menús
	MenuBar->Append (MenuSession, _("&Session"));
	MenuBar->Append (MenuEdit, _("&Edit"));
	MenuBar->Append (MenuHelp, _("&Help"));
	SetMenuBar (MenuBar);

	// señal close
	Connect (wxID_ANY, wxEVT_CLOSE_WINDOW,
		 (wxObjectEventFunction) &MainWindow::OnClose);

	// conectamos las señales de los menús
	Connect (Ids::Connect, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConnect);
	Connect (Ids::Disconnect, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnDisconnect);
	Connect (Ids::ConsoleSave, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConsoleSave);
	Connect (Ids::Quit, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnQuit);
	Connect (Ids::ConsoleFont, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConsoleFont);
	Connect (Ids::Help, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnHelp);
	Connect (Ids::About, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnAbout);

	// conectamos las señales de la conexión
	Connect (NetEvt::Connect, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnSocketConnected);
	Connect (NetEvt::Read, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnSocketRead);
	Connect (NetEvt::FailConnect, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnSocketFailConnect);
	Connect (NetEvt::FailRead, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnSocketFailRead);

	// creamos una barra de estado
	CreateStatusBar (2);

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

	wxNotebook *MainNotebook = new wxNotebook (this, -1);
	ServerConsole = new Console (MainNotebook, -1, ConsoleFont);
	LoggingTab = new Logs (MainNotebook, -1);

	MainNotebook->AddPage ((wxNotebookPage *) ServerConsole,
				_("Console"));
	MainNotebook->AddPage ((wxNotebookPage *) LoggingTab,
				_("Logs"));

	// señales de la consola
	Connect (Ids::Input, wxEVT_COMMAND_TEXT_ENTER,
		 (wxObjectEventFunction) &MainWindow::OnSend);
	Connect (Ids::SendButton, wxEVT_COMMAND_BUTTON_CLICKED,
		 (wxObjectEventFunction) &MainWindow::OnSend);

}

MainWindow::~MainWindow ()
{
	delete AppConfig;

	if (Net != NULL)
	{
		Net->Wait ();
		Net->Delete ();
		Net = NULL;
	}
}

void MainWindow::Disconnect (void)
{
	Net->Wait ();
	Net->Delete ();
	Net = NULL;
	
	SetStatusText (_("Disconnected"), 1);
	LoggingTab->Append (_("Disconnected."));

	wxString windowtitle;
	windowtitle.Printf (_("SAGS Client %s"), VERSION);
	SetTitle (windowtitle);
}

void MainWindow::OnConnect (wxCommandEvent& WXUNUSED(event))
{
	int i;
	wxString text, server, port, Key, Value, AntKey, AntValue, InputServer;
	char keystr[] = "/Login/Server";
	int repeated = 5;

	if (Net != NULL)
		if (Net->IsConnected ())
			return;

	LoginDialog *Login = new LoginDialog (this, -1, _("Connect to..."),
					      wxDefaultPosition, wxSize (200, 100));

	// leemos los servidores de la configuración
	// TODO: el límite de 5 servidores podría ser configurable
	for (i = 1; i <= 5; ++i)
	{
		Key.Printf ("%s%d", keystr, i);
		if (!AppConfig->Read (Key, &Value, ""))
			AppConfig->Write (Key, "");
		if (!Value.IsEmpty ())
			Login->AddServer (Value);
	}

	//Login->SetServerValue ("");

	if (Login->ShowModal () == wxID_OK)
	{
		text = _("Connecting to server [") + Login->GetServer ();
		text += "]:" + Login->GetPort () + "...";
		SetStatusText (text, 1);
		LoggingTab->Append (text);

		Net = new Network ((wxEvtHandler *) this,
				   Login->GetServer (),
				   Login->GetPort (),
				   Login->GetUsername (),
				   Login->GetPassword ());
		Net->Create ();
		Net->Run ();

		// buscamos el valor repetido y si existe lo borramos
		InputServer = Login->GetServerValue ();
		for (i = 1; i <= 5; ++i)
		{
			Key.Printf ("%s%d", keystr, i);
			AppConfig->Read (Key, &Value, "");
			if (Value == InputServer)
			{
				repeated = i;
				AppConfig->Write (Key, "");
			}
		}

		// guardamos los nuevos valores, dejando al recién ingresado
		// como el primero de la lista
		for (i = repeated; i >= 1; --i)
		{
			Key.Printf ("%s%d", keystr, i);
			AppConfig->Read (Key, &Value);
			if (i > 1)
			{
				AntKey.Printf ("%s%d", keystr, i - 1);
				AppConfig->Read (AntKey, &AntValue);
			}
			else
			{
				AntValue = InputServer;
			}
			if (!AntValue.IsEmpty ())
				AppConfig->Write (Key, AntValue);
		}
	}
}

void MainWindow::OnDisconnect (wxCommandEvent& WXUNUSED(event))
{
	if (Net != NULL)
	{
		if (Net->IsConnected ())
		{
			Net->Disconnect (TRUE);
			SetStatusText (_("Disconnecting..."), 1);
			LoggingTab->Append (_("Disconnecting..."));
			Disconnect ();
		}
		else
			SetStatusText (_("Not connected"), 1);
	}
	else
	{
		SetStatusText (_("Not connected"), 1);
	}
}

void MainWindow::OnClose (wxCommandEvent& WXUNUSED(event))
{
	if (Net != NULL)
		if (Net->IsConnected ())
			Net->Disconnect (TRUE);

	Destroy ();
}

void MainWindow::OnQuit (wxCommandEvent& WXUNUSED(event))
{
	Close (TRUE);
}

void MainWindow::OnHelp (wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox (_("No help available yet."),
		      _("Information"),
		      wxOK | wxICON_EXCLAMATION, this);
}

void MainWindow::OnAbout (wxCommandEvent& WXUNUSED(event))
{
	wxString progname, about = _("Secure Administrator of Game Servers\n"
				     "http://sags.sourceforge.net/\n\n"
				     "(C) 2004 Pablo Carmona Amigo\n"
				     "Licensed under the GNU GPL");

	progname.Printf (_("SAGS Client %s"), VERSION);

	AboutDialog *About = new AboutDialog (this, _("About"), progname, about);
	About->ShowModal ();
	About->Destroy ();
}

void MainWindow::OnSocketConnected (wxCommandEvent& WXUNUSED(event))
{
	wxString text = _("Connected to server. Authenticating...");

	SetStatusText (text, 1);
	LoggingTab->Append (text);
	ServerConsole->ClearOutput ();
}

void MainWindow::OnSocketRead (wxCommandEvent& WXUNUSED(event))
{
	Packet *Pkt = Net->Get ();

	if (Pkt == NULL)
	{
		LoggingTab->Append ("Nothing in Incoming list");
		return;
	}

	switch (Pkt->GetIndex ())
	{
	case Sync::Index:
		ProtoSync (Pkt);
		break;

	case Auth::Index:
		ProtoAuth (Pkt);
		break;

	case Error::Index:
		ProtoError (Pkt);
		break;

	default:
		// paquetes de sesión de un proceso
		if (Pkt->GetIndex () <= Session::MaxIndex)
			ProtoSession (Pkt);
	}

	delete Pkt;	
}

void MainWindow::ProtoSync (Packet *Pkt)
{
	wxString text;

	switch (Pkt->GetCommand ())
	{
	case Sync::Hello:
		text.Printf ("Sync::Hello (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		break;

	case Sync::Version:
		text.Printf ("Sync::Version (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		break;

	default:
		;
	}
}

void MainWindow::ProtoAuth (Packet *Pkt)
{
	wxString text, windowtitle;

	switch (Pkt->GetCommand ())
	{
	case Auth::Password:
		text.Printf ("Auth::Password (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		break;

	case Auth::Successful:
		text.Printf ("Auth::Successful (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		LoggingTab->Append (_("Logged in successfully."));
		text = _("Getting process's log...");
		SetStatusText (text, 1);
		LoggingTab->Append (text);
		windowtitle.Printf (_("SAGS Client %s"), VERSION);
		windowtitle += " - " + Net->GetUsername () + "@[";
		windowtitle += Net->GetAddress () + "]:" + Net->GetPort ();
		SetTitle (windowtitle);
		break;

	default:
		;
	}
}

void MainWindow::ProtoSession (Packet *Pkt)
{
	wxString text;
	static int bytes = 0;

	switch (Pkt->GetCommand ())
	{
	case Session::ConsoleOutput:
		// esto llena mucho el widget
		//text.Printf ("Session::ConsoleOutput (%d bytes)",
		//	     Pkt->GetLength ());
		//LoggingTab->Append (text);
		ServerConsole->Add (Pkt->GetData ());
		break;

	case Session::ConsoleSuccess:
		text.Printf ("Session::ConsoleSuccess");
		LoggingTab->Append (text);
		break;

	case Session::ConsoleLogs:
		text.Printf ("Session::ConsoleLogs (%d bytes)",
			     Pkt->GetLength ());
		LoggingTab->Append (text);
		ServerConsole->Add (Pkt->GetData ());

		bytes += Pkt->GetLength ();
		text.Printf (_("Receiving logs: %.1f KB"),
			     (float)(bytes) / 1024.0);
		SetStatusText (text, 1);

		// si la secuencia es 1 entonces es el último paquete!
		if (Pkt->GetSequence () == 1)
		{
			text.Printf (_("Received %.1f KB of logs"),
				     (float)(bytes) / 1024.0);
			LoggingTab->Append (text);
			text = _("User: ") + Net->GetUsername ();
			text += _(" Server: [") + Net->GetAddress () + "]:"
				+ Net->GetPort ();
			SetStatusText (text, 1);
			bytes = 0;
		}
		break;

	case Session::Disconnect:
		text.Printf ("Session::Disconnect");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Disconnected from server."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	default:
		;
	}
}

void MainWindow::ProtoError (Packet *Pkt)
{
	wxString text;

	switch (Pkt->GetCommand ())
	{
	case Error::ServerFull:
		text.Printf ("Error::ServerFull");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Server is full.\nDisconnected."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		return;

	case Error::NotValidVersion:
		text.Printf ("Error::NotValidVersion");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Protocol's version not valid.\nDisconnected."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		return;

	case Error::LoginFailed:
		text.Printf ("Error::LoginFailed");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Username or password not valid.\nDisconnected."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		return;

	case Error::AuthTimeout:
		text.Printf ("Error::AuthTimeout");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Time for authentication has expired.\nDisconnected."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		return;

	case Error::ServerQuit:
		text.Printf ("Error::ServerQuit");
		LoggingTab->Append (text);
		Disconnect ();
		wxMessageBox (_("Server is shutting down.\nDisconnected."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		return;

	case Error::BadProcess:
		text.Printf ("Error::BadProcess (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		wxMessageBox ((wxString)(_("Bad process on server:\n\n")) +
			      (wxString)(Pkt->GetData ()),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	case Error::CantWriteToProcess:
		text.Printf ("Error::CantWriteToProcess");
		LoggingTab->Append (text);
		wxMessageBox (_("Server can't write to the process."),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	case Error::Generic:
		text.Printf ("Error::Generic (%d bytes): %s",
			     Pkt->GetLength (), Pkt->GetData ());
		LoggingTab->Append (text);
		wxMessageBox ((wxString)(_("Generic Error:\n\n")) +
			      (wxString)(Pkt->GetData ()),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	default:
		;
	}
}

void MainWindow::OnSocketFailConnect (wxCommandEvent& WXUNUSED(event))
{
	SetStatusText (_("Disconnected"), 1);
	LoggingTab->Append (_("Failed to connect to server."));
	wxMessageBox (_("Failed to connect to server."),
		      _("Error"),
		      wxOK | wxICON_ERROR, this);
}

void MainWindow::OnSocketFailRead (wxCommandEvent& WXUNUSED(event))
{
	SetStatusText (_("Disconnected"), 1);
	LoggingTab->Append (_("Failed to read data from server."));
	LoggingTab->Append (_("Disconnected."));
	wxMessageBox (_("Failed to read data from server.\n"
			"Disconnected."),
		      _("Error"),
		      wxOK | wxICON_ERROR, this);
}

void MainWindow::OnSend (wxCommandEvent& WXUNUSED(event))
{
	wxString data = ServerConsole->Input->GetValue ();

	ServerConsole->ClearInput ();

	if (data.Length () > 0 && Net != NULL)
	{
		data += "\n";
		ServerConsole->SetInputStyle ();
		ServerConsole->Add (data, TRUE);
		ServerConsole->SetOutputStyle ();

		// para mientras pedimos el del proceso 1
		Net->AddBufferOut (1, Session::ConsoleInput, data.c_str ());
		Net->Send (); // esto bloquea la GUI?
	}
}

void MainWindow::OnConsoleFont (wxCommandEvent& WXUNUSED(event))
{
	wxFontData ActualFontData, NewFontData;

	// obtenemos la fuente actual usada en ConsoleOutput
	ActualFontData.SetInitialFont (ServerConsole->GetConsoleFont ());
	wxFontDialog *ConsoleFontDialog = new wxFontDialog (this, ActualFontData);

	if (ConsoleFontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente a ConsoleOutput
		NewFontData = ConsoleFontDialog->GetFontData ();
		ServerConsole->SetConsoleFont (NewFontData.GetChosenFont ());

		// los nuevos valores deben ser guardados en la configuración
		AppConfig->Write ("/Console/FontName",
				  (NewFontData.GetChosenFont ()).GetFaceName ());
		AppConfig->Write ("/Console/FontSize",
				  (NewFontData.GetChosenFont ()).GetPointSize ());
	}
}

void MainWindow::OnConsoleSave (wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *SaveDialog = new wxFileDialog (this, _("Save to file"), "", "", "*.*",
						     wxSAVE | wxOVERWRITE_PROMPT);

	if (SaveDialog->ShowModal () == wxID_OK)
		ServerConsole->SaveConsoleToFile (SaveDialog->GetPath ());
}
