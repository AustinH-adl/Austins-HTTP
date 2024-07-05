# Coding a HTTP server in C
Programming a basic HTTP server in C. Making use of cURL for testing purposes and sending requests to the server written in a bash script which can and will be modified with any required tests going forward. After cloning the repo to run the code first navigate to /src directory and compile code with the makefile by: 

```
cd src 
make
```

This will compile the as of now one file into server binary which can be ran with `./server`. 

## Testing
Testing was conducted with the `server_test.sh` file which can be ran by calling ```./server_test.sh``` in the parent directory `'HTTP server'`. This should run the tests and compare the HTTP response obtained to the expected result and output the ammount of tests passed as well as the server logs.

For manual testing first run the server with `./server` while it is compiled in the `/src` directory. Then send any HTTP request to the selected port defined in `HTTP.c`. 


## To-Do
1. Serve files
- Serving HTTP files so that a website can be accessed properly. 
2. Silent mode
- Allowing the program to be run in silent mode where no logs are output to the terminal.
3. Allowing peer to peer communication
- Obtaining POST requests from clients and verifying content then distributing to other clients. 
4. Final Chat app

## Environment 
Development was done with VScode 1.91.0 running on WSL ubuntu 22.04 compiling with gcc version 11.4.0.

> [!CAUTION]
> This server is not to be used for commercial uses. (Not that it has many uses)