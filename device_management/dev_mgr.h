#ifndef _DEV_MGR_H_
#define _DEV_MGR_H_

#include <time.h>
#include "common/sglib.h"

typedef enum {
	NONE,
	BASIC,
	DIGEST
} auth_type_t;

#define FREE_SYSTEM_DATE_AND_TIME_T(item) free(item->timezone)
typedef struct {
	char *timezone; 
	enum {MANUAL, NTP} type;
	int daylight_saving;
	struct tm local;
	struct tm utc;
} system_date_and_time_t;

#define FREE_SYSTEM_INFORMATION_T(item) 									   \
	free(item->manufacturer);												   \
	free(item->model);														   \
	free(item->firm_ver);													   \
	free(item->serial_num);													   \
	free(item->hardware_id)

typedef struct { 	
	char *manufacturer;
	char *model;
	char *firm_ver;
	char *serial_num;
	char *hardware_id;
} system_information_t;

#define FREE_CAPABILITIES(item)
typedef struct {
	char dummy;
} capabilities_t;

#define FREE_SYSTEM_LOG(item) free(item->log_msg);
typedef struct {
	char *log_msg;
} system_log_t;


#define FREE_NETWORK_INTERFACE_LIST_T(item) free(item)
typedef struct {
	char *token;
	int enabled;
	struct network_interface_info {
		char *name;
		char *hw_addr;
		int mtu;
	} *info;

	struct network_interface_link {
		struct network_interface_link_settings {
			int auto_negotation;
			int speed;
			enum { FULL, HALF } duplex;
		} admin_settings, oper_settings;

		enum { ETHERNET = 6 } interface_type;
	} *link;

	struct network_interface_ipv4 {
		int enabled;
		struct network_interface_ipv4_config {
			struct network_interface_ipv4_config_prefixed_ipv4_address {
				char *address;
				int prefix_length;
				struct network_interface_ipv4_config_prefixed_ipv4_address *next_ptr;
			} *manual, *link_local, *from_dhcp;
			int dhcp;
		} config;
	} *ipv4;

	struct network_interface_ipv6 {
		int enabled;
		struct network_interface_ipv6_config {
			int accept_router_advert;
			enum { AUTO, STATEFUL, STATELESS, OFF } dhcp;

			struct network_interface_ipv6_config_prefixed_ipv6_address {
				char *address;
				int prefix_length;
				struct network_interface_ipv6_config_prefixed_ipv6_address *next_ptr;
			} *manual, *link_local, *from_dhcp, *from_ra;
		} config;
	} *ipv6;

	struct network_interface_extension
	{
		// TODO: complete structure
		char dummy;

	} *extension;

	struct network_interfaces_list_t *next_ptr;

} network_interfaces_list_t;


typedef struct 
{
	int needed;
} reboot_needed_t;

int get_system_date_time(const char *, system_date_and_time_t *);
int get_device_information(const char *, system_information_t *);
int get_capabilities(const char *, capabilities_t *);
int get_system_log(const char *, system_log_t *);
int get_network_interfaces(const char *, network_interfaces_list_t *);
int set_network_interface(const char *, const network_interfaces_list_t *,
	reboot_needed_t *);
#endif