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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Main.cpp,v $
// $Revision: 1.6 $
// $Date: 2004/06/22 02:44:29 $
//

#include "Main.hpp"
#include "Window.hpp"

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

IMPLEMENT_APP (Application)

bool Application::OnInit (void)
{
	AppLocale.Init (wxLANGUAGE_DEFAULT);
#ifdef __WXMSW__
	AppLocale.AddCatalogLookupPathPrefix ("locale");
#endif
	AppLocale.AddCatalog (GETTEXT_PACKAGE);

	MainWindow *AppWindow = new MainWindow (wxString::Format (_("SAGS Client %s"),
								  VERSION),
						wxDefaultPosition,
						wxDefaultSize);
	AppWindow->Show (TRUE);
	SetTopWindow (AppWindow);

	return TRUE;
}
