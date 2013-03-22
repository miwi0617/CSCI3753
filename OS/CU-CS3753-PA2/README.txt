README
multi-lookup 
Heather Dykstra
Due: 2/27/2013

NOTE:

The following is an edit of the README that was provided

---Folders---
input - names*.txt input files

---Executables---
lookup - A basic non-threaded DNS query-er
queueTest - Unit test program for queue
pthread-hello - A simple threaded "Hello World" program
multi-lookup - A treaded mutex DNS query-er

---Examples---
Build:
 make

Clean:
 make clean
 
---How you run things--- 

Lookup DNS info for all names files in input folder:
 ./lookup input/names*.txt results.txt

Check queue for memory leaks:
 valgrind ./queueTest

Run pthread-hello:
 ./pthread-hello

Run multi-lookup:
 ./multi-lookup input/names*.txt results.txt

Check multi-lookup for memory leaks:
 valgrind ./multi-lookup input/names*.txt results.txt
