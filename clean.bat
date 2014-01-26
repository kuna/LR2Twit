del /s /q .\ipch
del /s /q .\Debug\*
del /s /q .\Release\*.obj
del /s /q .\Release\*.tlog
del /s /q .\Release\*.log
del /s /q .\Release\*.lastbuildstate
del /s /q .\Release\*.unsuccessfulbuild
del /s /q .\Release\*.pdb
del /s /q .\Release\*.idb
del /s /q .\Release\*.pch
del /s /q .\*.sdf
rmdir /s /q .\ipch

del /s /q .\twitterClient_token*
del /s /q .\Release\twitterClient_token*
del /s /q .\Release\log.txt
del /s /q .\log.txt

copy /y .\Release\LR2Twit.exe .\redistribute\LR2Twit.exe