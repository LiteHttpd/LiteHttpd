del /f /s /q %1\logs\*
rmdir /s /q %1\logs

del /f /s /q %1\*.iobj
del /f /s /q %1\*.ipdb
del /f /s /q %1\*.exp
del /f /s /q %1\*.ilk
del /f /s /q %1\*.lib
del /f /s /q %1\*.a