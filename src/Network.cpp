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
// $Revision: 1.3 $
// $Date: 2004/05/21 22:18:28 $
//

#include <cstdio>
#include <cstring>
#include <wx/socket.h>

#include "Network.hpp"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

Network::Network (wxEvtHandler *parent, wxString address, wxString port,
		  wxString username, wxString password)
	: wxThread (wxTHREAD_JOINABLE)
{
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
	
}

void Network::AddBuffer (List<Packet> &PktList, unsigned int type, const char *data)
{
	const char *p = data;
	int s;

	// data NO DEBE ser nulo!!!

	// calculamos cuantos paquetes necesitaremos
	// que corresponde a la parte entera más uno de
	// TamañoTotal / 1024
	s = (int) trunc (strlen (data) / PCKT_MAXDATA) + 1;

	while (strlen (p) >= PCKT_MAXDATA)
	{
		PktList << new Packet (type, s--, strlen (p), p); // asigna hasta PCKT_MAXDATA bytes
		p += PCKT_MAXDATA;
	}

	if (strlen (p) > 0 && strlen (p) < PCKT_MAXDATA)
		PktList << new Packet (type, s--, strlen (p), p);
}

void Network::AddBufferOut (unsigned int type, const char *data)
{
	OutgoingMutex.Lock ();
	AddBuffer (Outgoing, type, data);
	OutgoingMutex.Unlock ();
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
	Exiting = (exiting) ? TRUE : FALSE;

	// enviar un paquete de desconexión
	if (SendPacket (new Packet (Pckt::SessionDisconnect)) < 0)
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
	Packet *Sending = NULL;
	int bytes = 0, total = 0;

	while (Outgoing.GetCount ())
	{
		OutgoingMutex.Lock ();
		Sending = Outgoing.ExtractFirst ();
		OutgoingMutex.Unlock ();

		bytes = SendPacket (Sending);

		if (bytes < 0)
		{
			// paquete no se envió
			perror ("SendPacket");
			return -1; 
		}

		// borramos el paquete usado
		total += bytes;
		delete Sending;
	}

	return bytes;
}

int Network::Receive (void)
{
	Packet *Pkt = RecvPacket ();

	if (Pkt == NULL)
		return -1;

	IncomingMutex.Lock ();
	Incoming << Pkt;
	IncomingMutex.Unlock ();

	switch (Pkt->GetType ())
	{
		case Pckt::SessionDisconnect:
		case Pckt::ErrorServerFull:
		case Pckt::ErrorNotValidVersion:
		case Pckt::ErrorLoginFailed:
		case Pckt::ErrorAuthTimeout:
		case Pckt::ErrorServerQuit:
			return 1;
	}

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
	IncomingMutex.Lock ();
	Packet *Item = Incoming.ExtractFirst ();
	IncomingMutex.Unlock ();

	if (Item == NULL)
	{
		printf ("Incoming NULL\n");
		return NULL;
	}

	return Item;
}

void *Network::Entry (void)
{
	int val;
	bool send_now = FALSE;
	char hello_msg[21];

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
	snprintf (hello_msg, 20, "SAGS Client %s", VERSION);
	OutgoingMutex.Lock ();
	AddBuffer (Outgoing, Pckt::SyncHello, hello_msg);
	OutgoingMutex.Unlock ();
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

			// si estamos saliendo... salimos :)
			if (Exiting)
				break;

			// enviar evento
			wxSocketEvent ReadFailed (NetEvt::FailRead);
			EvtParent->AddPendingEvent ((wxEvent&) ReadFailed);
			break;
		}
		else
		{
			// manejamos la autenticación
			OutgoingMutex.Lock ();
			switch (Incoming[0]->GetType ())
			{
				case Pckt::SyncHello:
					Outgoing << new Packet (Pckt::SyncVersion, 1, 1, "1");
					send_now = TRUE;
					break;

				case Pckt::SyncVersion:
					AddBuffer (Outgoing, Pckt::AuthUsername, Username.c_str ());
					send_now = TRUE;
					break;

				case Pckt::AuthPassword:
					AddBuffer (Outgoing, Pckt::AuthPassword, Password.c_str ());
					send_now = TRUE;
					break;

				case Pckt::AuthSuccessful:
					Outgoing << new Packet (Pckt::SessionConsoleNeedLogs);
					send_now = TRUE;
					break;
			}
			OutgoingMutex.Unlock ();

			if (send_now)
			{
				Send ();
				send_now = FALSE;
			}

			// recibimos un paquete, enviar evento
			// la aplicación debiera leerlo con Net->Get ()
			wxSocketEvent ReadSuccessful (NetEvt::Read);
			EvtParent->AddPendingEvent ((wxEvent&) ReadSuccessful);

			// si es 1 entonces recibimos SessionDisconnect
			// o algún error por lo que debemos salir
			if (val == 1)
			{
				Drop ();
				break;
			}
		}
	}

	return (ExitCode) 0;
}
