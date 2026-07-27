CXX = g++
CXXFLAGS = -I. -g
DEPS = common.h writeDef.h readDef.h traceDef.h primDef.h
OBJ = main.o primDef.o readDef.o traceDef.o writeDef.o common.o

%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

raytracer: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

.PHONY: clean

clean:
	rm *.o
