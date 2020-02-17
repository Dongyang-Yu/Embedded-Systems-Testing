SET DIRECTORY_NAME="Folder To Delete"
TAKEOWN /f %DIRECTORY_NAME% /r /d y
ICACLS %DIRECTORY_NAME% /grant administrators:F /t
PAUSE