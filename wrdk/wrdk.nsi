;NSIS Modern User Interface

;--------------------------------
;Include Modern UI

  !include "MUI.nsh"
  !include "EnvVarUpdate.nsh"

;--------------------------------
;General

  ;Name and file
  Name "WikiReader Development Kit"
  OutFile "wrdk-installer.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\wrdk"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Seabright\wrdk" ""

  ;Vista redirects $SMPROGRAMS to all users without this
  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var MUI_TEMP
  Var STARTMENU_FOLDER

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Definitions

!define SHCNE_ASSOCCHANGED 0x8000000
!define SHCNF_IDLIST 0

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Seabright\wrdk" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES

  !define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\doc\index.html"

  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "wrdk" SecMain

  SetOutPath "$INSTDIR"
  
  File /r build/win32/wrdk/*
  
  ;Store installation folder
  WriteRegStr HKCU "Software\Seabright\wrdk" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Documentation.lnk" "$INSTDIR\doc\index.html"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Examples.lnk" "$INSTDIR\examples"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"

  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\bin"
  ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\msys\bin"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "MSYS" SecMSYS

  SetOutPath "$INSTDIR"
  
  File /r build/win32/msys
  
  ;Store installation folder
  WriteRegStr HKCU "Software\Seabright\wrdk" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMain} "The main development"
    !insertmacro MUI_DESCRIPTION_TEXT ${SecMSYS} "MSYS command line utilities used to build the examples"
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\$STARTMENU_FOLDER"

  DeleteRegKey /ifempty HKCU "Software\Seabright\wrdk"
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\bin"
  ${un.EnvVarUpdate} $0 "PATH" "R" "HKLM" "$INSTDIR\msys\bin"

SectionEnd
