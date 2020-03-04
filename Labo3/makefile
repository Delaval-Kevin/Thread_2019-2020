.SILENT:

ECRAN = $(HOME)/Ecran

OBJ = Ecran.o

PROGRAMS = Test1 Test2

GPP = g++ -m64 -Wall -lpthread -I $(ECRAN)

all: $(PROGRAMS)

Test1: Exercice1.c $(OBJ)
	echo Creation de Test1
	$(GPP) -o Test1 $(OBJ) Exercice1.c

Test2: Exercice2.c $(OBJ)
	echo Creation de Test2
	$(GPP) -o Test2 $(OBJ) Exercice2.c
	
Ecran.o: $(ECRAN)/Ecran.cpp
	echo Creation de Ecran.o
	g++ -m64 -D SUN -Wall -I $(ECRAN) -c $(ECRAN)/Ecran.cpp
