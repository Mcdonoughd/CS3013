Process Ancestry by Daniel McDonough 2/2/18

This program producess the sibling, parent and children process ID's of a given process ID for all process found. In other words a large family process tree.

How to run:
	In this directory:
		make
		sudo insmod phase2.ko (you may need to resart your computer after this step)
		cd test
	In test directory
		make
		./proc_ancestry # (where # is a valid process ID)
		tail -n # /var/log/syslog (where # in the number of lines you wish to view of the system log)
	

To get a list of all process ID's, in the terminal you can use the command:
 ps axjf  
Where the PIDs are the second column on the left.

The result will look like:

Jan 31 06:22:19 CS-3013-VM kernel: [  212.417556] 10's sibling: 868!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417558] 10's sibling: 882!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417560] 10's sibling: 981!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417562] 10's sibling: 1047!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417564] 10's sibling: 1048!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417566] 10's sibling: 1049!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417568] 10's sibling: 1071!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417570] 10's sibling: 1072!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417572] 10's sibling: 1115!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417573] 10's sibling: 3364!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417575] 10's sibling: 0!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417577] 10's sibling: 3!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417579] 10's sibling: 4!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417580] 10's sibling: 5!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417582] 10's sibling: 6!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417584] 10's sibling: 7!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417586] 10's sibling: 8!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417588] 10's sibling: 9!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417589] A parent: 2!
Jan 31 06:22:19 CS-3013-VM kernel: [  212.417591] A parent: 0!
