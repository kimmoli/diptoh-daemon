diptoh daemon
=======

Daemon for diptoh otherhalf (by dirkvl http://funkyotherhalf.com/ )

EEPROM Programming instructions http://talk.maemo.org/showpost.php?p=1439339&postcount=73

Whole thread in TMO http://talk.maemo.org/showthread.php?t=93350

Instructions
=======

(Detach and attach diptoh once before installing daemon after programming EEPROM)

Installer starts service automatically if you did manage to write EEPROM contents and DIPTOH is attached while installing.

If something fails: ```systemctl stop harbour-diptoh-daemon``` or just rip the diptoh off.

If you want to see what it says, e.g. ```journalctl -f | grep diptoh```

Script files are for now in ```/home/nemo/diptoh/```

Currently every script just append ```$0``` to text file ```/home/nemo/diplog```

you can ```tail -f /home/nemo/diplog``` to see are the scripts executed when you toggle dip switch.

Turning dipswitch 1 on runs ```dip1on.sh``` and turning dipswitch 3 off runs ```dip3off.sh``` and so on.

Pressing the hidden button runs ```buttondown.sh``` and releasing it runs ```buttonup.sh```.

