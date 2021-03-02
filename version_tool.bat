cd /D "%~dp0"
del .\version.h
echo|set /p="#define REPO_VERSION_HASH " >> .\version.h
for /f %%i in ('git rev-parse --short HEAD') do set HASH=%%i
echo ^"%HASH%^" >> .\version.h
echo|set /p="#define FW_VERSION_TAG ">> .\version.h
for /f %%j in ('git describe --abbrev^=0 --tags') do set TAG=%%j
echo ^"%TAG%^" >> .\version.h
