CC=g++
CFLAGS=-std=c++17 -Wall -g

# The source files we use for building custom_tests
ALL_SRC=main.cpp poly.cpp

# The name of the resulting executable
APP=test

custom_tests:
	$(CC) $(CFLAGS) $(ALL_SRC) -o $(APP) -pthread	

clean:
	rm -f $(APP)
