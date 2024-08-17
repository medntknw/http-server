# HTTP Server
## Usage
- Clone the repository
- Change directory to repository `cd http-server`
- Run the HTTP Server
```bash
./runserver.sh
```
- Send a HTTP request
- Using curl
```bash
curl -v http://localhost:4221/
```
- Using browser
<img width="1462" alt="Screenshot 2024-08-17 at 10 47 42 PM" src="https://github.com/user-attachments/assets/480dcd03-aeee-4c35-bf6e-90ab469b330a">

## Overview
- As we know that HTTP is written over TCP (ignoring HTTP/3) we created an abstract class for a TCP server
- The TCPServer class has all the implementation of creating, binding, listenting socket and accepting requests.
- The derived classes will inherit from TCPServer and will have to implement the abstract method `processRequest`. Given each protocol on top of TCP has something different.
- The HTTPServer will inherit from TCPServer and implement its own logic for processing the request and sending the response.
- We can extend this to even implement a Layer 7 or Layer 4 Loadbalancer.
