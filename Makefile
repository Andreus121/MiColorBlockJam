all: testBloque testSalida testCompuerta testTablero testMovimiento testParser

StaticData.o: StaticData.h StaticData.cpp
	g++ -c StaticData.cpp

Bloque.o: Bloque.h Bloque.cpp StaticData.o
	g++ -c Bloque.cpp
testBloque: testBloque.cpp Bloque.o
	g++ Bloque.o testBloque.cpp -o testBloque

Salida.o: Salida.h Salida.cpp StaticData.o
	g++ -c Salida.cpp
testSalida: testSalida.cpp Salida.o
	g++ Salida.o testSalida.cpp -o testSalida

Compuerta.o: Compuerta.h Compuerta.cpp StaticData.o
	g++ -c Compuerta.cpp
testCompuerta: testCompuerta.cpp Compuerta.o
	g++ Compuerta.o testCompuerta.cpp -o testCompuerta

Movimiento.o: Movimiento.h Movimiento.cpp
	g++ -c Movimiento.cpp
testMovimiento: testMovimiento.cpp Movimiento.o
	g++ Movimiento.o testMovimiento.cpp -o testMovimiento

Tablero.o: Tablero.h Tablero.cpp StaticData.o Salida.o Compuerta.o Bloque.o Movimiento.o
	g++ -c Tablero.cpp
testTablero: testTablero.cpp Tablero.o
	g++ Bloque.o Salida.o Compuerta.o StaticData.o Tablero.o Movimiento.o testTablero.cpp -o testTablero

Parser.o: Parser.h Parser.cpp StaticData.o Salida.o Compuerta.o Bloque.o Movimiento.o Tablero.o
	g++ -c Parser.cpp
testParser: testParser.cpp Parser.o StaticData.o Salida.o Compuerta.o Bloque.o Movimiento.o Tablero.o
	g++ Bloque.o Salida.o Compuerta.o StaticData.o Parser.o Movimiento.o Tablero.o testParser.cpp -o testParser

clean:
	rm -f *.o testBloque testSalida testCompuerta testTablero testMovimiento testParser