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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Icons.cpp,v $
// $Revision: 1.2 $
// $Date: 2004/08/29 22:16:08 $
//

#include "Icons.hpp"
#include <wx/dir.h>
#include <wx/image.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

IconList::IconList ()
{
	wxBitmap *p_icon = NULL;
	struct proc_type *newitem = NULL;
	wxString filename, absolute_filename;
	bool cont;
	int dash, point;

	// las imágenes son de 27x24 px.
	ImageList = new wxImageList (27, 24);

	// cargamos los manejadores de los formatos de imágenes
	wxInitAllImageHandlers ();

	// cargar las imágenes desde PACKAGE_PIXMAPS_DIR/icon-*.png
	// y agregarlas a ImageList y a Inventory.
	wxDir pixmaps_dir (PACKAGE_PIXMAPS_DIR);

	if (pixmaps_dir.IsOpened ())
	{
		cont = pixmaps_dir.GetFirst (&filename, "icon-*.png", wxDIR_FILES);
		while (cont)
		{
			absolute_filename = PACKAGE_PIXMAPS_DIR "/" + filename;
			
			newitem = new struct proc_type;
			p_icon = new wxBitmap (absolute_filename, wxBITMAP_TYPE_PNG);

			newitem->pt_index = ImageList->Add (*p_icon);
			dash = filename.Find ('-');
			point = filename.Find ('.');
			newitem->pt_name = filename.Mid (dash + 1, point - dash - 1);
			Inventory << newitem;

			delete p_icon;
			cont = pixmaps_dir.GetNext (&filename);
		}
	}
}

IconList::~IconList ()
{
	// borrar la lista de imágenes
	delete ImageList;
}

wxImageList *IconList::GetImageList (void)
{
	return ImageList;
}

int IconList::GetIconIndex (wxString type)
{
	int colon_position = type.Find (':');
	struct proc_type item, *found;

	if (colon_position < 0)
		return colon_position;

	item.pt_name = type.Mid (0, colon_position);
	found = Inventory.Find (item);

	if (found == NULL)
		return -1;

	return found->pt_index;
}

StatusIconList::StatusIconList ()
{
	wxBitmap *s_icon = NULL;
	struct status_type *newitem = NULL;
	wxString filename, absolute_filename;
	bool cont;
	int dash, point;

	// las imágenes son de 13x13 px.
	ImageList = new wxImageList (13, 13);

	// cargamos los manejadores de los formatos de imágenes
	// FIXME: a lo mejor ya se cargó antes
	//wxInitAllImageHandlers ();

	// cargar las imágenes desde PACKAGE_PIXMAPS_DIR/chat-*.png
	// y agregarlas a ImageList y a Inventory.
	wxDir pixmaps_dir (PACKAGE_PIXMAPS_DIR);

	if (pixmaps_dir.IsOpened ())
	{
		cont = pixmaps_dir.GetFirst (&filename, "chat-*.png", wxDIR_FILES);
		while (cont)
		{
			absolute_filename = PACKAGE_PIXMAPS_DIR "/" + filename;
			
			newitem = new struct status_type;
			s_icon = new wxBitmap (absolute_filename, wxBITMAP_TYPE_PNG);

			newitem->st_index = ImageList->Add (*s_icon);
			dash = filename.Find ('-');
			point = filename.Find ('.');
			newitem->st_name = filename.Mid (dash + 1, point - dash - 1);
			Inventory << newitem;

			delete s_icon;
			cont = pixmaps_dir.GetNext (&filename);
		}
	}
}

StatusIconList::~StatusIconList ()
{
	// borrar la lista de imágenes
	delete ImageList;
}

wxImageList *StatusIconList::GetImageList (void)
{
	return ImageList;
}

int StatusIconList::GetIconIndex (wxString type)
{
	struct status_type item, *found;

	item.st_name = type;
	found = Inventory.Find (item);

	if (found == NULL)
		return -1;

	return found->st_index;
}
