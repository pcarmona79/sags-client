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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Protocol.cpp,v $
// $Revision: 1.3 $
// $Date: 2004/06/22 02:44:29 $
//

#include <cstdio>
#include <cstring>
#include <sys/types.h>
#ifndef _WIN32
#  include <sys/socket.h>
#  include <netdb.h>
#  include <unistd.h>
#endif
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "Protocol.hpp"

Protocol::Protocol ()
{
#ifdef _WIN32
	bio_server = NULL;
#else
	socketd = 0;
#endif
	OpenSSL_add_all_algorithms ();
	SSL_load_error_strings ();
	method = SSLv3_client_method ();
	context = SSL_CTX_new (method);
	ssl = SSL_new (context);
}

Protocol::~Protocol ()
{
	SSL_free (ssl);
	SSL_CTX_free (context);
#ifdef _WIN32
	BIO_free (bio_server);
#endif
}

int Protocol::Connect (const char *address, const char *port)
{
#ifdef _WIN32
	int error;
	BIO_METHOD *bio_method;

	bio_method = BIO_s_connect ();
	bio_server = BIO_new (bio_method);
	BIO_set_conn_hostname (bio_server, address);
	BIO_set_conn_port (bio_server, port);

	if (BIO_do_connect(bio_server) <= 0)
	{
		fprintf(stderr, "Error connecting to server\n");
		ERR_print_errors_fp(stderr);
		return -1;
	}

	SSL_set_bio (ssl, bio_server, bio_server);
	error = SSL_connect (ssl);
	if (error < 0)
	{
		fprintf (stderr, "Error: %s",
			 ERR_error_string (SSL_get_error (ssl, error), NULL));
		return -1;
	}

	printf ("SSL connection active with %s cipher\n",
		SSL_get_cipher (ssl));

	return 0;
#else
	int error, success = 0;
	struct addrinfo hints, *res, *results;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo (address, port, &hints, &res);
	if (error)
		perror (gai_strerror (error));
	else
	{
		for (results = res; results; results = results->ai_next)
		{
			socketd = socket (results->ai_family, results->ai_socktype, 0);
			if (socketd < 0)
			{
				perror ("socket");
				continue;
			}

			if (connect (socketd, results->ai_addr, results->ai_addrlen) < 0)
			{
				perror ("connect");
				continue;
			}

			SSL_set_fd (ssl, socketd);

			error = SSL_connect (ssl);
			if (error < 0)
			{
				fprintf (stderr, "Error: %s",
					 ERR_error_string (SSL_get_error (ssl, error), NULL));
				continue;
			}

			printf ("SSL connection active with %s cipher via socket %d\n",
				SSL_get_cipher (ssl), socketd);


			success = 1;
			break;
		}

		freeaddrinfo (res);
	}

	if (!success)
		return -1;

	return 0;
#endif
}

void Protocol::Disconnect (void)
{
	SSL_shutdown (ssl);

#ifdef _WIN32
	BIO_free (bio_server);
#else
	printf ("Closing socket %d\n", socketd);
	if (close (socketd))
		perror ("close");
#endif
}

int Protocol::SendPacket (Packet *Pkt)
{
	struct pkt sent;
	int bytes;
	char minidata[14];

	memset (minidata, 0, 14);
	strncpy (minidata, Pkt->GetData (), 10);
	if (strlen (Pkt->GetData ()) > 10)
		strncat (minidata, "...", 3);
	
	printf ("Packet to send: IDX: %02X COM: %02X SEQ: %d LEN: %d DATA: \"%s\"\n",
		Pkt->GetIndex (), Pkt->GetCommand (), Pkt->GetSequence (),
		Pkt->GetLength (), minidata);

	sent.pkt_header = Pkt->GetHeader ();
	strncpy (sent.pkt_data, Pkt->GetData (), Pkt->GetLength ());

	bytes = SSL_write (ssl, &sent, sizeof (struct pkt_hdr) + Pkt->GetLength ());
	if (bytes < 0)
		return -1;

	return bytes;
}

Packet *Protocol::RecvPacket (void)
{
	int bytes, total = 0;
	struct pkt header;
	Packet *Pkt = NULL;
	char minidata[14];

	memset (&header, 0, sizeof (header));

	bytes = SSL_read (ssl, &header, sizeof (struct pkt_hdr));

	if (bytes <= 0)
		return NULL;
	total += bytes;

	if (header.pkt_header.pkt_seq >= 1)
	{
		bytes = SSL_read (ssl, header.pkt_data, header.pkt_header.pkt_len + 1);
		if (bytes <= 0)
			return NULL;
		total += bytes;
	}

	Pkt = new Packet (header);
	
	memset (minidata, 0, 14);
	strncpy (minidata, Pkt->GetData (), 10);
	if (strlen (Pkt->GetData ()) > 10)
		strncat (minidata, "...", 3);

	printf ("Packet received: IDX: %02X COM: %02X SEQ: %d LEN: %d DATA: \"%s\"\n",
		Pkt->GetIndex (), Pkt->GetCommand (), Pkt->GetSequence (),
		Pkt->GetLength (), minidata);

	return Pkt;
}
