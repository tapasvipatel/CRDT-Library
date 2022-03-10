#!/bin/bash

TESTING=0
TRELLO_APPLICATION=0
TEST_APPLICATION=0
PERFORMANCE=0

while [[ $# -gt 0 ]]; do
	key="$1"
	
	case $key in
		-testing)
		TESTING=1
		shift
		;;
	esac
	case $key in
		-trello_application)
		TRELLO_APPLICATION=1
		shift
		;;
	esac
	case $key in
		-test_application)
		TEST_APPLICATION=1
		shift
		;;
	esac
	case $key in
		-performance)
		PERFORMANCE=1
		shift
		;;
	esac
done

rm -Rf build
mkdir build
cd build

cmake_flags=''

if [ $TESTING -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_TESTING=1'
fi

if [ $TRELLO_APPLICATION -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_TRELLO_APPLICATION=1'
fi

if [ $TEST_APPLICATION -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_TEST_APPLICATION=1'
fi

if [ $PERFORMANCE -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_PERFORMANCE=1'
fi

cmake $cmake_flags ../ && make clean && make
