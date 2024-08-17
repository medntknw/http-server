#!/bin/sh
set -e
(
  cd "$(dirname "$0")"
  echo $PWD
  g++ -o server src/http_server.cpp src/server.cpp src/http_server.h src/tcp_server.cpp src/tcp_server.h
)
exec ./server
