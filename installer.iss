[Setup]
AppName=Simple Archiver
AppVersion=1.0
DefaultDirName={autopf}\SimpleArchiver
DefaultGroupName=Simple Archiver
OutputDir=Output
OutputBaseFilename=SimpleArchiver_Setup
Compression=lzma
SolidCompression=yes

[Files]
Source: "dist\gui.exe"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Simple Archiver"; Filename: "{app}\gui.exe"
Name: "{autodesktop}\Simple Archiver"; Filename: "{app}\gui.exe"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Add an icon to the desktop"