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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Protocol.hpp,v $
// $Revision: 1.1 $
// $Date: 2004/04/13 22:01:53 $
//

#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <openssl/ssl.h>

#include "Packet.hpp"

class Protocol
{
protected:
	SSL_METHOD *method;
	SSL_CTX *context;
	SSL *ssl;
#ifdef _WIN32
	BIO *bio_server;
#else
	int socketd;
#endif

public:
	Protocol ();
	~Protocol ();

	int Connect (const char *address, const char *port);
	void Disconnect (void);

	int SendPacket (Packet *Pkt);
	Packet *RecvPacket (void);
};

#endif // __PROTOCOL_HPP__
