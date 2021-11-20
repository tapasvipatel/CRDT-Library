#!/bin/bash

g++ server.cpp -o server -lboost_system -pthread
g++ client.cpp -o client -lboost_system -pthread
g++ async_server.cpp -o async_server -lboost_system -pthread
