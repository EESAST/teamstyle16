CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra
OJBS = obj/ai.o obj/basic.o obj/connection.o obj/main.o
LIBFLAGS = -lboost_system
MKDIR = mkdir

bin/empty_ai: $(OJBS) | bin
	$(CXX) -o $@ $^ $(LIBFLAGS)

obj/ai.o: ai.cpp basic.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/basic.o: basic.cpp basic.h connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/connection.o: connection.cpp connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/main.o: main.cpp connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

connection.h: basic.h

bin:
	$(MKDIR) bin

obj:
	$(MKDIR) obj

clean:
	-rm obj/*.o
