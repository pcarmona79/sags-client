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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Network.cpp,v $
// $Revision: 1.1 $
// $Date: 2004/04/13 22:01:53 $
//

#include <cstdio>
#include <cstring>
#include <wx/socket.h>

#include "Network.hpp"

Network::Network (wxEvtHandler *parent, wxString address, wxString port,
		  wxString username, wxString password)
	: wxThread (wxTHREAD_JOINABLE)
{
	Outgoing = NULL;
	Incoming = NULL;
	EvtParent = parent;
	Address = address;
	Port = port;
	Username = username;
	Password = password;
	Connected = FALSE;
	Exiting = FALSE;
}

Network::~Network ()
{
	Packet *UnLink;

	// liberamos la lista de paquetes
	while (Outgoing)
	{
		UnLink = Outgoing;
		Outgoing = UnLink->Next;
		delete UnLink;
	}
	while (Incoming)
	{
		UnLink = Incoming;
		Incoming = UnLink->Next;
		delete UnLink;
	}
}

void Network::AddOut (Packet *NewItem)
{
	Packet *Searched = Outgoing;

	if (Searched)
	{
		// buscamos el último elemento
		while (Searched->Next)
			Searched = Searched->Next;
		Searched->Next = NewItem;
	}
	else
		Outgoing = NewItem;  // lista estaba vacía
}

void Network::AddFirstOut (Packet *NewItem)
{
	NewItem->Next = Outgoing;
	Outgoing = NewItem;
}

void Network::AddIn (Packet *NewItem)
{
	Packet *Searched = Incoming;

	if (Searched)
	{
		// buscamos el último elemento
		while (Searched->Next)
			Searched = Searched->Next;
		Searched->Next = NewItem;
	}
	else
		Incoming = NewItem;  // lista estaba vacía
}

void Network::AddFirstIn (Packet *NewItem)
{
	NewItem->Next = Incoming;
	Incoming = NewItem;
}

int Network::Connect (void)
{
	int retval = Protocol::Connect (Address.c_str (), Port.c_str ());

	if (retval)
		Connected = FALSE;
	else
		Connected = TRUE;

	return retval;
}

int Network::Disconnect (bool exiting)
{
	Packet *Disc;

	Exiting = (exiting) ? TRUE : FALSE;

	// enviar un paquete de desconexión
	Disc = new Packet (Pckt::SessionDisconnect);

	if (SendPacket (Disc) < 0)
		return -1;

	return 0;
}

void Network::Drop (void)
{
	// cerramos la conexión SSL
	Protocol::Disconnect ();
	Connected = FALSE;
}

int Network::Send (void)
{
	Packet *Sending = Outgoing;
	int bytes = 0, total = 0;

	while (Outgoing)
	{
		bytes = SendPacket (Sending);

		if (bytes < 0)
		{
			// paquete no se envió
			perror ("SendPacket");
			return -1; 
		}

		// avanzamos la lista en un paquete
		// y borramos el paquete usado
		total += bytes;
		Outgoing = Outgoing->Next;
		delete Sending;
		Sending = Outgoing;
	}

	return bytes;
}

int Network::Receive (void)
{
	Packet *Pkt = RecvPacket ();

	if (Pkt == NULL)
		return -1;

	AddIn (Pkt);

	if (Pkt->GetType () == Pckt::SessionDisconnect)
		return 1;

	return 0;
}

bool Network::IsConnected (void)
{
	return Connected;
}

bool Network::IsExiting (void)
{
	return Exiting;
}

wxString Network::GetAddress (void)
{
	return Address;
}

wxString Network::GetPort (void)
{
	return Port;
}

wxString Network::GetUsername (void)
{
	return Username;
}

wxString Network::GetPassword (void)
{
	return Password;
}

Packet *Network::Get (void)
{
	Packet *Item = Incoming;

	if (Incoming == NULL)
	{
		printf ("Incoming NULL\n");
		return NULL;
	}

	Incoming = Incoming->Next;
	//Item->Next = NULL;

	return Item;
}

void *Network::Entry (void)
{
	int val;
	Packet *StartAuth, *Ans = NULL;

	if (!Connected)
		Connect ();

	if (!Connected)
	{
		// hay que enviar un evento de NetEvt::FailConnect
		wxSocketEvent ConnectFailed (NetEvt::FailConnect);
		EvtParent->AddPendingEvent ((wxEvent&) ConnectFailed);
		return NULL;
	}

	// enviar un evento de NetEvt::Connect
	wxSocketEvent ConnectSuccessful (NetEvt::Connect);
	EvtParent->AddPendingEvent ((wxEvent&) ConnectSuccessful);

	// La autenticación comieza enviando un SyncHello
	StartAuth = new Packet (Pckt::SyncHello, "RPA Client 0.1");
	printf ("StartAuth: TYPE: %04X SEQ: %d LEN: %d DATA: \"%s\"\n",
		StartAuth->GetType (), StartAuth->GetSequence (),
		StartAuth->GetLength (), StartAuth->GetData ());
	AddOut (StartAuth);
	Send ();

	// Si se leen datos se envía el evento NetEvt::Read
	// Si falla algo al leer, enviamos NetEvt::FailRead y salimos
	while (1)
	{
		// nos bloqueamos esperando un paquete
		val = Receive ();
		if (val < 0)
		{
			// error al leer, cerrar la conexión
			Drop ();

			// enviar evento
			wxSocketEvent ReadFailed (NetEvt::FailRead);
			EvtParent->AddPendingEvent ((wxEvent&) ReadFailed);
			break;
		}
		else
		{
			// manejamos la autenticación
			switch (Incoming->GetType ())
			{
				case Pckt::SyncHello:
					Ans = new Packet (Pckt::SyncVersion, 1, 1, "1");
					break;

				case Pckt::SyncVersion:
					Ans = new Packet (Pckt::AuthUsername, Username.c_str ());
					break;

				case Pckt::AuthPassword:
					Ans = new Packet (Pckt::AuthPassword, Password.c_str ());
					break;

				case Pckt::AuthSuccessful:
					Ans = new Packet (Pckt::SessionConsoleNeedLogs);
					break;
			}

			if (Ans != NULL)
			{
				AddOut (Ans);
				Send ();
				Ans = NULL;
			}

			// recibimos un paquete, enviar evento
			// la aplicación debiera leerlo con Net->Get ()
			wxSocketEvent ReadSuccessful (NetEvt::Read);
			EvtParent->AddPendingEvent ((wxEvent&) ReadSuccessful);

			// si es 1 entonces recibimos SessionDisconnect
			// por lo que debemos salir
			if (val == 1)
			{
				Drop ();
				break;
			}
		}
	}

	return (ExitCode) 0;
}
