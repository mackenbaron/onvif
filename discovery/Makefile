GSOAP_PREFIX=/usr
# /usr/share/gsoap
GSOAP_BASE=$(GSOAP_PREFIX)/share/gsoap
# /usr/share/gsoap/import
GSOAP_IMPORT=$(GSOAP_BASE)/import
# /usr/share/gsoap/plugins
GSOAP_PLUGINS=$(GSOAP_BASE)/plugin

GSOAP_CFLAGS= -g -I gen -I $(GSOAP_PREFIX) -I $(GSOAP_PLUGINS) -I ..
GSOAP_LDFLAGS= -lgsoap -L $(GSOAP_PREFIX)/lib/

all: gen/soapC.cpp probe

gen/soapC.cpp: 
	mkdir -p gen
	wsdl2h -c -o gen/onvif.h http://www.onvif.org/ver10/network/wsdl/remotediscovery.wsdl 
	soapcpp2 -Ec -Ed -C -L -c -r gen/onvif.h -I $(GSOAP_IMPORT) -d gen

probe:   $(GSOAP_PLUGINS)/wsddapi.c $(GSOAP_PLUGINS)/wsaapi.c gen/soapClient.c gen/soapC.c probe.c
	cc -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 

	
	
clean:
	rm -rf probe gen

rebuild: clean all

