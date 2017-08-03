
all: build_dirs


.PHONY: build_dirs

build_dirs:
	@mkdir -p build


clean:
	@rm -rf build/
