Midday Commander by Daniel Mcdonough
1/21/17

This program has 3 version v0,v1,v2 in their respected folders.
Each higher version has the same features as the previous versions.

To run the program open the terminal and type make in the respected version directory then ./mc# Where # is the version number.

v0:
Starting UI:
G'day, Commander! What command would you like to run?
   0. whoami : Prints out the result of the whoamicommand
   1. last   : Prints out the result of the last command
   2. ls     : Prints out the result of a listing on a user-specified path


The base v0, has 3 commands 
0: whoami
1: last
2: ls

Depending on the input given the program will run the given command and will tell the user the statistics of the fuction ran:

-- Statistics ---
Elapsed time: 7 milliseconds
Page Faults: 0
Page Faults (reclaimed): 0

the ls function will also give the user options to add arguments and choose a specific directory

v1:

Starting UI:
G'day, Commander! What command would you like to run?
   0. whoami : Prints out the result of the whoamicommand
   1. last   : Prints out the result of the last command
   2. ls     : Prints out the result of a listing on a user-specified path
   a. add command : Adds a new command to the menu
   c. change directory : Changes process working directory
   e. exit : Leave Mid-Day Commander
   p. pwd : Prints working directory
Option?:

v1 has 4 additional commands:
a: add command
c: change directory
e: exit
p: pwd

the a command allows the user to add a custom command to the program with any arguments the user requires and will give it a numerical number to correspond to the command

the c command is equivelent to the cd command

e, exits the program

p, lists the current directory

v2:

v2 has 1 addition command inwhich it is able to list any currently running background processes indecated by a custom addition with the & addition on the end of the command.

If there are any background processes you cannot exit the program by the e input.

UI:
G'day, Commander! What command would you like to run?
   0. whoami : Prints out the result of the whoamicommand
   1. last   : Prints out the result of the last command
   2. ls     : Prints out the result of a listing on a user-specified path
   a. add command : Adds a new command to the menu
   c. change directory : Changes process working directory
   e. exit : Leave Mid-Day Commander
   p. pwd : Prints working directory
   r. running processes : Print list of running processes
Option?:

Ex if I added a command make -j2 O=~/kernelDst &
and ran it would be a backround process






