CC = gcc
CFLAGS += -Wall -Wextra

CXX = g++
CXXFLAGS += $(CFLAGS)

AR = ar
ARFLAGS = rcs

RM = rm
RMFLAGS = -f

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
.cpp.o:
	$(CXX) $(CXXFLAGS) -I. -I./sqlite -I./test/tut -c $< -o $@

SQLITE_OBJS = $(patsubst %.c, %.o, sqlite/sqlite3.c)
SQLITEPP_OBJS = $(patsubst %.cpp, %.o, $(wildcard sqlitepp/*.cpp))
TEST_OBJS = $(patsubst %.cpp, %.o, $(wildcard test/*.cpp))  

all: sqlite sqlitepp sqlitepp_test

sqlite: $(SQLITE_OBJS)
	$(AR) $(ARFLAGS) libsqlite.a $(SQLITE_OBJS)

sqlitepp: $(SQLITEPP_OBJS) sqlite
	$(AR) $(ARFLAGS) libsqlitepp.a $(SQLITEPP_OBJS)

sqlitepp_test: $(TEST_OBJS) sqlitepp
	$(CXX) $(TEST_OBJS) -o $@ -L. -lsqlite -lsqlitepp -lpthread -ldl 
 
clean:
	$(RM) $(RMFLAGS) $(SQLITE_OBJS) $(SQLITEPP_OBJS) $(TEST_OBJS) libsqlite.a libsqlitepp.a sqlitepp_test

