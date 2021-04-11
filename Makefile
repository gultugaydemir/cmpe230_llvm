FLAGS='-std=c++14'

mylang2ir:	main.o
			g++ ${FLAGS} main.o -o mylang2ir
			@echo "Completed."

main.o:		main.cpp
			g++ ${FLAGS} -c main.cpp
