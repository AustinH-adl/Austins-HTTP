#!/bin/bash

# Compile and start the server in the background
cd src
make
./server &
SERVER_PID=$!

echo "Server PID: $SERVER_PID"

PORT="8080"
URL="localhost:${PORT}"

numtest=0

function test_server {
    #Get 2 arguements
    local endpoint=$1
    local expected_result=$2

    #Send HTTP request and save respone
    response=$(curl -s $URL$endpoint)

    let "numtest += 1"

    #If actual response matches expected test passed
    if ["${response}" -eq "${result}"]; then
        echo "Test ${numtest} passed."
    #Otherwise test failed
    else
        echo "Test ${numtest} failed. Expected: $expected_result, Got: $response"
    fi
    unset $response  # Clear the response variable
}

#Shutdown the server
function cleanup {
    echo "Stopping the server..."
    kill $SERVER_PID
    wait $SERVER_PID 2>/dev/null
    echo "Server stopped."
}

#If script exits early always end the server session
trap cleanup EXIT

sleep 4 #Allow time for server to start

#tests to be executed 
tests=("/" "200"
       "/echo/HelloWorld" "200"
       "/user-agent" "200"
       "/not-found" "404")

for (( i=0; i<${#tests[@]} ; i+=2 )) ; do
    test_server ${tests[i]} ${tests[i+1]}
done

#Call cleanup function at end of the script
cleanup