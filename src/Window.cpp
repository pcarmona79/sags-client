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
// $Revision: 1.29 $
// $Date: 2004/08/10 03:17:15 $
//

#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/socket.h>
#include <wx/filedlg.h>
#include <wx/menuitem.h>
#include <wx/fontdlg.h>
#include "Window.hpp"
#include "Login.hpp"
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
	wxMenu *MenuProcess = new wxMenu;
	wxMenu *MenuView = new wxMenu;
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
	MenuItemConnect = new wxMenuItem (MenuSession, Ids::Connect, _("&Connect..."),
					  _("Connect to server"), wxITEM_NORMAL);
	MenuItemDisconnect = new wxMenuItem (MenuSession, Ids::Disconnect, _("&Disconnect"),
					     _("Disconnect from server"), wxITEM_NORMAL);
	MenuSession->Append (MenuItemConnect);
	MenuSession->Append (MenuItemDisconnect);
	MenuSession->AppendSeparator ();
	MenuSession->Append (Ids::Quit, _("E&xit"), _("Exit the application"));

	// menú Process
	MenuProcess->Append (Ids::ConsoleFont, _("Change &font..."),
			     _("Change the console's font"));
	MenuProcess->Append (Ids::ConsoleSave, _("&Save to file..."),
			     _("Save console's messages to a file"));
	MenuProcess->AppendSeparator ();
	MenuItemProcessKill = new wxMenuItem (MenuProcess, Ids::ProcessKill,
					      _("&Kill"),
					      _("Terminates the actual process"),
					      wxITEM_NORMAL);
	MenuProcess->Append (MenuItemProcessKill);
	MenuItemProcessLaunch = new wxMenuItem (MenuProcess, Ids::ProcessLaunch,
						_("&Launch"),
						_("Launch the actual process"),
						wxITEM_NORMAL);
	MenuProcess->Append (MenuItemProcessLaunch);
	MenuItemProcessRestart = new wxMenuItem (MenuProcess, Ids::ProcessRestart,
						 _("Force to &restart"),
						 _("Force to restart the actual process"),
						 wxITEM_NORMAL);
	MenuProcess->Append (MenuItemProcessRestart);

	// menú View
	MenuItemShowLogs = new wxMenuItem (MenuView, Ids::ShowLogs, _("Show &logs"),
					   _("Show the logging window"), wxITEM_CHECK);
	MenuView->Append (MenuItemShowLogs);

	// menú Help
	MenuHelp->Append (Ids::Help, _("&Contents"), _("Show help contents"));
	MenuHelp->AppendSeparator ();
	MenuHelp->Append (Ids::About, _("&About..."), _("About this application"));

	// agregamos a la barra de menús
	MenuBar->Append (MenuSession, _("&Session"));
	MenuBar->Append (MenuProcess, _("&Process"));
	MenuBar->Append (MenuView, _("&View"));
	MenuBar->Append (MenuHelp, _("&Help"));
	SetMenuBar (MenuBar);

	// deshabilitamos algunos menús
	MenuItemDisconnect->Enable (FALSE);
	MenuItemProcessKill->Enable (FALSE);
	MenuItemProcessLaunch->Enable (FALSE);
	MenuItemProcessRestart->Enable (FALSE);

	// mostramos los logs?
	long ShowLogs;
	if (!AppConfig->Read ("/Options/ShowLogs", &ShowLogs, 0l))
		AppConfig->Write ("/Options/ShowLogs", 0l);
	if (ShowLogs)
		MenuItemShowLogs->Check (TRUE);
	else
		MenuItemShowLogs->Check (FALSE);

	// señal close
	Connect (wxID_ANY, wxEVT_CLOSE_WINDOW,
		 (wxObjectEventFunction) &MainWindow::OnClose);

	// conectamos las señales de los menús
	Connect (Ids::Connect, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConnect);
	Connect (Ids::Disconnect, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnDisconnect);
	Connect (Ids::Quit, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnQuit);

	Connect (Ids::ConsoleSave, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConsoleSave);
	Connect (Ids::ConsoleFont, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnConsoleFont);
	Connect (Ids::ProcessKill, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnProcessKill);
	Connect (Ids::ProcessLaunch, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnProcessLaunch);
	Connect (Ids::ProcessRestart, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnProcessRestart);

	Connect (Ids::ShowLogs, wxEVT_COMMAND_MENU_SELECTED,
		 (wxObjectEventFunction) &MainWindow::OnShowLogs);

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

	NotebookWindow = new wxSplitterWindow (this, -1,
					       wxDefaultPosition, wxDefaultSize,
					       wxSP_NOBORDER | wxSP_LIVE_UPDATE);
	NotebookWindow->SetMinimumPaneSize (3);

	PanelsWindow = new wxSplitterWindow (NotebookWindow, -1,
					     wxDefaultPosition, wxDefaultSize,
					     wxSP_NOBORDER | wxSP_LIVE_UPDATE);
	PanelsWindow->SetMinimumPaneSize (3);

	ProcListPanel = new ListPanel (PanelsWindow, -1);
	ProcInfoPanel = new InfoPanel (PanelsWindow, -1);

	PanelsWindow->SplitHorizontally (ProcListPanel, ProcInfoPanel, 150);

	MainNotebook = new wxNotebook (NotebookWindow, -1);
	LoggingTab = new Logs (MainNotebook, -1);
	GeneralChannel = new Channel (MainNotebook, -1, Net, AppConfig);

