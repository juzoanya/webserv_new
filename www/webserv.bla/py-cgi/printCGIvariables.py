#!/Users/maximilian/goinfre/.brew/bin/python3 python3

import os, cgitb, cgi

# cgitb.enable()
# cgi.test()

crlf = "\r\n"

print ("Content-type:text/html\r\n\r\n")

try:
    print("QUERY_STRING = " + os.environ["QUERY_STRING"])
    print(crlf)
    print("REQUEST_METHOD = " + os.environ["REQUEST_METHOD"])
    print(crlf)
    print("CONTENT_TYPE = " + os.environ["CONTENT_TYPE"])
    print("CONTENT_LENGTH = " + os.environ["CONTENT_LENGTH"])

    print("SCRIPT_FILENAME = " + os.environ["SCRIPT_FILENAME"])
    print("SCRIPT_NAME = " + os.environ["SCRIPT_NAME"])
    print("PATH_INFO = " + os.environ["PATH_INFO"])
    print("PATH_TRANSLATED = " + os.environ["PATH_TRANSLATED"])
    print("REQUEST_URI = " + os.environ["REQUEST_URI"])
    print("SERVER_PROTOCOL = " + os.environ["SERVER_PROTOCOL"])

    print("GATEWAY_INTERFACE = " + os.environ["GATEWAY_INTERFACE"])
    print("SERVER_SOFTWARE = " + os.environ["SERVER_SOFTWARE"])
    print("REMOTE_ADDR = " + os.environ["REMOTE_ADDR"])
    print("REMOTE_PORT = " + os.environ["REMOTE_PORT"])
    print("SERVER_ADDR = " + os.environ["SERVER_ADDR"])
    print("SERVER_PORT = " + os.environ["SERVER_PORT"])
    print("SERVER_NAME = " + os.environ["SERVER_NAME"])

    print("REDIRECT_STATUS = " + os.environ["REDIRECT_STATUS"])

except:
    print("Error occured")
