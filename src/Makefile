all: $(TARGET) $(BUILD_PATH)/translation.o



SOURCES:=$(wildcard *.c)
OBJECTS:=$(SOURCES:%.c=$(BUILD_PATH)/%.o) $(BUILD_PATH)/execution_context_assembly.o


$(BUILD_PATH)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -I $(INCLUDE_PATH) -c $< -o $@


$(TARGET): $(OBJECTS)
	$(CC) $^ $(LINK_FLAGS) -o $@


$(BUILD_PATH)/translation.o: x86_64_translations.s
	$(CC) --target=x86_64-pc-win32 -m64 -c $< -o $@ 

SYS:=$(shell $(CC) -dumpmachine)

$(BUILD_PATH)/execution_context_assembly.o: execution_context_assembly.s
ifneq (, $(findstring apple, $(SYS)))
	$(CC) -DIS_MAC_OS -E $< -o $(BUILD_PATH)/execution_context_assembly.preprocessed.s 
else
	$(CC) -E $< -o $(BUILD_PATH)/execution_context_assembly.preprocessed.s 
endif
	$(CC) -m64 -c $(BUILD_PATH)/execution_context_assembly.preprocessed.s -o $@
