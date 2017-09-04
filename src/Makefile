all: $(TARGET)


SOURCES:=$(wildcard *.c)
OBJECTS:=$(SOURCES:%.c=$(BUILD_PATH)/%.o)


$(BUILD_PATH)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LINK_FLAGS) -o $@



