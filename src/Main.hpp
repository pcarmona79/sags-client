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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Main.hpp,v $
// $Revision: 1.3 $
// $Date: 2004/06/19 05:28:08 $
//

#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <wx/wx.h>
#include "Network.hpp"
#include "Window.hpp"

class Application : public wxApp
{
protected:
	wxLocale AppLocale;

public:
	virtual bool OnInit (void);
};

#endif // __MAIN_HPP__
