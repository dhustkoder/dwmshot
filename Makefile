LDFLAGS=-lX11 -lpng

all: dwmshot

dwmshot: dwmshot.c config.h
	$(CC) $< -o $@ $(LDFLAGS)

config.h: config.def.h
	cp config.def.h config.h

install: dwmshot
	cp dwmshot /usr/local/bin

uninstall:
	rm /usr/local/bin/dwmshot

clean:
	rm dwmshot

