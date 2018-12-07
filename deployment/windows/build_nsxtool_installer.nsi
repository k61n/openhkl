
!include "MUI.nsh"
!include "x64.nsh"
!include "LogicLib.nsh"

; The name of the installer
Name NSXTool ${VERSION}"

; The name of the installer file to write
OutFile "${TARGET_DIR}\nsxtool-${VERSION}-${ARCH}.exe"

RequestExecutionLevel admin #NOTE: You still need to check user rights with UserInfo!

; The default installation directory
InstallDir "$PROGRAMFILES\NSXTool"

; Registry key to check for directory (so if you install again, it will overwrite the old one automatically)
InstallDirRegKey HKLM "Software\NSXTool" "Install_Dir"

; Will show the details of installation
ShowInstDetails show

; Will show the details of uninstallation
ShowUnInstDetails show

!define PUBLISHER "Institut Laue-Langevin"
!define WEB_SITE "https://code.ill.fr/scientific-software/nsxtool"
!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\NSXTool"
!define UNINST_ROOT_KEY "HKLM"

!define ICONS_DIR $INSTDIR\icons

; Prompt the user in case he wants to cancel the installation
!define MUI_ABORTWARNING

; define the icon for the installer file and the installer 'bandeau'
!define MUI_ICON   "icons\nsxtool.ico"
!define MUI_UNICON "icons\nsxtool-uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "icons\nsxtool.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "icons\nsxtool-uninstall.bmp"

!define WEB_ICON   "icons\website.ico"

!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of \
NSXTool release ${VERSION}.\
\n\nNSXTool is a software for reducing single crystal experimental data."

; Insert a "Welcome" page in the installer
!insertmacro MUI_PAGE_WELCOME

; Insert a "License" page in the installer
!insertmacro MUI_PAGE_LICENSE "..\..\License.txt"

; Insert a page to browse for the installation directory
!insertmacro MUI_PAGE_DIRECTORY

; Insert a page for actual installation (+display) of NSXTool
!insertmacro MUI_PAGE_INSTFILES

; Insert in the finish page the possibility to run NSXTool
!define MUI_FINISHPAGE_RUN_NOTCHECKED
!define MUI_FINISHPAGE_RUN_TEXT "Start NSXTool ${VERSION}"
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\nsxqt.exe"
; Actually insert the finish page to the installer
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Set the installer language to english
!insertmacro MUI_LANGUAGE "English"

;RequestExecutionLevel user

Function .onInit
  ${If} ${ARCH} == "win-amd64"
    StrCpy $INSTDIR "$PROGRAMFILES64\NSXTool"
  ${Else}
    StrCpy $INSTDIR "$PROGRAMFILES\NSXTool"
  ${EndIf}
FunctionEnd

Section "NSXTool ${VERSION}" SEC01
  SetShellVarContext all
  SetOutPath "$INSTDIR"
  SetOverwrite on
  File /r ..\..\build\bin

  File "..\..\LICENSE.txt"
  
  SetShellVarContext Current
  CreateDirectory "$APPDATA\nsxtool"
  SetOutPath "$APPDATA\nsxtool"
  SetOverwrite on
  File /r ..\..\build\share\nsxtool\*

  SetShellVarContext All
  CreateDirectory "$APPDATA\nsxtool"
  SetOutPath "$APPDATA\nsxtool"
  SetOverwrite on
  File /r ..\..\build\share\nsxtool\*

  CreateDirectory "${ICONS_DIR}"
  SetOutPath "${ICONS_DIR}"
  SetOverwrite on
  File /oname=run.ico "${MUI_ICON}"
  File /oname=uninstall.ico "${MUI_UNICON}"
  File /oname=web.ico "${WEB_ICON}"
  SetOutPath "$INSTDIR"
  SetOverwrite on
  CreateShortCut "$DESKTOP\NSXTool.lnk" "$INSTDIR\bin\nsxqt.exe" "" "${ICONS_DIR}\run.ico" 0
  CreateDirectory "$SMPROGRAMS\NSXTool"
  CreateShortCut "$SMPROGRAMS\NSXTool\NSXTool.lnk" "$INSTDIR\bin\nsxqt.exe" "" "${ICONS_DIR}\run.ico" 0
  CreateShortCut "$SMPROGRAMS\NSXTool\Uninstall.lnk" "$INSTDIR\uninst.exe" "" "${ICONS_DIR}\uninstall.ico" 0

  WriteIniStr "$INSTDIR\nsxtool.url" "InternetShortcut" "URL" "${WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\NSXTool\Website.lnk" "$INSTDIR\nsxtool.url" "" "${ICONS_DIR}\web.ico" 0
  
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "DisplayName" "NSXTool"
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "DisplayIcon" "${ICONS_DIR}\run.ico"
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "Publisher" "${PUBLISHER}"
  WriteRegStr ${UNINST_ROOT_KEY} "${UNINST_KEY}" "URLInfoAbout" "${WEB_SITE}"
  
  WriteUninstaller "$INSTDIR\uninst.exe"
  SetAutoClose false
SectionEnd

Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you really sure you want to uninstall NSXTool ?" IDYES +2
  Abort
FunctionEnd

Section uninstall
  SetShellVarContext all
  Delete "${ICONS_DIR}\run.ico"
  Delete "${ICONS_DIR}\uninstall.ico"
  Delete "${ICONS_DIR}\web.ico"
  
  Delete "$DESKTOP\NSXTool.lnk"

  Delete "$SMPROGRAMS\NSXTool\Uninstall.lnk"
  Delete "$SMPROGRAMS\NSXTool\NSXTool.lnk"
  Delete "$SMPROGRAMS\NSXTool\Website.lnk"
  
  RMDir /r "$SMPROGRAMS\NSXTool"
  RMDir /r "$INSTDIR"

  SetShellVarContext Current
  RMDir /r "$APPDATA\nsxtool"

  SetShellVarContext All
  RMDir /r "$APPDATA\nsxtool"

  DeleteRegKey ${UNINST_ROOT_KEY} "${UNINST_KEY}"
  SetAutoClose false
SectionEnd


