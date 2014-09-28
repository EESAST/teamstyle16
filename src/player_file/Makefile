CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra
OJBS = ai.o basic.o connection.o main.o
LIBFLAGS = -lboost_system

empty_ai: $(OJBS)
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

clean:
	-rm *.o
