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
// $Revision: 1.7 $
// $Date: 2004/06/17 08:26:37 $
//

#include <cstdio>
#include <cstring>
#include <cmath>
#include <wx/socket.h>
#include <openssl/md5.h>

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

void Network::AddBuffer (List<Packet> &PktList, unsigned int idx,
			 unsigned int com, const char *data)
{
	const char *p = data;
	int s;

	// data NO DEBE ser nulo!!!

	// calculamos cuantos paquetes necesitaremos
	// que corresponde a la parte entera más uno de
	// TamañoTotal / PCKT_MAXDATA
	s = (int) trunc (strlen (data) / PCKT_MAXDATA) + 1;

	while (strlen (p) >= PCKT_MAXDATA)
	{
		PktList << new Packet (idx, com, s--, strlen (p), p);
		p += PCKT_MAXDATA;
	}

	if (strlen (p) > 0 && strlen (p) < PCKT_MAXDATA)
		PktList << new Packet (idx, com, s--, strlen (p), p);
}

void Network::AddBufferOut (unsigned int idx, unsigned int com, const char *data)
{
	OutgoingMutex.Lock ();
	AddBuffer (Outgoing, idx, com, data);
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
	if (SendPacket (new Packet (Session::MainIndex, Session::Disconnect)) < 0)
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

	if (Pkt->GetIndex () == Error::Index)
	{
		switch (Pkt->GetCommand ())
		{
			case Error::ServerFull:
			case Error::NotValidVersion:
			case Error::LoginFailed:
			case Error::AuthTimeout:
			case Error::ServerQuit:
				return 1;
		}
	}
	else if (Pkt->GetIndex () >= Session::MainIndex &&
		 Pkt->GetIndex () <= Session::MaxIndex)
	{
		switch (Pkt->GetCommand ())
		{
			case Session::Disconnect:
				return 1;
		}
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

wxString Network::GetMD5 (wxString password)
{
	char *md5_password;
	char *md5_password_hex;
	char hexadecimal[3];
	int i;

	md5_password = new char [MD5_DIGEST_LENGTH + 1];
	memset (md5_password, 0, MD5_DIGEST_LENGTH + 1);
	md5_password_hex = new char [2 * MD5_DIGEST_LENGTH + 1];
	memset (md5_password_hex, 0, 2 * MD5_DIGEST_LENGTH + 1);

	MD5 ((unsigned char *) password.c_str (), password.Length (),
	     (unsigned char *) md5_password);

	for ( i = 0; i < MD5_DIGEST_LENGTH; ++i ) {
		snprintf (hexadecimal, 3, "%.2x", *(md5_password + i));
		strncat (md5_password_hex, hexadecimal, sizeof (hexadecimal));
	}

	wxString retstr = md5_password_hex;

	delete[] md5_password;
	delete[] md5_password_hex;

	return retstr;
}

void *Network::Entry (void)
{
	int val;
	bool send_now = FALSE;
	wxString hello_msg, pwdhash;

	if (!Connected)
		Connect ();

	if (!Connected)
	{
		// hay que enviar un evento de NetEvt::FailConnect
		wxSocketEvent ConnectFailed (NetEvt::FailConnect);
		EvtParent->AddPendingEvent ((wxEvent&) ConnectFailed);
		return (ExitCode) -1;
	}

	// enviar un evento de NetEvt::Connect
	wxSocketEvent ConnectSuccessful (NetEvt::Connect);
	EvtParent->AddPendingEvent ((wxEvent&) ConnectSuccessful);

	// La autenticación comieza enviando un Sync::Hello
	hello_msg.Printf ("SAGS Client %s", VERSION);
	OutgoingMutex.Lock ();
	AddBuffer (Outgoing, Sync::Index, Sync::Hello, hello_msg.c_str ());
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
			if (Incoming[0]->GetIndex () == Sync::Index)
			{
				switch (Incoming[0]->GetCommand ())
				{
				case Sync::Hello:
					// ahora menejamos la versión 2
					// del protocolo
					Outgoing << new Packet (Sync::Index, Sync::Version,
								1, 1, "2");
					send_now = TRUE;
					break;

				case Sync::Version:
					AddBuffer (Outgoing, Auth::Index, Auth::Username,
						   Username.c_str ());
					send_now = TRUE;
					break;
				}
			}
			else if (Incoming[0]->GetIndex () == Auth::Index)
			{
				switch (Incoming[0]->GetCommand ())
				{
				case Auth::RandomHash:
					pwdhash = Incoming[0]->GetData ();
					pwdhash += Password;
					AddBuffer (Outgoing, Auth::Index, Auth::Password,
						   (GetMD5 (pwdhash)).c_str ());
					send_now = TRUE;
					break;

					// ahora hay que esperar que lleguen
					// los paquetes Session::Authorized
				}
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
