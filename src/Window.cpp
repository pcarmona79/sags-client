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
// $Revision: 1.3 $
// $Date: 2004/04/17 22:00:14 $
//

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/socket.h>
#include <wx/fontdlg.h>

#include "Window.hpp"
#include "Login.hpp"
#include "Packet.hpp"

MainWindow::MainWindow (const wxString& title,
			const wxPoint& position,
			const wxSize& size)
	: wxFrame ((wxWindow*) NULL, -1, title, position, size)
{
	wxMenuBar *MenuBar = new wxMenuBar;
	wxMenu *MenuSession = new wxMenu;
	wxMenu *MenuEdit = new wxMenu;
	wxMenu *MenuHelp = new wxMenu;

	// al comienzo no tenemos red
	Net = NULL;

#ifdef _WIN32
	SetIcon (wxICON(A));
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

	// conectamos las señales de la conexion
	Connect (NetEvt::Connect, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnConnected);
	Connect (NetEvt::Read, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnRead);
	Connect (NetEvt::FailConnect, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnFailConnect);
	Connect (NetEvt::FailRead, wxEVT_SOCKET,
		 (wxObjectEventFunction) &MainWindow::OnFailRead);

	// creamos una barra de estado
	CreateStatusBar (2);

	wxNotebook *MainNotebook = new wxNotebook (this, -1);
	ServerConsole = new Console (MainNotebook, -1);
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
	if (Net != NULL)
	{
		Net->Wait ();
		Net->Delete ();
		Net = NULL;
	}
}

void MainWindow::OnConnect (wxCommandEvent& WXUNUSED(event))
{
	if (Net != NULL)
		if (Net->IsConnected ())
			return;

	wxString text, server, port;
	LoginDialog *Login = new LoginDialog (this, -1, "Connect to...",
					      wxDefaultPosition, wxSize (200, 100));

	if (Login->ShowModal () == wxID_OK)
	{

		text = "Connecting to server [" + Login->GetServer ();
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
	wxMessageBox (_("Client for SAGS Server\n"
			"(C) 2004 Pablo Carmona Amigo"),
		      _("About Secure Administrator of Game Servers"),
		      wxOK | wxICON_INFORMATION, this);
}

void MainWindow::OnConnected (wxCommandEvent& WXUNUSED(event))
{
	wxString text = _("Authenticating...");

	SetStatusText (text, 1);
	LoggingTab->Append (text);
	ServerConsole->Output->Clear ();
}

void MainWindow::OnRead (wxCommandEvent& WXUNUSED(event))
{
	wxString text;
	bool ex;
	Packet *Ans = NULL, *Pkt = Net->Get ();
	static int bytes = 0;

	if (Pkt != NULL)
	{
		switch (Pkt->GetType ())
		{
			case Pckt::SyncHello:
				text.Printf ("SyncHello (%d bytes): %s",
					     Pkt->GetLength (), Pkt->GetData ());
				LoggingTab->Append (text);
				break;

			case Pckt::SyncVersion:
				text.Printf ("SyncVersion (%d bytes): %s",
					     Pkt->GetLength (), Pkt->GetData ());
				LoggingTab->Append (text);
				break;

			case Pckt::AuthPassword:
				text.Printf ("AuthPassword (%d bytes): %s",
					     Pkt->GetLength (), Pkt->GetData ());
				LoggingTab->Append (text);
				break;

			case Pckt::AuthSuccessful:
				text.Printf ("AuthSuccessful (%d bytes): %s",
					     Pkt->GetLength (), Pkt->GetData ());
				LoggingTab->Append (text);
				LoggingTab->Append ("Logged successfully.");
				text = _("Getting process's log...");
				SetStatusText (text, 1);
				LoggingTab->Append (text);
				break;

			case Pckt::SessionConsoleOutput:
				// esto llena mucho el widget
				//text.Printf ("SessionConsoleOutput (%d bytes)",
				//	     Pkt->GetLength ());
				//LoggingTab->Append (text);
				ServerConsole->Add (Pkt->GetData ());
				break;

			case Pckt::SessionConsoleSuccess:
				text.Printf ("SessionConsoleSuccess");
				LoggingTab->Append (text);
				break;

			case Pckt::SessionConsoleLogs:
				text.Printf ("SessionConsoleLogs (%d bytes)",
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
					text += _(" Server: [") + Net->GetAddress () + "]:" + Net->GetPort ();
					SetStatusText (text, 1);
					bytes = 0;
				}
				break;

			case Pckt::SessionDisconnect:
			case Pckt::SessionDrop:
				text.Printf ("SessionDisconnect");
				LoggingTab->Append (text);
				ex = Net->IsExiting ();
				Net->Wait ();
				Net->Delete ();
				Net = NULL;
				SetStatusText (_("Disconnected"), 1);
				LoggingTab->Append (_("Disconnected."));
				if (ex == FALSE)
				{
					wxMessageBox (_("Disconnected from server."),
						      _("Error"),
						      wxOK | wxICON_EXCLAMATION, this);
				}
				return;

			case Pckt::ErrorGeneric:
				text.Printf ("ErrorGeneric (%d bytes): %s",
					     Pkt->GetLength (), Pkt->GetData ());
				LoggingTab->Append (text);
				wxMessageBox (Pkt->GetData (),
					      _("Error"),
					      wxOK | wxICON_ERROR, this);
				break;

			default:
				;
		}
	}
	else
		LoggingTab->Append ("Pkt NULL");

	if (Ans != NULL)
	{
		Net->AddOut (Ans);
		Net->Send (); // esto bloquea la GUI?
	}
}

void MainWindow::OnFailConnect (wxCommandEvent& WXUNUSED(event))
{
	SetStatusText (_("Disconnected"), 1);
	LoggingTab->Append (_("Failed to connect to server."));
	wxMessageBox (_("Failed to connect to server."),
		      _("Error"),
		      wxOK | wxICON_ERROR, this);
}

void MainWindow::OnFailRead (wxCommandEvent& WXUNUSED(event))
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
	Packet *Pkt = NULL;

	ServerConsole->Input->Clear ();

	if (data.Length () > 0 && Net != NULL)
	{
		data += "\n";
		ServerConsole->SetInputStyle ();
		ServerConsole->Add (data, TRUE);
		ServerConsole->SetOutputStyle ();

		Pkt = new Packet (Pckt::SessionConsoleInput, data.c_str ());
		Net->AddOut (Pkt);
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
	}
}

void MainWindow::OnConsoleSave (wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *SaveDialog = new wxFileDialog (this, _("Save to file"), "", "", "*.*",
						     wxSAVE | wxOVERWRITE_PROMPT);

	if (SaveDialog->ShowModal () == wxID_OK)
		ServerConsole->SaveConsoleToFile (SaveDialog->GetPath ());
}
