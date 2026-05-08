CC = gcc
FLAGS = -Wall -pedantic -lpthread 

all: contador prod_cons leit_escr filosofos barbeiro ponte estacionamento impressora

contador:
	$(CC) 01-contador/contador.c -o 01-contador/contador $(FLAGS)

prod_cons:
	$(CC) 02-produtor-consumidor/prod_cons.c -o 02-produtor-consumidor/prod_cons $(FLAGS)

leit_escr:
	$(CC) 03-leitores-escritores/leit_escr.c -o 03-leitores-escritores/leit_escr $(FLAGS)

filosofos:
	$(CC) 04-filosofos/filosofos.c -o 04-filosofos/filosofos $(FLAGS)

barbeiro:
	$(CC) 05-barbeiro/barbeiro.c -o 05-barbeiro/barbeiro $(FLAGS)

ponte:
	$(CC) 06-ponte/ponte.c -o 06-ponte/ponte $(FLAGS)

estacionamento:
	$(CC) 07-estacionamento/estacionamento.c -o 07-estacionamento/estacionamento $(FLAGS)

impressora:
	$(CC) 08-impressora/impressora.c -o 08-impressora/impressora $(FLAGS)

clean:
	find . -type f -executable -not -name "*.c" -not -name "Makefile" -delete

run: all
	@echo "Running all simulations..." > log.txt
	@echo "" >> log.txt
	./01-contador/contador >> log.txt
	@echo "" >> log.txt
	./02-produtor-consumidor/prod_cons >> log.txt
	@echo "" >> log.txt
	./03-leitores-escritores/leit_escr >> log.txt
	@echo "" >> log.txt
	./04-filosofos/filosofos >> log.txt
	@echo "" >> log.txt
	./05-barbeiro/barbeiro >> log.txt
	@echo "" >> log.txt
	./07-estacionamento/estacionamento >> log.txt
	@echo "" >> log.txt
	./08-impressora/impressora >> log.txt
	@echo "" >> log.txt
	@echo "Done. Results saved to log.txt"