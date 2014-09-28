CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra
OJBS = ai.o basic.o connection.o main.o
LIBFLAGS = -lboost_system
MKDIR = mkdir

bin/empty_ai: $(OJBS) | bin
	$(CXX) -o $@ $^ $(LIBFLAGS)

ai.o: ai.cpp basic.h
	$(CXX) $(CXXFLAGS) -o $@ $<

basic.o: basic.cpp basic.h connection.h
	$(CXX) $(CXXFLAGS) -o $@ $<

connection.o: connection.cpp connection.h
	$(CXX) $(CXXFLAGS) -o $@ $<

main.o: main.cpp connection.h
	$(CXX) $(CXXFLAGS) -o $@ $<

connection.h: basic.h

bin:
	$(MKDIR) bin

clean:
	-rm *.o
