;
; SAGS - Secure Administrator of Game Servers
; Copyright (C) 2004 Pablo Carmona Amigo
;
; This file is part of SAGS Client.
;
; SAGS Client is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; SAGS Client is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU Library General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
;
; $Source: /home/pablo/Desarrollo/sags-cvs/client/win32/Attic/sagscl.iss,v $
; $Revision: 1.2 $
; $Date: 2004/05/18 05:37:31 $
;

[Setup]
AppName=SAGS Client
AppVerName=SAGS Client version 0.1.2
AppVersion=0.1.2
AppPublisher=Pablo Carmona Amigo
AppPublisherURL=http://sags.sourceforge.net/
AppCopyright=Copyright (C) 2004 Pablo Carmona Amigo
AppSupportURL=http://sags.sourceforge.net/
AppUpdatesURL=http://sags.sourceforge.net/
CreateAppDir=yes
DefaultDirName={pf}\SAGS Client
DefaultGroupName=SAGS Client
LicenseFile=..\COPYING
Compression=bzip/9
SourceDir=.
OutputDir=.
OutputBaseFilename=sagscl-0.1.2-setup

[Files]
Source: "sagscl.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\COPYING"; DestDir: "{app}"; DestName: "License.txt"; Flags: ignoreversion
Source: "..\README"; DestDir: "{app}"; DestName: "Readme.txt"; Flags: ignoreversion isreadme
Source: "libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "libssl32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\pixmaps\sagscl.png"; DestDir: "{app}\pixmaps"; Flags: ignoreversion
Source: "..\po\es.gmo"; DestDir: "{app}\locale\es\LC_MESSAGES"; DestName: "sagscl.mo"; Flags: ignoreversion

[Icons]
Name: "{group}\SAGS Client"; Filename: "{app}\sagscl.exe"; WorkingDir: "{app}"
Name: "{group}\SAGS Website"; Filename: "http://sags.sourceforge.net/"
Name: "{group}\Uninstall SAGS Client"; Filename: "{uninstallexe}"
