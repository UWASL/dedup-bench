MAKEFILE = Makefile
EXEC_NAME = measure-dedup
BUILD_DIR_PATH = ../../build


INCLUDE_PATH = ../include/
INCLUDE_FLAGS = -I $(INCLUDE_PATH)

SRC_PATH = ../src
SRC_MAIN = $(wildcard $(SRC_PATH)/*.cpp)
OBJS_MAIN = $(SRC_MAIN:$(SRC_PATH)/%.cpp=%.o)

COMPILER_FLAGS= -std=c++17 -Wall -Wextra -Wno-format

CC = g++
RM = rm -f

DEBUG = 1

ifeq ($(DEBUG), 1)
	$(COMPILER_FLAGS) += -g
endif

default: $(EXEC_NAME)
all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJS_MAIN)
	@echo ""
	@echo "Linking $@ ....."
	@echo "============================"
	$(CC) -o $(EXEC_NAME).exe $(OBJS_MAIN) $(INCLUDE_FLAGS) $(COMPILER_FLAGS)
	cp $(EXEC_NAME).exe $(BUILD_DIR_PATH)/$(EXEC_NAME).exe
	@echo ""

$(OBJS_MAIN): %.o: $(SRC_PATH)/%.cpp $(MAKEFILE)
	$(CC) $(INCLUDE_FLAGS) $(COMPILER_FLAGS) -c $< -o $@

clean:
	$(RM) *.o *.d *.exe