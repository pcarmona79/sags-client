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
// $Source: /home/pablo/Desarrollo/sags-cvs/client/src/Config.hpp,v $
// $Revision: 1.1 $
// $Date: 2004/08/12 02:12:39 $
//

#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include <wx/wx.h>
#include <wx/config.h>
#include "Process.hpp"
#include "Channel.hpp"

class ConfigDialog : public wxDialog
{
private:
	wxButton *ButtonConsoleFont;
	wxButton *ButtonChannelFont;

	wxFontData ConsoleFontData;
	wxFontData ChannelFontData;

	wxConfig *AppConfig;
	ProcessTree *ProcList;
	Channel *GeneralChannel;

public:
	ConfigDialog (wxConfig *AppCfg,
		      ProcessTree *ProcLst,
		      Channel *GenCh,
		      wxWindow *parent,
		      wxWindowID id,
		      const wxString& title,
		      const wxPoint& pos = wxDefaultPosition,
		      const wxSize& size = wxDefaultSize,
		      long style = wxDEFAULT_DIALOG_STYLE);
	~ConfigDialog ();

	wxString GetConsoleFontName (void);
	int GetConsoleFontSize (void);
	wxString GetChannelFontName (void);
	int GetChannelFontSize (void);

	void OnButtonConsoleFont (wxCommandEvent& event);
	void OnButtonChannelFont (wxCommandEvent& event);
};

#endif // __CONFIG_HPP__
