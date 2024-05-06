@echo off
setlocal enabledelayedexpansion

REM Définition de la date au format AAAA-MM-JJ
for /f "tokens=1-3 delims=/" %%a in ('date /t') do (
    set "year=%%c"
    set "month=%%a"
    set "day=%%b"
)
set "date=!year!-!month!-!day!"

cd "C:\Users\Administrateur.WIN-FHNERB40S8S\Documents\REM\log"

REM Compresse le fichier .log en .zip
compact "%date%.log.zip" "log.log"

REM Supprime le fichier .log
del "log.log"

endlocal
pause