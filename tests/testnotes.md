# Webserv Tests

### Static File Tests

### CGI Test
1. Upload File
2. 

### Memory Leak Test

### Availability Test


### Check the Code and ask Questions
1. explain basics of an HTTP Server ✅
2. Explain what function we used for I/O Multiplexing (select / poll / epoll /  kqueue) and explain how it works ❓
3. show, that we uses only one poll for all I/O and explain, how we archieved it to accept connections and read / write to client ❓
4. poll should be in the main loop and should check read and write availability of an File Descriptor AT THE SAME TIME ✅
5. Only one read per client per poll / write per client per poll. Show the code of the poll to the read / write of client ❓
6. show for all read / write, if an error occured, the client will be removed ✅
7. show for all read / write, return value for -1 AND 0 is checked (-1 or 0 only is not enough ) ✅
8. show for all read / write, checking ERRNO is FORBIDDEN! ✅
9. show for all read / write, if write / read without poll is FORBIDDEN! ✅
10. NO RELINKING in makefile ✅
11. show that any point is clear and correct

### Configuration
1. (check different Situations and make sure, every returned STATUSCODE is CORRECT!)
2. multiple servers with different ports
3. multiple servers with different hostnames
4. default error page (e.g. 404)
5. limit client body size
6. show routes in a server to different directories
7. show default file, if the path points to a directory
8. show accepted methods (delete something, if the method is not accepted in this route)

### basic checks, using telnet, curl, prepared files (tester files?)
1. GET, POST, DELETE requests should work ✅
2. unknown requests should not result in crash ❓
3. every request should result in the correct statuscode! (Configuration 1.) ❓
4. upload file to server and get it back ✅

### check CGI
1. server is fine using cgi ✅
2. CGI should be run in the correct directory for relative path file access! ❓
3. test cgi with GET and POST methods - show everything works properly ❓
4. test wit files, containing errors -> cgi script with infinite loop of error !! handle this ❓
4. server should never crash and error should be visible in case of problem!

### check with browser
1. use chrome, open network tab, connect to server
2. check the request and response headers in network tab
3. show server can serve full static webpage
4. try wrong URL on the server
5. try directory listing
6. try redirection
7. try any other

### port issues
1. multiple ports in config file with different websites, show, that the server serves the correct websites and files, based on these config
2. try to set up the same port multiple times, IT SHOULD NOT WORK!
3. lanuch multiple servers, different configurations but same ports! Explain, why the server should work if one of the configurations is not functional

### siege and stress test
1. use Siege (-b flag) to run stress test, Availability should be above 99.5% for GET on empty page 
2. verify there is NO Memory Leak - monitor process memory usage - should not go up indefinitely
3. verify there is no hanging connections
4. should be able to use siege indefinitely without having to restart the server (check siege -b man page)

#### bonus
1. session and cookie system
2. multiple cgi systems

