CFLAGS_DEBUG = -g -O0 -DDEBUG -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined
CFLAGS_RELEASE = -O2 -DNDEBUG -march=native -s -fomit-frame-pointer -flto -ffast-math

# Optional: If DEBUG_TIME is set to 1, add -DDEBUG_TIME to the CFLAGS
ifeq ($(DEBUG_TIME),1)
    CFLAGS_DEBUG += -DDEBUG_TIME
endif

LDFLAGS = -lX11 -lm
LDFLAGS_DEBUG = -fsanitize=address -fsanitize=undefined

SOURCES = src/main.c src/config.c src/cursor.c src/utils.c src/debug.c
OBJECTS_RELEASE = $(SOURCES:.c=.o)
OBJECTS_DEBUG = $(SOURCES:.c=.debug.o)

INCLUDES = -Iinclude

# Path to cache file where environment variables are stored
ENV_CACHE_FILE = .env_build_cache

# Rule to compile .o files from .c files
%.o: %.c
	@echo "Compiling $<"
	@gcc $(CFLAGS_RELEASE) $(INCLUDES) -c $< -o $@

%.debug.o: %.c
	@echo "Compiling $< for debug"
	@gcc $(CFLAGS_DEBUG) $(INCLUDES) -c $< -o $@

xmffw: check_env $(OBJECTS_RELEASE)
	@echo "Linking release build"
	@gcc $(OBJECTS_RELEASE) $(LDFLAGS) -o xmffw

xmffw-debug: check_env $(OBJECTS_DEBUG)
	@echo "Linking debug build"
	@gcc $(OBJECTS_DEBUG) $(LDFLAGS) $(LDFLAGS_DEBUG) -o xmffw-debug

.PHONY: debug build install clean uninstall force save_env check_env

debug: xmffw-debug

# Force rule for the debug build
force: clean-debug xmffw-debug

# Clean only debug object files
clean-debug:
	@echo "Cleaning debug object files"
	@rm -f $(OBJECTS_DEBUG)

build: xmffw

install: xmffw
	@echo "Copying generated binary to /usr/bin"
	@cp ./xmffw /usr/bin/xmffw

uninstall:
	@echo "Removing link from /usr/bin"
	@rm /usr/bin/xmffw

clean:
	@echo "Cleaning files"
	@rm -f $(OBJECTS_RELEASE) $(OBJECTS_DEBUG) $(ENV_CACHE_FILE) xmffw xmffw-debug

# Save the environment variables that affect the build into a file
save_env:
	@echo "Saving environment variables"
	@echo "CFLAGS_DEBUG=$(CFLAGS_DEBUG)" > $(ENV_CACHE_FILE)
	@echo "DEBUG_TIME=$(DEBUG_TIME)" >> $(ENV_CACHE_FILE)
	@echo "CFLAGS_RELEASE=$(CFLAGS_RELEASE)" >> $(ENV_CACHE_FILE)
	@echo "LDFLAGS=$(LDFLAGS)" >> $(ENV_CACHE_FILE)
	@echo "LDFLAGS_DEBUG=$(LDFLAGS_DEBUG)" >> $(ENV_CACHE_FILE)

# Check if environment variables have changed since the last build
check_env:
	@if [ -f $(ENV_CACHE_FILE) ]; then \
		if ! cmp --silent <(echo "CFLAGS_DEBUG=$(CFLAGS_DEBUG)"; echo "DEBUG_TIME=$(DEBUG_TIME)"; echo "CFLAGS_RELEASE=$(CFLAGS_RELEASE)"; echo "LDFLAGS=$(LDFLAGS)"; echo "LDFLAGS_DEBUG=$(LDFLAGS_DEBUG)") $(ENV_CACHE_FILE); then \
			echo "Environment variables have changed. Forcing clean build."; \
			$(MAKE) clean; \
		fi; \
	fi; \
	$(MAKE) save_env
