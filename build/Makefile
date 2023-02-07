INCLUDE_PATH = ../include/
INCLUDE_PATH_CHUNKING = ../include/chunking/
INCLUDE_PATH_HASHING = ../include/hashing/

SRC_PATH = ../src/
SRC_PATH_CHUNKING = ../src/chunking/
SRC_PATH_HASHING = ../src/hashing/

COMPILER_FLAGS= -Wall

INCLUDE_FLAGS = -I $(INCLUDE_PATH)
INCLUDE_FLAGS += -I ${INCLUDE_PATH_CHUNKING}
INCLUDE_FLAGS += -I ${INCLUDE_PATH_HASHING}

CC = g++
RM = rm -f

DEBUG = 1

ifeq ($(DEBUG), 1)
	$(COMPILER_FLAGS) += -g
endif

all: dedup.exe

dedup.exe: driver.o fixed_chunking.o chunking_common.o hashing_common.o std_hashing.o
	$(CC) -o dedup.exe driver.o fixed_chunking.o chunking_common.o hashing_common.o std_hashing.o $(INCLUDE_FLAGS) $(COMPILER_FLAGS)

driver.o: ${SRC_PATH}/driver.cpp
	$(CC) -c $(SRC_PATH)/driver.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o driver.o


# Hashing related build rules

hashing_common.o: ${SRC_PATH_HASHING}/hashing_common.cpp
	$(CC) -c $(SRC_PATH_HASHING)/hashing_common.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o hashing_common.o

std_hashing.o: ${SRC_PATH_HASHING}/std_hashing.cpp
	$(CC) -c $(SRC_PATH_HASHING)/std_hashing.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o std_hashing.o


# Chunking related build rules

chunking_common.o: $(SRC_PATH_CHUNKING)/chunking_common.cpp 
	$(CC) -c $(SRC_PATH_CHUNKING)/chunking_common.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o chunking_common.o

fixed_chunking.o: $(SRC_PATH_CHUNKING)/fixed_chunking.cpp
	$(CC) -c $(SRC_PATH_CHUNKING)/fixed_chunking.cpp $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -o fixed_chunking.o

clean:
	$(RM) *.o *.d *.exe
