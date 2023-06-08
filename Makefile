LDLIBS=-lpcap

all: deauth-attack

deauth-attack: main.o mac.o radiotap.o beacon.o
	$(LINK.cc) $^ $(LDLIBS) -o $@

clean:
	rm -f deauth-attack *.os