CC = gcc
CFLAGS = -Iinclude -Wall -g
LDFLAGS =

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = .

# Source and object files
SOURCES = $(SRC_DIR)/dictionary.c $(SRC_DIR)/spchk.c
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Target executable name
TARGET = $(BIN_DIR)/spchk

# Default rule to build the program
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependencies
-include $(OBJECTS:.o=.d)

# Generate dependencies
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CFLAGS) -MM -MT '$(@:.d=.o)' $< >$@

# Clean build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d $(TARGET)

# Rule for cleaning and then building
rebuild: clean $(TARGET)

.PHONY: clean rebuild
