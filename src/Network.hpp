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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Network.hpp,v $
// $Revision: 1.1 $
// $Date: 2004/04/13 22:01:53 $
//

#ifndef __NETWORK_HPP__
#define __NETWORK_HPP__

#include <wx/wx.h>
#include <wx/thread.h>

#include "Protocol.hpp"
#include "Packet.hpp"

namespace NetEvt
{
	enum
	{
		Read,
		Write,
		Connect,
		Lost,
		FailConnect,
		FailRead,
		FailWrite
	};
}

class Network : private Protocol, public wxThread
{
private:
	// lista de paquetes
	Packet *Outgoing;
	Packet *Incoming;

	// datos
	wxString Address;
	wxString Port;
	wxString Username;
	wxString Password;
	wxEvtHandler *EvtParent;

	// estado
	bool Connected;
	bool Exiting;

public:
	Network (wxEvtHandler *parent,
		 wxString address,
		 wxString port,
		 wxString username,
		 wxString password);
	~Network ();

	void AddOut (Packet *NewItem);
	void AddFirstOut (Packet *NewItem);
	void AddIn (Packet *NewItem);
	void AddFirstIn (Packet *NewItem);

	int Connect (void);
	int Disconnect (bool exiting = FALSE);
	void Drop (void);

	int Send (void);
	int Receive (void);

	bool IsConnected (void);
	bool IsExiting (void);
	wxString GetAddress (void);
	wxString GetPort (void);
	wxString GetUsername (void);
	wxString GetPassword (void);
	Packet *Get (void);

	virtual ExitCode Entry (void);
};

#endif // __NETWORK_HPP__
