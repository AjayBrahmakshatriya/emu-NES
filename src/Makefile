all: $(TARGET)



.PHONY: $(BUILD_PATH)/driver.o
.PHONY: $(BUILD_PATH)/file_loader.o


$(TARGET): $(BUILD_PATH)/file_loader.o $(BUILD_PATH)/driver.o
	$(CC) $^ $(LINK_FLAGS) -o $@

$(BUILD_PATH)/file_loader.o:
	make -C file_loader

$(BUILD_PATH)/driver.o:
	make -C driver
