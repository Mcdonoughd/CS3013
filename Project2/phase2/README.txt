Process Ancestry by Daniel McDonough 2/2/18

This program producess the sibling, parent and children process ID's of a given process ID for all process found. In other words a large family process tree.

How to run:
	In this directory:
		make
		sudo insmod phase2.ko (you may need to resart your computer after this step)
		cd test
	In test directory
		make
		./testcalls
		tail -n # /var/log/syslog (where # in the number of lines you wish to view of the system log)
	

To get a list of all process ID's, in the terminal you can use the command:
 ps axjf  

Where the PIDs are the second column on the left.