#ifdef __WXMSW__
	if (ShowLogs)
		MainNotebook->AddPage ((wxNotebookPage *) LoggingTab, _("Logs"));
	else
		LoggingTab->Hide ();
#else
	if (!ShowLogs)
		LoggingTab->Hide ();

	MainNotebook->AddPage ((wxNotebookPage *) LoggingTab, _("Logs"));
#endif

	MainNotebook->InsertPage (MainNotebook->GetPageCount () - 1,
				  (wxNotebookPage *) GeneralChannel, _("Chat"), TRUE);

	NotebookWindow->SplitVertically (PanelsWindow, MainNotebook, 150);

	// conectamos la señal de la lista de procesos
	Connect (Ids::ProcessSelected, wxEVT_COMMAND_LIST_ITEM_SELECTED,
	         (wxObjectEventFunction) &MainWindow::OnProcessSelected);

	// TODO: las dimensiones y la posición se podrían
	//       obtener de las opciones con AppConfig
	SetSize (750, 500);
	Centre ();
}

MainWindow::~MainWindow ()
{
	delete AppConfig;

	if (Net != NULL)
	{
		if (Net->IsRunning ())
			Net->Wait ();
		Net->Delete ();	
	}
}

void MainWindow::Disconnect (void)
{
	Net->Wait ();
	Net->Delete ();
	Net = NULL;
	
	SetStatusText (_("Disconnected"), 1);
	LoggingTab->Append (_("Disconnected."));

	SetTitle (wxString::Format (_("SAGS Client %s"), VERSION));

	MenuItemConnect->Enable (TRUE);
	MenuItemDisconnect->Enable (FALSE);
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
#ifdef __WXMSW__
		if (i == 1)
			Login->SetServerValue (Value);
#endif
	}

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
		
		MenuItemConnect->Enable (FALSE);
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
	// borrar las consolas del notebook
#ifdef __WXMSW__
	if (MainNotebook->GetPageCount () > 0)
	{
		if (!MenuItemShowLogs->IsChecked () || MainNotebook->GetPageCount () != 1)
			MainNotebook->DeletePage (0);
	}
#else
	// en 0.4 tenemos una pestaña más
	while (MainNotebook->GetPageCount () > 2)
		MainNotebook->DeletePage (0);
