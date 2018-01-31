Virus Scanner by Daniel McDOnough 1/28/18

This program was made to intercept system calls to sys_open,sys_close, sys_read in order to scan the files for the word "virus" and warn the user of any potential maliciuos code in the system logs.

How to run:
	In this directory:
		make
		sudo insmod cs3013_projet2.ko
		cd test
	In test directory
		make
		./testcalls
		tail -n # /var/log/syslog (where # in the number of lines you wish to view of the system log)


From here you can edit the files virus.txt or notvirus.txt to scan for the word virus in the file

