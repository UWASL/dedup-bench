# =========================
# Can consider using -MMD flag in the compiler to auto-gen dependencies file
# if the number of headers ever grow too big
# =========================
MAKEFILE = Makefile
EXEC_NAME = dedup
BUILD_DIR_PATH = ../../build


INCLUDE_PATH = ../include/
INCLUDE_PATH_CHUNKING = ../include/chunking
INCLUDE_PATH_HASHING = ../include/hashing
INCLUDE_PATH_CONFIG = ../include/config
INCLUDE_PATH_OPENSSL = /usr/local/opt/openssl@3/include

INCLUDE_FLAGS = -I $(INCLUDE_PATH)
INCLUDE_FLAGS += -I ${INCLUDE_PATH_CHUNKING}
INCLUDE_FLAGS += -I ${INCLUDE_PATH_HASHING}
INCLUDE_FLAGS += -I ${INCLUDE_PATH_CONFIG}
INCLUDE_FLAGS += -I ${INCLUDE_PATH_OPENSSL}

LD_FLAGS = -L /usr/local/opt/openssl@3/lib -lcrypto

SRC_PATH = ../src
SRC_MAIN = $(wildcard $(SRC_PATH)/*.cpp)
OBJS_MAIN = $(SRC_MAIN:$(SRC_PATH)/%.cpp=%.o)

SRC_PATH_CHUNKING = ../src/chunking
SRC_CHUNKING = $(wildcard $(SRC_PATH_CHUNKING)/*.cpp)
OBJS_CHUNKING = $(SRC_CHUNKING:$(SRC_PATH_CHUNKING)/%.cpp=%.o)
DEPS_CHUNKING = $(wildcard $(INCLUDE_PATH_CHUNKING)/*.hpp)

SRC_PATH_HASHING = ../src/hashing
SRC_HASHING = $(wildcard $(SRC_PATH_HASHING)/*.cpp)
OBJS_HASHING = $(SRC_HASHING:$(SRC_PATH_HASHING)/%.cpp=%.o)
DEPS_HASHING = $(wildcard $(INCLUDE_PATH_HASHING)/*.hpp)

SRC_PATH_CONFIG = ../src/config
SRC_CONFIG = $(wildcard $(SRC_PATH_CONFIG)/*.cpp)
OBJS_CONFIG = $(SRC_CONFIG:$(SRC_PATH_CONFIG)/%.cpp=%.o)
DEPS_CONFIG = $(wildcard $(INCLUDE_PATH_CONFIG)/*.hpp)

COMPILER_FLAGS= -std=c++17 -Wall -Wextra -Wno-format

CC = g++
RM = rm -f

DEBUG = 1

ifeq ($(DEBUG), 1)
	$(COMPILER_FLAGS) += -g
endif


default: $(EXEC_NAME)
all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJS_MAIN) $(OBJS_CHUNKING) $(OBJS_HASHING) $(OBJS_CONFIG) 
	@echo ""
	@echo "Linking $@ ....."
	@echo "============================"
	$(CC) -o $(EXEC_NAME).exe $(OBJS_MAIN) $(OBJS_CHUNKING) $(OBJS_HASHING) $(OBJS_CONFIG) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) $(LD_FLAGS)
	cp $(EXEC_NAME).exe $(BUILD_DIR_PATH)/$(EXEC_NAME).exe
	@echo ""

# Driver related build rules

$(OBJS_MAIN): %.o: $(SRC_PATH)/%.cpp $(MAKEFILE)
	$(CC) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -c $< -o $@

# Hashing related build rules

$(OBJS_HASHING): %.o: $(SRC_PATH_HASHING)/%.cpp $(DEPS_HASHING) $(MAKEFILE)
	$(CC) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -c $< -o $@

# Chunking related build rules

$(OBJS_CHUNKING): %.o: $(SRC_PATH_CHUNKING)/%.cpp $(DEPS_CHUNKING) $(MAKEFILE)
	$(CC) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -c $< -o $@

# Config related build rules

$(OBJS_CONFIG): %.o: $(SRC_PATH_CONFIG)/%.cpp $(DEPS_CONFIG) $(MAKEFILE)
	$(CC) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -c $< -o $@

clean:
	$(RM) *.o *.d *.exe