#endif
	ProcList.TheList.Clear ();

	// borrar la lista de procesos y la informacion
	ProcListPanel->ProcessList->DeleteAllItems ();
	ProcInfoPanel->InfoList->DeleteAllItems ();

	wxString text = _("Connected to server. Authenticating...");

	SetStatusText (text, 1);
	LoggingTab->Append (text);

	MenuItemDisconnect->Enable (TRUE);
	MenuItemProcessKill->Enable (TRUE);
	MenuItemProcessLaunch->Enable (TRUE);
	MenuItemProcessRestart->Enable (TRUE);
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
	Process *Proc = NULL, *NewProc = NULL;
	wxListItem newitem;
	static unsigned int first_seq = 0;

	switch (Pkt->GetCommand ())
	{
	case Session::ConsoleOutput:

		Proc = ProcList.Index (Pkt->GetIndex ());
		if (Proc != NULL)
			Proc->ProcConsole->Add (Pkt->GetData ());

		break;

	case Session::ConsoleSuccess:

		text.Printf ("Session::ConsoleSuccess on process %d", Pkt->GetIndex ());
		LoggingTab->Append (text);
		break;

	case Session::ConsoleLogs:

		Proc = ProcList.Index (Pkt->GetIndex ());
		if (Proc != NULL)
			Proc->ProcConsole->Add (Pkt->GetData ());

		if (first_seq == 0)
			first_seq = Pkt->GetSequence ();

		bytes += Pkt->GetLength ();
		text.Printf (_("Receiving logs for process %d: %.1f KB (%.1f%%)"),
			     Pkt->GetIndex (), (float)(bytes) / 1024.0,
			     100.0 * ((float)first_seq + 1 - (float)Pkt->GetSequence ())
			     / (float)first_seq);
		SetStatusText (text, 1);

		// si la secuencia es 1 entonces es el último paquete!
		if (Pkt->GetSequence () == 1)
		{
			text.Printf (_("Received %.1f KB of logs of process %d"),
				     (float)(bytes) / 1024.0, Pkt->GetIndex ());
			LoggingTab->Append (text);
			text = _("User: ") + Net->GetUsername ();
			text += _(" Server: [") + Net->GetAddress () + "]:"
				+ Net->GetPort ();
			SetStatusText (text, 1);
			bytes = first_seq = 0;
		}
		break;

	case Session::ProcessInfo:

		Proc = ProcList.Index (Pkt->GetIndex ());
		if (Proc == NULL)
			break;

		Proc->InfoString += Pkt->GetData ();

		// hay que agregar a ProcListPanel
		if (Pkt->GetSequence () == 1)
		{
			newitem.m_itemId = ProcListPanel->ProcessList->GetItemCount ();
			newitem.m_text = Proc->GetName ();
			newitem.m_data = Pkt->GetIndex ();
			newitem.m_image = ProcListPanel->ProcessIcons->GetIconIndex (Proc->GetType ());
			newitem.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_DATA |
					 wxLIST_MASK_IMAGE;

			ProcListPanel->ProcessList->InsertItem (newitem);
#ifdef __WXGTK__
			// ahora insertamos la página
			// en 0.4 ya tenemos dos pestañas agregadas
			MainNotebook->InsertPage (MainNotebook->GetPageCount () - 2,
				(wxNotebookPage *) Proc->ProcConsole,
				wxString::Format (_("Process %d: "), Pkt->GetIndex ())
					+ newitem.m_text,
				FALSE);
			Proc->ProcConsole->Hide ();
			//MainNotebook->SetSelection (MainNotebook->GetPageCount () - 1);
#endif
			LoggingTab->Append (wxString::Format (_("Added page \"Process %d\""),
							      Pkt->GetIndex ()));
		}
		break;

	case Session::Authorized:

		if (Pkt->GetIndex () == 0)
		{
			// somos un administrador!
			LoggingTab->Append (_("You are an administrator of this server"));
			break;
		}

		// creamos un nuevo proceso
		NewProc = new Process (Pkt->GetIndex ());
		NewProc->ProcConsole = new Console (MainNotebook, -1, Net, AppConfig,
						    MenuItemShowLogs, Pkt->GetIndex ());
#ifdef __WXMSW__
		NewProc->ProcConsole->Hide ();
#endif
		ProcList.TheList << NewProc;
		NewProc = NULL;
		
		LoggingTab->Append (wxString::Format (_("Creating new process with index %d"),
						      Pkt->GetIndex ()));
		break;

	case Session::ProcessExits:

		LoggingTab->Append (wxString::Format (_("Process %d exits: \"%s\""),
						      Pkt->GetIndex (), Pkt->GetData ()));
		break;

	case Session::ProcessStart:

		LoggingTab->Append (wxString::Format (_("Process %d start: \"%s\""),
						      Pkt->GetIndex (), Pkt->GetData ()));
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

	case Error::ProcessNotKilled:
		LoggingTab->Append ("Error::ProcessNotKilled");
		wxMessageBox (wxString::Format (_("Process %s couldn't be terminated."),
  						Pkt->GetData ()),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	case Error::ProcessNotLaunched:
		LoggingTab->Append ("Error::ProcessNotLaunched");
		wxMessageBox (wxString::Format (_("Process %s couldn't be launched."),
  						Pkt->GetData ()),
			      _("Error"),
			      wxOK | wxICON_ERROR, this);
		break;

	case Error::ProcessNotRestarted:
		LoggingTab->Append ("Error::ProcessNotRestarted");
		wxMessageBox (wxString::Format (_("Process %s couldn't be restarted."),
  						Pkt->GetData ()),
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

	MenuItemConnect->Enable (TRUE);
	MenuItemDisconnect->Enable (FALSE);
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

	MenuItemConnect->Enable (TRUE);
	MenuItemDisconnect->Enable (FALSE);
}

void MainWindow::OnConsoleFont (wxCommandEvent& WXUNUSED(event))
{
	wxFontData ActualFontData, NewFontData;
	wxNotebookPage *FirstNB = NULL;
	wxString FontName;
	long FontSize;
	unsigned int i;
	bool changing_console_font = TRUE;

	// FIXME: todo esto debiera hacerse en una ventana de configuración

	// obtenemos la fuente actual usada
	if (MainNotebook->GetPageCount () > 0)
	{
		if (MenuItemShowLogs->IsChecked () && MainNotebook->GetPageCount () == 2 &&
		    MainNotebook->GetSelection () == MainNotebook->GetPageCount () - 1)
		{
			// la pestaña de logs esta seleccionada
			// seleccionamos la fuente de las consolas
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
			ActualFontData.SetInitialFont (ConsoleFont);
		}
		else if (MenuItemShowLogs->IsChecked () &&
			 MainNotebook->GetSelection () == MainNotebook->GetPageCount () - 2)
		{
			// la pestaña de chat
			// seleccionamos la fuente del canal de chat
#ifdef __WXMSW__
			if (!AppConfig->Read ("/Channel/FontName", &FontName, "Courier New"))
				AppConfig->Write ("/Chat/FontName", "Courier New");
#else
			if (!AppConfig->Read ("/Channel/FontName", &FontName, "fixed"))
				AppConfig->Write ("/Chat/FontName", "fixed");
#endif
			if (!AppConfig->Read ("/Channel/FontSize", &FontSize, 12))
				AppConfig->Write ("/Chat/FontSize", 12);

			// creamos un wxFont con los valores leídos
			wxFont ConsoleFont (FontSize, wxDEFAULT, wxNORMAL,
					    wxNORMAL, FALSE, FontName);
			ActualFontData.SetInitialFont (ConsoleFont);
			changing_console_font = FALSE;
		}
		else
		{
			// una pestaña de proceso
			// seleccionamos la fuente de las consolas
			FirstNB = MainNotebook->GetPage (0);
			ActualFontData.SetInitialFont (((Console*)FirstNB)->GetConsoleFont ());
		}
	}
	else
	{
		// no hay ningún tab
		// seleccionamos la fuente de las consolas
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
		ActualFontData.SetInitialFont (ConsoleFont);
	}

	wxFontDialog *ConsoleFontDialog = new wxFontDialog (this, ActualFontData);

	if (ConsoleFontDialog->ShowModal () == wxID_OK)
	{
		// obtener el wxFontData y usarlo para
		// cambiar la fuente
		NewFontData = ConsoleFontDialog->GetFontData ();

		// los nuevos valores deben ser guardados en la configuración
		if (changing_console_font)
		{
			AppConfig->Write ("/Console/FontName",
					  (NewFontData.GetChosenFont ()).GetFaceName ());
			AppConfig->Write ("/Console/FontSize",
					  (NewFontData.GetChosenFont ()).GetPointSize ());

			for (i = 0; i <= ProcList.TheList.GetCount () - 1; ++i)
				ProcList[i]->ProcConsole->SetConsoleFont (NewFontData.GetChosenFont ());
		}
		else
		{
			AppConfig->Write ("/Channel/FontName",
					  (NewFontData.GetChosenFont ()).GetFaceName ());
			AppConfig->Write ("/Channel/FontSize",
					  (NewFontData.GetChosenFont ()).GetPointSize ());

			GeneralChannel->SetChannelFont (NewFontData.GetChosenFont ());
		}
	}
}

void MainWindow::OnConsoleSave (wxCommandEvent& WXUNUSED(event))
{
	int nb_selected;
	wxNotebookPage *CurrentNB;
	wxFileDialog *SaveDialog = new wxFileDialog (this, _("Save to file"),
						     "", "", "*.*",
						     wxSAVE | wxOVERWRITE_PROMPT);

	if (SaveDialog->ShowModal () == wxID_OK)
	{
		nb_selected = MainNotebook->GetSelection ();
		if (nb_selected >= 0)
		{
#ifdef __WXMSW__
			if (MenuItemShowLogs->IsChecked () && MainNotebook->GetPageCount () == 1)
#else
			// TODO: falta detectar la pestaña de chat
			if (MenuItemShowLogs->IsChecked () && nb_selected == MainNotebook->GetPageCount () - 1)
#endif
				LoggingTab->SaveOutputToFile (SaveDialog->GetPath ());
			else
			{
				CurrentNB = MainNotebook->GetPage (nb_selected);
				((Console *)(CurrentNB))->SaveConsoleToFile (SaveDialog->GetPath ());
			}
		}
	}
}

void MainWindow::OnProcessSelected (wxListEvent& event)
{
#ifdef __WXGTK__
	int i;
#endif
	int nb_selected;
        wxListItem info;
	Process *ProcessToShow;
	wxNotebookPage *CurrentNB;

        info.m_itemId = event.m_itemIndex;
        info.m_col = 0;
        info.m_mask = wxLIST_MASK_TEXT | wxLIST_MASK_DATA;

	ProcListPanel->ProcessList->GetItem (info);

	// primero obtener la consola dada por el índice,
	// si se está mostrando, salimos, si no, escondemos
	// la consola mostrada actualmente y mostramos la
	// nueva

	ProcessToShow = ProcList.Index (info.m_data);
	nb_selected = MainNotebook->GetSelection ();

	if (ProcessToShow->ProcConsole->IsShown ())
	{
#ifdef __WXMSW__
		if (nb_selected != 0)
			MainNotebook->SetSelection (0);
#else
		if (nb_selected != info.m_itemId)
			MainNotebook->SetSelection (info.m_itemId);
#endif
	}
	else
	{
#ifdef __WXMSW__
		// si se están mostrando los logs, no debe ser removido
		if (MainNotebook->GetPageCount () > 0)
		{
			if (!MenuItemShowLogs->IsChecked () || MainNotebook->GetPageCount () != 1)
			{
				CurrentNB = MainNotebook->GetPage (0);
				CurrentNB->Hide ();
				MainNotebook->RemovePage (0);
			}
		}

		MainNotebook->InsertPage (0, (wxNotebookPage *) ProcessToShow->ProcConsole,
					  wxString::Format (_("Process %d: "), info.m_data) +
					  info.m_text);

		MainNotebook->SetSelection (0);
#else
		// escondemos las páginas ya mostradas, excepto las
		// últimas que son la del chat y los logs
		for (i = 0; i < MainNotebook->GetPageCount () - 2; ++i)
		{
			CurrentNB = MainNotebook->GetPage (i);
			if (CurrentNB->IsShown ())
				CurrentNB->Hide ();
		}

		ProcessToShow->ProcConsole->Show ();
		MainNotebook->SetSelection (info.m_itemId);
#endif

		ProcInfoPanel->SetInfo (ProcessToShow->InfoString);
#ifdef __WXGTK__
		ProcessToShow->ProcConsole->ScrollToBottom ();
#endif
	}
}

void MainWindow::OnShowLogs (wxCommandEvent& WXUNUSED(event))
{
	if (MenuItemShowLogs->IsChecked ())
	{
#ifdef __WXMSW__
		MainNotebook->InsertPage (MainNotebook->GetPageCount (),
					  (wxNotebookPage *) LoggingTab, _("Logs"));
#else
		LoggingTab->Show ();
#endif
		AppConfig->Write ("/Options/ShowLogs", 1l);
	}
	else
	{
		LoggingTab->Hide ();
#ifdef __WXMSW__
		MainNotebook->RemovePage (MainNotebook->GetPageCount () - 1);
#endif
		AppConfig->Write ("/Options/ShowLogs", 0l);
	}
}

void MainWindow::OnProcessKill (wxCommandEvent& WXUNUSED(event))
{
	wxNotebookPage *CurrentNB;
	int nb_selected = MainNotebook->GetSelection ();
	unsigned int idx;
	wxListItem info;

	// FIXME: el proceso seleccionado se debería sacar
	// de la lista de procesos
/*	info.m_state = wxLIST_STATE_SELECTED;
	info.m_stateMask = wxLIST_STATE_SELECTED;
	info.m_col = 0;
	info.m_data = 0;
	info.m_mask = wxLIST_MASK_STATE;
	ProcListPanel->ProcessList->GetItem (info);

	printf ("m_data: %ld\n", info.m_data);
*/
	if (nb_selected >= 0)
	{
#ifdef __WXMSW__
		if (!MenuItemShowLogs->IsChecked () || MainNotebook->GetPageCount () != 1)
#else
		if (!MenuItemShowLogs->IsChecked () || nb_selected < MainNotebook->GetPageCount () - 2)
#endif
		{
			CurrentNB = MainNotebook->GetPage (nb_selected);
			idx = ((Console *)(CurrentNB))->GetIndex ();
			Net->AddOut (idx, Session::ProcessKill);
			Net->Send ();
		}
	}
}

void MainWindow::OnProcessLaunch (wxCommandEvent& WXUNUSED(event))
{
	wxNotebookPage *CurrentNB;
	int nb_selected = MainNotebook->GetSelection ();
	unsigned int idx;

	if (nb_selected >= 0)
	{
#ifdef __WXMSW__
		if (!MenuItemShowLogs->IsChecked () || MainNotebook->GetPageCount () != 1)
#else
		if (!MenuItemShowLogs->IsChecked () || nb_selected < MainNotebook->GetPageCount () - 2)
#endif
		{
			CurrentNB = MainNotebook->GetPage (nb_selected);
			idx = ((Console *)(CurrentNB))->GetIndex ();
			Net->AddOut (idx, Session::ProcessLaunch);
			Net->Send ();
		}
	}

}

void MainWindow::OnProcessRestart (wxCommandEvent& WXUNUSED(event))
{
	wxNotebookPage *CurrentNB;
	int nb_selected = MainNotebook->GetSelection ();
	unsigned int idx;

	if (nb_selected >= 0)
	{
#ifdef __WXMSW__
		if (!MenuItemShowLogs->IsChecked () || MainNotebook->GetPageCount () != 1)
#else
		if (!MenuItemShowLogs->IsChecked () || nb_selected < MainNotebook->GetPageCount () - 2)
#endif
		{
			CurrentNB = MainNotebook->GetPage (nb_selected);
			idx = ((Console *)(CurrentNB))->GetIndex ();
			Net->AddOut (idx, Session::ProcessRestart);
			Net->Send ();
		}
	}

}
