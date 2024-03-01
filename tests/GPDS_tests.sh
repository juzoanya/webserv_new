#!/bin/bash

# Define sleep duration
SLEEP_DURATION=5

# Server address, endpoint paths, and file paths
SERVER_ADDRESS="http://example.net:8080"
UPLOAD_ENDPOINT="/uploads"
UNKNOWN_ENDPOINT="/unknown_endpoint"
UNKNOWN_METHOD_FILE="/fileUpload.html"
FILE_PATH="/Users/constantin/Desktop"
FILE_NAME="Caltech_AI_Updated_121223.pdf"

# ANSI color codes
yellow='\033[1;33m'
green='\033[1;32m'
red='\033[1;31m'
reset='\033[0m'

# Function to extract availability percentage from Siege output
get_availability_percentage() {
    echo "$1" | sed -n 's/Availability:[[:space:]]*\([0-9.]*\) %.*/\1/p'
}

# Decorative elements
decorate_message() {
    echo "******************************************************************************"
    echo -e "* ${yellow}$1${reset}"
    echo "******************************************************************************"
}

# Function to execute HTTP request with error handling
execute_request() {
    local command="$1"
    local success_message="$2"
    local failure_message="$3"

    if eval "$command"; then
        echo -e "${red}$failure_message${reset}"
    else
        echo -e "${green}$success_message${reset}"
    fi
}

# GET METHOD
decorate_message "Testing GET Method..."
if curl -sSf "${SERVER_ADDRESS}/fileUpload.html"; then
    decorate_message "${green}GET Method Test Successful${reset}"
else
    decorate_message "${red}GET Method Test Failed${reset}"
fi

# Add waiting time after GET method
sleep $SLEEP_DURATION

# POST METHOD
decorate_message "Testing POST Method..."
if curl -sSf -X POST -F "file=@${FILE_PATH}/${FILE_NAME}" "${SERVER_ADDRESS}${UPLOAD_ENDPOINT}"; then
    decorate_message "${green}POST Method Test Successful${reset}"
else
    decorate_message "${red}POST Method Test Failed${reset}"
fi

# Add waiting time after POST method
sleep $SLEEP_DURATION

# DELETE METHOD
decorate_message "Testing DELETE Method..."
if curl -sSf -X DELETE "${SERVER_ADDRESS}${UPLOAD_ENDPOINT}/${FILE_NAME}"; then
    decorate_message "${green}DELETE Method Test Successful${reset}"
else
    decorate_message "${red}DELETE Method Test Failed${reset}"
fi

# Add waiting time after DELETE method
sleep $SLEEP_DURATION

# CHUNKED UPLOAD METHOD
decorate_message "Testing Chunked Upload Method..."
if curl -v -H "Transfer-Encoding: chunked" -F "file=@${FILE_PATH}/${FILE_NAME}" "${SERVER_ADDRESS}${UPLOAD_ENDPOINT}"; then
    decorate_message "${green}Chunked Upload Method Test Successful${reset}"
else
    decorate_message "${red}Chunked Upload Method Test Failed${reset}"
fi

# Add waiting time after chunked upload method
sleep $SLEEP_DURATION

# UNKNOWN REQUEST TEST
decorate_message "Testing Unknown Request..."
execute_request "curl -sSf ${SERVER_ADDRESS}${UNKNOWN_ENDPOINT}" "Unknown Request Test Successful" "Unknown Request Test Failed"

# Add waiting time after unknown request test
sleep $SLEEP_DURATION

# UNKNOWN METHOD TEST
decorate_message "Testing Unknown Method..."
execute_request "curl -sSf -X UNKNOWN ${SERVER_ADDRESS}${UNKNOWN_METHOD_FILE}" "Unknown Method Test Successful" "Unknown Method Test Failed"

# Add waiting time after unknown method test
sleep $SLEEP_DURATION

# METHOD NOT IMPLEMENTED TEST
decorate_message "Testing Method Not Implemented (PUT)..."
execute_request "curl -sSf -X PUT ${SERVER_ADDRESS}${UNKNOWN_METHOD_FILE}" "Method Not Implemented (PUT) Test Successful" "Method Not Implemented (PUT) Test Failed"

# Add waiting time after method not implemented test
sleep $SLEEP_DURATION

# SIEGE BENCHMARK (SILENT MODE)
decorate_message "Running Siege Benchmark (silent mode)..."
siege_output=$(siege -b -c 100 -t 1M "${SERVER_ADDRESS}/fileUpload.html" 2>&1)

# Check if siege command executed successfully
if [ $? -eq 0 ]; then
    echo -e "\n$siege_output"
    availability=$(get_availability_percentage "$siege_output")

    # Compare availability percentage with threshold
    if (( $(bc <<< "$availability >= 99.95") )); then
        decorate_message "${green}Siege Benchmark Successful (Availability: $availability %)${reset}"
    else
        decorate_message "${red}Siege Benchmark Failed (Availability < 99.95%)${reset}"
    fi
else
    decorate_message "${red}Failed to run Siege Benchmark${reset}"
fi
