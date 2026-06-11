@echo off
setlocal

:: ---- Configuración -------------------------------------------------------
set GAME=elemental_dungeon
set BUILD=build
set CC65_HOME=C:\cc65
set PATH=%CC65_HOME%\bin;%PATH%
set NES_LIB=%CC65_HOME%\lib\nes.lib

set INC=-I lib -I src/core -I src/world -I src/graphics -I src/entities
set CFLAGS=-t nes -O --add-source %INC%
set ASFLAGS=-t nes -I lib

:: ---- Limpiar si se pasa "clean" ------------------------------------------
if /I "%1"=="clean" (
    del /Q %BUILD%\*.o %BUILD%\*.s %BUILD%\*.nes 2>nul
    echo Limpiado.
    goto :eof
)

:: ---- Crear directorio build si no existe ---------------------------------
if not exist %BUILD% mkdir %BUILD%

:: ---- Compilar fuentes C --------------------------------------------------
echo Compilando fuentes C...

call :compile_c src\core\main.c       main       || goto :error
call :compile_c src\core\game.c       game       || goto :error
call :compile_c src\core\text.c       text       || goto :error
call :compile_c src\core\sound.c      sound      || goto :error
call :compile_c src\world\map.c       map        || goto :error
call :compile_c src\world\score.c     score      || goto :error
call :compile_c src\entities\player.c player     || goto :error
call :compile_c src\entities\ghost.c  ghost      || goto :error
call :compile_c src\entities\obstacles.c obstacles || goto :error

:: ---- Ensamblar fuentes ASM -----------------------------------------------
echo Ensamblando fuentes ASM...

call :assemble_s src\core\crt0.s      crt0      || goto :error
call :assemble_s src\graphics\tiles.s tiles     || goto :error
call :assemble_s lib\neslib.s         neslib    || goto :error

:: ---- Enlazar ROM ---------------------------------------------------------
echo Enlazando...

ld65 -C nes.cfg -o %BUILD%\%GAME%.nes ^
    %BUILD%\crt0.o ^
    %BUILD%\tiles.o ^
    %BUILD%\neslib.o ^
    %BUILD%\main.o ^
    %BUILD%\game.o ^
    %BUILD%\text.o ^
    %BUILD%\sound.o ^
    %BUILD%\map.o ^
    %BUILD%\score.o ^
    %BUILD%\player.o ^
    %BUILD%\ghost.o ^
    %BUILD%\obstacles.o ^
    %NES_LIB%

if errorlevel 1 goto :error

del /Q %BUILD%\*.o %BUILD%\*.s 2>nul

echo.
echo OK: %BUILD%\%GAME%.nes
goto :eof

:: ---- Subrutinas ----------------------------------------------------------

:compile_c
:: %1 = ruta fuente .c   %2 = nombre base
cc65 %CFLAGS% -o %BUILD%\%2.s %1
if errorlevel 1 exit /b 1
ca65 %ASFLAGS% -o %BUILD%\%2.o %BUILD%\%2.s
if errorlevel 1 exit /b 1
exit /b 0

:assemble_s
:: %1 = ruta fuente .s   %2 = nombre base
ca65 %ASFLAGS% -o %BUILD%\%2.o %1
if errorlevel 1 exit /b 1
exit /b 0

:error
echo.
echo ERROR: build fallido.
exit /b 1
