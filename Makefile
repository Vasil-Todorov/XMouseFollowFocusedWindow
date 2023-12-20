xmffw: main.c
	@echo Compiling...
	@gcc main.c -lX11 -lm -o "xmffw"

.PHONY: install clean uninstall

install: xmffw
	@echo "Copying generated binary to /usr/bin"
	@cp ./xmffw /usr/bin/xmffw

uninstall:
	@echo "Removing link from /usr/bin"
	@rm /usr/bin/xmffw

clean:
	@echo "Cleaning files here"
	@rm xmffw
