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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Panels.hpp,v $
// $Revision: 1.2 $
// $Date: 2004/06/22 02:44:29 $
//

#ifndef __PANELS_HPP__
#define __PANELS_HPP__

#include <wx/wx.h>
#include <wx/listctrl.h>

class ListPanel : public wxPanel
{
public:
	ListPanel (wxWindow *parent, wxWindowID id);
	~ListPanel ();

	wxListCtrl *ProcessList;
};

class InfoPanel : public wxPanel
{
public:
	InfoPanel (wxWindow *parent, wxWindowID id);
	~InfoPanel ();

	wxListCtrl *InfoList;

	void SetInfo (wxString info);
};

#endif // __PANELS_HPP__
