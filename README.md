# Web Server

This is a simple implementation of a web server for the Networks course at the
University of Montana during the spring semester of 2015. 

## Build

A consolidated Makefile is provided to compile the project. All of the source
files are defined within the `src` directory and the make process will produce
an executable binary within the `bin` directory.

## Running

Since the webserver serves files from the directory it is run in a simple bash
script is included to change directory to the test `html` directory. It then
invokes the compiled executable in this directory. You can pass a target port to
the script which is then passed to the binary as well

`./start.sh <port_number | 8080>`

## Functions

### Threading

A simple struct and supporting functions are defined to construct and manage
requests into a job queue as well as start and manage threads in a thread pool.

* `tp_init() & tp_destroy() // Mange memory allocations/deallocations for struct`
* `tp_wc_init() & tp_wc_destroy() // Manage memory for worker collection`
* `tp_job_queue_init() & tp_job_queue_destroy() // Manage memrory for job queu`
* 
