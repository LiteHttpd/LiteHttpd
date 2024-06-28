$LOG_PATH = "$($args[0])/logs"
if ((Test-Path $LOG_PATH) -eq "True") {
	Remove-Item $LOG_PATH -Recurse
}

Remove-Item "$($args[0])/*" -Include "*.iobj"
Remove-Item "$($args[0])/*" -Include "*.ipdb"
Remove-Item "$($args[0])/*" -Include "*.exp"
Remove-Item "$($args[0])/*" -Include "*.ilk"
Remove-Item "$($args[0])/*" -Include "*.lib"
Remove-Item "$($args[0])/*" -Include "*.a"