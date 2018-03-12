To compile use make
To run use ./memsim
You can pipe in a text file with commmands seperated by lines
Supports make clean

Page Table data structure:
	There is an explicit defintion of this in the c file, but in reality this is not used it is just a representation to display it. Page tables take up 16 bytes(1 page) and are structured as such:

byte 0: VPN 0, byte 1: PFN 0, byte 2: PROTECTION BITS(for vpn 0), byte 3: VALID/PRESENT(for vpn 0) and this repeats 3 more times. Therefore page tables will generally look like the following form. Bytes will be seperated by lines and page table entries by spaces.

0|16|1|2 1|0|0|0 2|32|0|2 3|3|1|1

Therefore a page table entry lookes like this:

Page table Entry{
	unsigned char vpn;//in range 0-3
	unsigned char pfn;//in range 0,16,32,48 or if a file number 0,1,2...
	unsigned char protection;//if writeable = 1, otherwise 0
	unsigned char valpres;//if valid and present = 2, if valid but in disk = 1, if hasn't been allocated = 0
}


Eviction strategy:
	I use a simply round robin eviction technique to remove pages once the memory is full. This is denoted in the round robin int global counter, which gets incremented and mod%4 everytime something is removed. This number corresponds to the actual page frames of the 4 physical pages. Therefore it simply loops through and removes pages consecutively, however this is not true roundrobin as first a check is conducted to see if the page being removed is the page table for the current process it will increment round robin without removing anything. This prevents a page table from being removed and then needing to be immediately reimplemented to use. Furthermore if possible a page is removed over a page table this makes it easier to update values as page tables aren't always swapped out. This breaks once there are 4 page tables in memory then it uses standard round robin to remove one.


Test Cases:
Test1 was submitted with my checkpoint therefore it simply tests whether the protection bytes work and shows off the segfault error that is thrown.

Test2 displays the ability to swap pages in and out of memory, this includes retrieving a value from a page that is not currently in memory. This test is quite small as swapping prints a bunch of stuff to the console so to keep it visible what was happening it is only a few commands.

Test3 is an amalgamation of everything else and tests pretty much everything about this simulation.
