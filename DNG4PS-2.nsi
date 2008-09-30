!define PROG_NAME dng4ps2

!include "MUI.nsh"
!include "LogicLib.nsh"

SetCompressor /SOLID lzma
SetCompressorDictSize 32

LicenseBkColor /windows

XPStyle on

; The name of the installer
Name "${PROG_NAME}"

; The file to write
OutFile "setup\${PROG_NAME}-${PROG_VERS}.exe"

; The default installation directory
InstallDir $PROGRAMFILES\${PROG_NAME}

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\${PROG_NAME}" "Install_Dir"

!define MUI_ABORTWARNING

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"

BrandingText 'DNG for PowerShot-2'

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE src\license.txt
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES


!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Spanish"

!insertmacro MUI_RESERVEFILE_LANGDLL

Function .onInit
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

;--------------------------------

; The stuff to install
Section "${PROG_NAME}"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r "src\bin\win\Release\langs"
  File "src\bin\win\Release\dng4ps-2.exe"
  File "src\changes_en.txt"
  File "src\changes_ru.txt"
  File "src\license.txt"
  File "src\readme_en.txt"
  File "src\readme_rus.txt"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\${PROG_NAME} "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" "DisplayName" "${PROG_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  ${Switch} $LANGUAGE
    ${Case} 1049 ; Russian
      WriteRegDWORD HKCU "Software\${PROG_NAME}" "Language" 156
      ${Break}
      
    ${Case} 1034 ; Spanish
      WriteRegDWORD HKCU "Software\${PROG_NAME}" "Language" 175
      ${Break}
      
    ${Case} 1031 ; German
      WriteRegDWORD HKCU "Software\${PROG_NAME}" "Language" 87
      ${Break}
      
    ${Case} 1044 ; Norwegian.nlf
      WriteRegDWORD HKCU "Software\${PROG_NAME}" "Language" 144
      ${Break}

    ${Default} ; English
      WriteRegDWORD HKCU "Software\${PROG_NAME}" "Language" 56
      ${Break}
    
   ${EndSwitch}


  CreateDirectory "$SMPROGRAMS\${PROG_NAME}"
  CreateShortCut "$SMPROGRAMS\${PROG_NAME}\DNG for PowerShot-2.lnk" "$INSTDIR\DNG4PS-2.EXE" "" "$INSTDIR\DNG4PS-2.EXE" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PROG_NAME}"
  DeleteRegKey HKLM SOFTWARE\${PROG_NAME}

  ; Remove files and uninstaller
  RMDir /r "$INSTDIR"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\${PROG_NAME}\*.*"

SectionEnd
