#include "wsdd.nsmap"
#include "wsddapi.h"
#include "probe.h"
#include <stdio.h>
#include <string.h>

#include "common/sglib.h"
#include "common/dbg.h"


#define DISCOVERY_ADDRESS_UDP "soap.udp://239.255.255.250:3702"
#define DISCOVERY_TYPE "tds:Device"


int main() {
	int res;
	discovery_result_list_t *li 	= NULL, 
							*result = NULL;
	res = onvif_discover(&result);

	SGLIB_LIST_MAP_ON_ELEMENTS(discovery_result_list_t, result, li, next_ptr, {
		printf("Item\n\tService Address: %s\n\tMetadata Version: %d\n",
			li->service_address, li->metadata_version);
	});

	FREE_DISCOVERY_RESULT_LIST_T(result, li);

	return res;
}


int onvif_discover(discovery_result_list_t **probe_result){
	struct soap* serv 	= soap_new1(SOAP_IO_UDP);
	const char* uuid 	= soap_wsa_rand_uuid(serv);
	probe_codes out_res = PROBE_CODES_OK;
	int res 			= 0;

	discovery_result_list_t *li;

	if (!soap_valid_socket(soap_bind(serv, NULL, 0, 1000))) {
		soap_print_fault(serv, stderr);
		out_res = PROBE_CODES_BIND_ERROR;
		goto exit;
	}

	res = soap_wsdd_Probe(serv, SOAP_WSDD_ADHOC, SOAP_WSDD_TO_TS,
		DISCOVERY_ADDRESS_UDP, uuid, NULL, DISCOVERY_TYPE, NULL, NULL);

	if (res != SOAP_OK) {
		soap_print_fault(serv, stderr);
		out_res = PROBE_CODES_REQUEST_ERROR;
		goto exit;
	}

	soap_wsdd_listen(serv, -1000000);
	*probe_result = (discovery_result_list_t*)serv->user;

exit:
	soap_destroy(serv);
	soap_end(serv);
	soap_free(serv);
	return out_res;
}




void wsdd_event_ProbeMatches(
		struct soap		*soap,
		unsigned int	InstanceId,
		const char		*SequenceId,
		unsigned int	MessageNumber,
		const char		*MessageID,
		const char		*RelatesTo,
		struct wsdd__ProbeMatchesType	*matches){

	int i;
	discovery_result_list_t *item, *result = NULL;
	wsdd__ProbeMatchType *probe_match 	= matches->ProbeMatch;
	

	for (i = 0; i < matches->__sizeProbeMatch; i++){
		item = malloc(sizeof(discovery_result_list_t));
		item->metadata_version = probe_match->MetadataVersion;
		item->service_address = calloc(strlen(probe_match->XAddrs) + 1, 
			sizeof(char));
		strcpy(item->service_address, probe_match->XAddrs);
		SGLIB_LIST_ADD(discovery_result_list_t, result, item, next_ptr);

		probe_match++;
	}

	soap->user = (void*)result;
}


void wsdd_event_Hello(
		struct soap 	*soap,
		unsigned int	InstanceId,
		const char		*SequenceId,
		unsigned int	MessageNumber,
		const char		*MessageID,
		const char		*RelatesTo,
		const char		*EndpointReference,
		const char		*Types,
		const char		*Scopes,
		const char		*MatchBy,
		const char		*XAddrs,
		unsigned int	MetadataVersion){ }

void wsdd_event_Bye(	
		struct soap		*soap,
		unsigned int	InstanceId,
		const char		*SequenceId,
		unsigned int	MessageNumber,
		const char		*MessageID,
		const char		*RelatesTo,
		const char		*EndpointReference,
		const char		*Types,
		const char		*Scopes,
		const char		*MatchBy,
		const char		*XAddrs,
		unsigned int	*MetadataVersion) { }

soap_wsdd_mode wsdd_event_Probe(
		struct soap		*soap,
		const char		*MessageID,
		const char		*ReplyTo,
		const char		*Types,
		const char		*Scopes,
		const char		*MatchBy,
		struct wsdd__ProbeMatchesType	*matches) { return SOAP_WSDD_ADHOC; }

soap_wsdd_mode wsdd_event_Resolve (
		struct soap		*soap,
		const char		*MessageID,
		const char		*ReplyTo,
		const char		*EndpointReference,
		struct wsdd__ResolveMatchType	*match) { return SOAP_WSDD_ADHOC; }

void wsdd_event_ResolveMatches (
		struct soap		*soap,
		unsigned int	InstanceId,
		const char		*SequenceId,
		unsigned int	MessageNumber,
		const char		*MessageID,
		const char		*RelatesTo,
		struct wsdd__ResolveMatchType	*match) { }