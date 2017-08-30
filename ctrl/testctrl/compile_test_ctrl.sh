#!/bin/bash

gcc -o test_send.o test_send.c -lrt -lpthread
gcc -o test_rc.o test_rc.c -lrt -lpthread
gcc -o control.o control.c -lrt -lpthread
