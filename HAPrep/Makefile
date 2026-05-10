BUILD_DIR := build

.PHONY: all clean run

all:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -S .. -B . -Wno-dev > /dev/null
	@$(MAKE) -C $(BUILD_DIR) --no-print-directory

run: all
	@cd $(BUILD_DIR) && ./main

clean:
	@rm -rf $(BUILD_DIR)
