all:
	gcc src/kui_midi_alsa.c -o kui_midi_alsa -lasound -lpthread
	gcc src/kui_test.c -o kui_test -lasound
	gcc src/kui_simply.c -o kui_simply -lasound
clean:
	rm kui_midi_alsa
	rm kui_test
install:
	install -m 0755 kui_midi_alsa /usr/local/bin
uninstall:
	rm /usr/local/bin/kui_ttymidi
