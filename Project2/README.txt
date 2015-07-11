Team BESTOS2014

Building the custom scheduler:

SSH into the MINIX machine.
Move schedproc.h into /usr/src/servers/sched/.
Move ~/pm/schedule.c into /usr/src/servers/pm/.  

Select a schedule.c program depending on whether 'static' or 'dynamic' 
scheduling needs to be performed.

Move schedule.c into /usr/src/servers/sched/.

cd into /usr/src/tools/.

execute 'make install'
	$  make install

execute 'shutdown'
	$  shutdown

execute 'boot d0p0'
	$  boot d0p0

Hit the number 2 to start MINIX using the custom scheduler.
