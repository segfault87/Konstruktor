#
# Regular cron jobs for the konstruktor package
#
0 4	* * *	root	[ -x /usr/bin/konstruktor_maintenance ] && /usr/bin/konstruktor_maintenance
