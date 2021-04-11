FLAGS='-std=c++14'

mylang2ir:		main.o fileio.o generator.o
				g++ ${FLAGS} fileio.o generator.o main.o -o mylang2ir
				@echo "Completed."

main.o:			main.cpp generator.h fileio.h
				g++ ${FLAGS} -c main.cpp

fileio.o:		fileio.cpp fileio.h
				g++ ${FLAGS} -c fileio.cpp

generator.o:	generator.cpp generator.h
				g++ ${FLAGS} -c generator.cpp
