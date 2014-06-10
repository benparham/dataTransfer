CC = gcc
AR = ar

TRANSFER = libtransfer.a

INCLUDE_DIR = include
SOURCE_DIR = src
BUILD_DIR = build

CLIENT_INC_DIR = ../client/include
CLIENT_LIB_DIR = ../client/lib
SERV_INC_DIR = ../serv/include
SERV_LIB_DIR = ../serv/lib

# Flags
CFLAGS = -g -Wall -std=c99 -I $(INCLUDE_DIR)
ARFLAGS = ru

# Header files
INC = $(shell find $(INCLUDE_DIR) -type f -name '*.h')

# Source files
SRC = $(shell find $(SOURCE_DIR) -type f -name '*.c')

# Object files
OBJ = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC))

# Rules #######################

all: $(TRANSFER)

$(TRANSFER): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@
	@ln -f $(INCLUDE_DIR)/transfer.h $(CLIENT_INC_DIR)/transfer.h
	@ln -f $(INCLUDE_DIR)/transfer.h $(SERV_INC_DIR)/transfer.h
	@ln -f $(TRANSFER) $(CLIENT_LIB_DIR)/$(TRANSFER)
	@ln -f $(TRANSFER) $(SERV_LIB_DIR)/$(TRANSFER)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(INC)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TRANSFER)
	@rm -f $(CLIENT_INC_DIR)/transfer.h
	@rm -f $(SERV_INC_DIR)/transfer.h
	@rm -f $(SERV_LIB_DIR)/$(TRANSFER)
	@rm -f $(CLIENT_LIB_DIR)/$(TRANSFER)

###############################