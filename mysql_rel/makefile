GPP=g++
GCC=gcc

OUTFILE="../mysql.so"

COMPILE_FLAGS=-c -O3 -w -DLINUX -Wall -I../SDK/amx/

all:
	$(GCC) $(COMPILE_FLAGS) ../SDK/amx/*.c
	$(GPP) $(COMPILE_FLAGS) ../SDK/*.cpp
	$(GPP) $(COMPILE_FLAGS) main.cpp
	$(GPP) $(COMPILE_FLAGS) source/*.cpp
	$(GPP) -O2 -fshort-wchar -shared -o $(OUTFILE) *.o -L/usr/lib/mysql -lmysqlclient_r -lpthread
	rm -f *.o