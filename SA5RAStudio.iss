#define MyAppName "SA5RA STUDIO"
#define MyAppVersion "0.2.0"
#define MyAppPublisher "SA5RA Audio"
#define MyAppExeName "SA5RAStudio.exe"

[Setup]
AppId={{9C5A5A52-4C9B-4A65-9F2B-5A5A7A020200}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={localappdata}\Programs\SA5RA STUDIO
DefaultGroupName=SA5RA STUDIO
OutputDir=..\dist
OutputBaseFilename=SA5RAStudioSetup
SetupIconFile=..\assets\SA5RAStudio.ico
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop shortcut"; GroupDescription: "Additional shortcuts:"; Flags: unchecked

[Files]
Source: "..\package\SA5RAStudio.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\package\SA5RAStudio.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\package\README.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\SA5RA STUDIO"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\SA5RAStudio.ico"
Name: "{autodesktop}\SA5RA STUDIO"; Filename: "{app}\{#MyAppExeName}"; IconFilename: "{app}\SA5RAStudio.ico"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch SA5RA STUDIO"; Flags: postinstall nowait skipifsilent
