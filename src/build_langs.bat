IF "%1"=="" EXIT 1
IF NOT EXIST "%1\langs" md "%1\langs"
IF NOT EXIST "%1\langs\ru" md "%1\langs\ru"
IF NOT EXIST "%1\langs\en" md "%1\langs\en"
IF NOT EXIST "%1\langs\nn" md "%1\langs\nn"
IF NOT EXIST "%1\langs\de" md "%1\langs\de"
IF NOT EXIST "%1\langs\es" md "%1\langs\es"
IF NOT EXIST "%1\langs\it" md "%1\langs\it"
IF NOT EXIST "%1\langs\zh" md "%1\langs\zh"
IF NOT EXIST "%1\langs\fi" md "%1\langs\fi"
IF NOT EXIST "%1\langs\pl" md "%1\langs\pl"
IF NOT EXIST "%1\langs\fr" md "%1\langs\fr"
set THIS_PATH=%~dp0
set MSGFMT_EXE=%THIS_PATH%external_files\msgfmt.exe
"%MSGFMT_EXE%" -D langs -o "%1\langs\ru\dng4ps-2.mo" ru.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\en\dng4ps-2.mo" en.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\nn\dng4ps-2.mo" nn.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\de\dng4ps-2.mo" de.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\es\dng4ps-2.mo" es.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\it\dng4ps-2.mo" it.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\zh\dng4ps-2.mo" zh.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\fi\dng4ps-2.mo" fi.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\pl\dng4ps-2.mo" pl.po
"%MSGFMT_EXE%" -D langs -o "%1\langs\fr\dng4ps-2.mo" fr.po
