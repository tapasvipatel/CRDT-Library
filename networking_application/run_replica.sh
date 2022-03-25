#!/bin/bash

NAME=''
IPADDRESS=''
PORT=''

while [[ $# -gt 0 ]]; do
	key="$1"
	
	case $key in
		-name)
		NAME="$2"
		shift
		shift
		;;
	esac
	case $key in
		-ipaddress)
		IPADDRESS="$2"
		shift
		shift
		;;
	esac
	case $key in
		-port)
		PORT="$2"
		shift
		shift
		;;
	esac
done

cd ../build/networking_application
./replica $NAME $IPADDRESS $PORT
