all: $(TARGET) $(BUILD_PATH)/translation.o


SOURCES:=$(wildcard *.c)
OBJECTS:=$(SOURCES:%.c=$(BUILD_PATH)/%.o)


$(BUILD_PATH)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I $(INCLUDE_PATH) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LINK_FLAGS) -o $@


$(BUILD_PATH)/translation.o: x86_64_translations.s
	$(CC) --target=x86_64-pc-win32 -m64 -c $< -o $@ 
