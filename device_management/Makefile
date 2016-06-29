GSOAP_PREFIX=/usr
# /usr/share/gsoap
GSOAP_BASE=$(GSOAP_PREFIX)/share/gsoap
# /usr/share/gsoap/custom
GSOAP_CUSTOM=$(GSOAP_BASE)/custom
# /usr/share/gsoap/plugin
GSOAP_PLUGIN=$(GSOAP_BASE)/plugin
# /usr/share/gsoap/plugin
GSOAP_IMPORT=$(GSOAP_BASE)/import

GSOAP_CFLAGS= -g -I .. -I gen -I $(GSOAP_PLUGIN) -I $(GSOAP_IMPORT) -I $(GSOAP_CUSTOM) -DWITH_OPENSSL
GSOAP_LDFLAGS= -L $(GSOAP_PREFIX)/lib/ -L $(GSOAP_PREFIX) -lz -lcrypto -lssl -lgsoapssl 
#-lssl -lgsoapssl 

all: gen/soapC.cpp dev_mgr

gen/soapC.cpp: 
	mkdir -p gen
	wsdl2h -c -o gen/onvif.h http://www.onvif.org/ver10/device/wsdl/devicemgmt.wsdl
	soapcpp2 -Ec -Ed -C -L -c -r gen/onvif.h -I$(GSOAP_BASE) -d gen

dev_mgr: gen/soapClient.c gen/soapC.c dev_mgr.c $(GSOAP_CUSTOM)/duration.c $(GSOAP_PLUGIN)/httpda.c $(GSOAP_PLUGIN)/smdevp.c $(GSOAP_PLUGIN)/threads.c
	cc -o $@ $^ $(GSOAP_LDFLAGS) $(GSOAP_CFLAGS) 

	
	
clean:
	rm -rf dev_mgr gen

rebuild: clean all