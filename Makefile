CCFLAGS = -O2

all: prim kruskal

prim: prim.o
		g++ $(CCFLAGS) -o prim prim.o

kruskal: kruskal.o
		g++ $(CCFLAGS) -o kruskal kruskal.o

prim.o: prim.cpp
		g++ $(CCFLAGS) -c prim.cpp

kruskal.o: kruskal.cpp
		g++ $(CCFLAGS) -c kruskal.cpp

clean:
		rm -rf *.o *~ *.x