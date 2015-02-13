# Web Server

This is a simple implementation of a web server for the Networks course at the
University of Montana during the spring semester of 2015. 

The program begins by trying to open a socket for a given port. Upon success
currency is achived by creating a thread pool and accompanying job queue. Upon
any new request a job is created to represent the request and is enqueued to the
thread pool.

Each worker in the thread pool then iteratively dequeues requests and responds
to the client by parsing the request and opening the file and sending the
content of the requested file in blocks of 8k. When the worker finishes the
request, the job is destroyed and the iteration begins again servicing another
job.

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

* `tp_init() & tp_destroy()` 
    * Mange memory allocations/deallocations for struct
* `tp_wc_init() & tp_wc_destroy()` 
		* Manage memory for worker collection
* `tp_job_queue_init()` & `tp_job_queue_destroy()` 
		* Manage memrory for job queu

### Workers

Workers deal with processing raw requests. This is where all the string
manipulation comes into play.

* `void parse_request(char *buffer, char **method, char **uri);`
    * Takes a buffer and selectively fills the character string with the vaule
* `char* parse_mime(char *uri);`
		* Passes a uri and uses a a substring selection to find the filetype and returns the mime string
