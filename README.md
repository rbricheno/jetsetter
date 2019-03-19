# jetsetter
Set keys in redis from stdin. Autogenerates keys based on a common root name. Splits stdin on space character.

First argument is the root of the key name. So to set keys like "this:value0", "this:value1" etc do:

stdout_writing_app | jetsetter this:value

To make it work:
* set up redis server
* make and install hiredis, and sudo ldconfig
* gcc jetsetter.c  -lhiredis -I /usr/local/include/hiredis -ojetsetter
