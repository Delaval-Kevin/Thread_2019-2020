.SILENT:

PROGRAMS = Test1 Test2 Test2Bis Test2Tre

GPP = g++ -m64 -Wall -lpthread

all: $(PROGRAMS)

Test1: Exercice1.c
	echo Creation de Test1
	$(GPP) -o Test1  Exercice1.c

Test2: Exercice2.c
	echo Creation de Test2
	$(GPP) -o Test2  Exercice2.c
	
Test2Bis: Exercice2Bis.c
	echo Creation de Test2Bis
	$(GPP) -o Test2Bis  Exercice2Bis.c

Test2Tre: Exercice2Tre.c
	echo Creation de Test2Tre
	$(GPP) -o Test2Tre  Exercice2Tre.c
