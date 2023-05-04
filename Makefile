default:
	g++ src/*.cpp -o project3.out -lsfml-graphics -lsfml-window -lsfml-system
	./project3.out

clean:
	rm -rf *.h *.out *.cpp *.txt
	rm -rf src/*.h src/*.out src/*.cpp src/*.txt