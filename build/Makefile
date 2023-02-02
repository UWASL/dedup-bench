INCLUDE_PATH = ../include/
SRC_PATH = ../src/

COMPILER_FLAGS= -Wall
INCLUDE_FLAGS = -I $(INCLUDE_PATH)
CC = g++
RM = rm -f

DEBUG = 0

ifeq ($(DEBUG), 1)
	$(COMPILER_FLAGS) += -g
endif

all: dedup

dedup: driver.o fixed_chunking.o chunking_common.o
	$(CC) -o dedup.exe driver.o fixed_chunking.o chunking_common.o $(INCLUDE_FLAGS) $(COMPILER_FLAGS)

driver.o: fixed_chunking.o chunking_common.o
	$(CC) -c $(SRC_PATH)/driver.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o driver.o

chunking_common.o: $(SRC_PATH)/chunking_common.cpp 
	$(CC) -c $(SRC_PATH)/chunking_common.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o chunking_common.o

fixed_chunking.o: $(SRC_PATH)/fixed_chunking.cpp
	$(CC) -c $(SRC_PATH)/fixed_chunking.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o fixed_chunking.o

clean:
	$(RM) *.o *.d *.exe
