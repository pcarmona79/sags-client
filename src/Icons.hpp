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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Icons.hpp,v $
// $Revision: 1.1 $
// $Date: 2004/07/07 00:06:19 $
//

#ifndef __ICONS_HPP__
#define __ICONS_HPP__

#include <wx/wx.h>
#include <wx/imaglist.h>
#include "List.hpp"

struct proc_type
{
	wxString pt_name;
	int pt_index;

	proc_type ()
	{
		pt_name = wxEmptyString;
		pt_index = 0;
	}

	bool operator== (const struct proc_type &op)
	{
		return (pt_name == op.pt_name);
	}
};

class IconList
{
private:
	wxImageList *ImageList;
	List<struct proc_type> Inventory;

public:
	IconList ();
	~IconList ();

	wxImageList *GetImageList (void);
	int GetIconIndex (wxString type);
};

#endif // __ICONS_HPP__
