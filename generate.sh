#!/bin/bash

TESTING=0
TRELLO_APPLICATION=0
TEST_APPLICATION=0
PERFORMANCE=0
CLEAN=0
NETWORKING=0
DATABASE_APPLICATION=0

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
	case $key in
		-clean)
		CLEAN=1
		shift
		;;
	esac
	case $key in
		-networking)
		NETWORKING=1
		shift
		;;
	esac
	case $key in
		-database_application)
		DATABASE_APPLICATION=1
		shift
		;;
	esac
done

if [ $TESTING -eq 1 ]; then
	cd tests
	rm -Rf temp_data
	mkdir temp_data
	cd ..
fi

if [ $CLEAN -eq 1 ]; then
	rm -Rf build
	mkdir build
fi

cd build

cmake_flags=''

if [ $TESTING -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_TESTING=1 -DLOCAL_TESTING=1'
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

if [ $CLEAN -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_CLEAN=1'
fi

if [ $NETWORKING -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_NETWORKING=1'
fi

if [ $DATABASE_APPLICATION -eq 1 ]; then
	cmake_flags=${cmake_flags}' -DBUILD_DATABASE_APPLICATION=1'
fi

cmake $cmake_flags ../ && make clean && make
