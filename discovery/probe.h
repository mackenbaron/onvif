#ifndef _PROBE_H_
#define _PROBE_H_

#define FREE_DISCOVERY_RESULT_LIST_T(item, li) 								   \
	SGLIB_LIST_MAP_ON_ELEMENTS(discovery_result_list_t, item, li, next_ptr, {  \
		free(li->service_address); 											   \
		free(li); 															   \
	})

typedef struct discovery_result_list {
	unsigned int metadata_version;
	char* service_address;
	struct discovery_result_list *next_ptr;
} discovery_result_list_t;

typedef enum {
	PROBE_CODES_OK,
	PROBE_CODES_BIND_ERROR,
	PROBE_CODES_REQUEST_ERROR
} probe_codes;

/*
 * 
 */
int onvif_discover(discovery_result_list_t **result);

#endif /* _PROBE_H_ */