CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra

OBJS_WITHOUT_MAIN = obj/ai.o obj/basic.o obj/connection.o
OBJS_WITHOUT_AI = obj/main.o obj/basic.o obj/connection.o
TEST_OBJS = obj/basic_test.o obj/connection_test.o

LIBS = -lboost_system

# gtest
GTEST_INCLUDE_DIR = /Users/lisihan969/src/gtest-1.7.0/include
GTEST_LIB_DIR = /Users/lisihan969/src/gtest-1.7.0/lib
GTEST_LIB = -lgtest -lgtest_main

MKDIR = mkdir

.PHONY: ai debugger test clean

ai: bin/empty_ai
debugger: bin/debugger
test: bin/test_all
	./$^

bin/empty_ai: $(OBJS_WITHOUT_MAIN) obj/main.o | bin
	$(CXX) -o $@ $^ $(LIBS)

bin/debugger: $(OBJS_WITHOUT_AI) obj/debugger.o | bin
	$(CXX) -o $@ $^ $(LIBS)

bin/test_all: $(TEST_OBJS) $(OBJS_WITHOUT_MAIN) | bin
	$(CXX) -o $@ $^ -L$(GTEST_LIB_DIR) $(GTEST_LIB) $(LIBS)

obj/ai.o: ai.cpp basic.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/debugger.o: debugger.cpp basic.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/basic.o: basic.cpp basic.h connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/basic_test.o: basic_test.cpp basic.h | obj
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE_DIR) -o $@ $<

obj/connection.o: connection.cpp connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/connection_test.o: connection_test.cpp connection.h | obj
	$(CXX) $(CXXFLAGS) -I$(GTEST_INCLUDE_DIR) -o $@ $<

obj/main.o: main.cpp connection.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

connection.h: basic.h
	touch connection.h

bin:
	$(MKDIR) bin

obj:
	$(MKDIR) obj

clean:
	-rm obj/*.o
