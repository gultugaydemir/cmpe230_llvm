FLAGS='-std=c++14'

mylang2ir:		main.o fileio.o generator.o
				g++ ${FLAGS} fileio.o generator.o main.o -o mylang2ir
				@echo "Completed."

main.o:			src/main.cpp include/generator.h include/fileio.h
				g++ ${FLAGS} -c src/main.cpp

fileio.o:		src/fileio.cpp include/fileio.h
				g++ ${FLAGS} -c src/fileio.cpp

generator.o:	src/generator.cpp include/generator.h
				g++ ${FLAGS} -c src/generator.cpp
