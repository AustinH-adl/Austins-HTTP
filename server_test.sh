#!/bin/bash

# Start the server in the background
./src/server &
SERVER_PID=$!

echo "Server PID: $!"

# Give the server some time to start
sleep 2

# Define the URL to test
URL="http://localhost:8080"

# Function to test the server response
test_server() {
    local path=$1
    local expected_response=$2

    # Send the request using curl and capture the response
    response=$(curl -s "$URL$path")

    # Check if the response matches the expected response
    if [ "$response" == "$expected_response" ]; then
        echo "Test passed: $path"
    else
        echo "Test failed: $path"
        echo "Expected: '$expected_response'"
        echo "Got: '$response'"
        return 1
    fi
}

# Define tests
tests=(
    "" "HTTP/1.1 200 OK\r\n\r\n"
    "/nonexistent" "HTTP/1.1 404 Not Found\r\n\r\n"
    "/echo/test" "test"
    "/user-agent" "curl/7.68.0"
)

# Run tests
for ((i = 0; i < ${#tests[@]}; i += 2)); do
    test_server "${tests[i]}" "${tests[i+1]}"
done

# Capture the result of the tests
TEST_RESULT=$?

# Kill the server process
kill $!

# Exit with the test result status
exit $TEST_RESULT