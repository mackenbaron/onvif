#include "DeviceBinding.nsmap"
#include "soapH.h"
#include "dev_mgr.h"
#include "httpda.h"
#include "common/sglib.h"
#include <string.h>

#define SERVICE_ADDR "http://192.168.1.66:2000/onvif/device_service"

/* 
 * Macro for inline authorized request.
 *
 *  REQ - full function call with parametrs of gSoap method;
 *  CALLBACK - full function call of method what parse response;
 *  SOAP - the name of the pointer to struct soap;
 *	AUTH_TYPE - the name of the auth_type_t variable;
 *  UID - the name of variable what stores user id;
 *  PSSWD - the name of variable what stores user password;
 *	EXIT_LBL - the name of exit label;
 *	RES_CODE - the name of the variable what stores result codes.  
 */
#define AUTHORIZED_REQUEST(REQ, CALLBACK, SOAP, AUTH_TYPE, UID, PSSWD, 		   \
	EXIT_LBL, RES_CODE)														   \
	create_auth(SOAP, UID, PSSWD, AUTH_TYPE);							   	   \
    /* try calling without authenticating */								   \
    RES_CODE = REQ;		   													   \
    if (RES_CODE){															   \
    	if (SOAP->error == 401){											   \
    		if (AUTH_TYPE == NONE) goto EXIT_LBL;							   \
    		struct http_da_info info;										   \
    		http_da_save(SOAP, &info, SOAP->authrealm, UID, PSSWD);			   \
    		/* call again, now with credentials */							   \
    		RES_CODE = REQ;													   \
    		http_da_release(SOAP, &info); /* remove userid and passwd */	   \
    	}																	   \
    	if (RES_CODE == SOAP_OK) {											   \
    		CALLBACK;														   \
    	}																	   \
    }else{																	   \
    	soap_print_fault(SOAP, stderr);										   \
    }																			

// append digest to existing soap message
static inline void create_auth(struct soap *soap, const char *userid, 
        const char* passwd, auth_type_t auth_type){
    switch (auth_type) {
        case DIGEST:
            soap_register_plugin(soap, http_da);
        break;
        case BASIC:
            soap->userid = userid;
            soap->passwd = passwd;
        break;
    }
}

static inline void end_soap(struct soap *soap) {
    soap_destroy(soap);
    soap_end(soap);
    soap_free(soap);
}


int main(){
    network_interfaces_list_t item;
    get_network_interfaces(SERVICE_ADDR, &item);
    return 0;
}

static inline void parse_get_system_date_and_time(
		struct _tds__GetSystemDateAndTimeResponse *resp,
		system_date_and_time_t *out) {
    char *p;
    int len;

    p = resp->SystemDateAndTime->TimeZone->TZ;
    len = strlen(p);
    out->timezone = malloc((len + 1) * sizeof(char));
    strcpy(out->timezone, resp->SystemDateAndTime->TimeZone->TZ);

    out->daylight_saving = resp->SystemDateAndTime->DaylightSavings;
    	
    out->utc.tm_sec = resp->SystemDateAndTime->UTCDateTime->Time->Second;
    out->utc.tm_min = resp->SystemDateAndTime->UTCDateTime->Time->Minute;
    out->utc.tm_hour = resp->SystemDateAndTime->UTCDateTime->Time->Hour;
    out->utc.tm_mday = resp->SystemDateAndTime->UTCDateTime->Date->Day;
    out->utc.tm_mon = resp->SystemDateAndTime->UTCDateTime->Date->Month;
    out->utc.tm_year = resp->SystemDateAndTime->UTCDateTime->Date->Year;
    	
    out->local.tm_sec = resp->SystemDateAndTime->UTCDateTime->Time->Second;
    out->local.tm_min = resp->SystemDateAndTime->UTCDateTime->Time->Minute;
    out->local.tm_hour = resp->SystemDateAndTime->UTCDateTime->Time->Hour;
    out->local.tm_mday = resp->SystemDateAndTime->UTCDateTime->Date->Day;
    out->local.tm_mon = resp->SystemDateAndTime->UTCDateTime->Date->Month;
    out->local.tm_year = resp->SystemDateAndTime->UTCDateTime->Date->Year; 
}


int get_system_date_time(const char* service_url, system_date_and_time_t *res){
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/GetSystemDateAndTime";

    struct _tds__GetSystemDateAndTimeResponse resp;
    struct _tds__GetSystemDateAndTime req;

    res_code = soap_call___tds__GetSystemDateAndTime(soap, service_url, 
        action_url, &req, &resp);

    if (res_code == SOAP_OK)
        parse_get_system_date_and_time(&resp, res);
    else
        soap_print_fault(soap, stderr);

exit:
    end_soap(soap);
    return res_code;
}


static inline void parse_get_device_information(
        struct _tds__GetDeviceInformationResponse *resp,
        system_information_t *res) {
    res->manufacturer = malloc((strlen(resp->Manufacturer) + 1)*sizeof(char));
    res->model = malloc((strlen(resp->Model) + 1)*sizeof(char));
    res->firm_ver = malloc((strlen(resp->FirmwareVersion) + 1)*sizeof(char));
    res->serial_num = malloc((strlen(resp->SerialNumber) + 1)*sizeof(char));
    res->hardware_id = malloc((strlen(resp->HardwareId) + 1)*sizeof(char));

    strcpy(res->manufacturer, resp->Manufacturer);
    strcpy(res->model       , resp->Model);
    strcpy(res->firm_ver    , resp->FirmwareVersion);
    strcpy(res->serial_num  , resp->SerialNumber);
    strcpy(res->hardware_id , resp->HardwareId);
}

int get_device_information(const char *service_url, system_information_t *res){
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/GetDeviceInformation";

    struct _tds__GetDeviceInformation req;
    struct _tds__GetDeviceInformationResponse resp;

    res_code = soap_call___tds__GetDeviceInformation(soap, service_url,
        action_url, &req, &resp);

    if (res_code == SOAP_OK)
        parse_get_device_information(&resp, res);
    else
        soap_print_fault(soap, stderr);

exit:
    end_soap(soap);
    return res_code;
}


static inline void parse_get_capabilities(
        struct _tds__GetCapabilitiesResponse *resp,
        capabilities_t *result){

}

int get_capabilities(const char *service_url, capabilities_t *res){
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/GetCapabilities";
    struct _tds__GetCapabilities req;
    struct _tds__GetCapabilitiesResponse resp;
    
    res_code = soap_call___tds__GetCapabilities(soap, service_url,
        action_url, &req, &resp);

    if (res_code == SOAP_OK)
        parse_get_capabilities(&resp, res);
    else
        soap_print_fault(soap, stderr);

exit:
    end_soap(soap);
    return res_code;
}


static inline void parse_get_system_log(
    struct _tds__GetSystemLogResponse *resp,
    system_log_t *result){

}

int get_system_log(const char *service_url, system_log_t *res) {
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/GetSystemLog";

    struct _tds__GetSystemLog req;
    struct _tds__GetSystemLogResponse resp;

    res_code = soap_call___tds__GetSystemLog(soap, service_url, 
        action_url, &req, &resp);

    if (res_code == SOAP_OK) 
        parse_get_system_log(&resp, res);
    else
        soap_print_fault(soap, stderr);

exit:
    end_soap(soap);
    return res_code;
}


static inline void parse_get_network_interfaces(
        struct _tds__GetNetworkInterfacesResponse *resp,
        network_interfaces_list_t *result) {
    int i;
    network_interfaces_list_t *li;
    struct tt__NetworkInterface *intf;

    for (i = 0; i < resp->__sizeNetworkInterfaces; i++) {
        intf = &resp->NetworkInterfaces[i];
        li = malloc(sizeof(network_interfaces_list_t));
        if (!li) return;

        li->token = malloc((strlen(intf->token) + 1) * sizeof(char));
        strcpy(li->token, intf->token);

        li->enabled = intf->Enabled;

        if (intf->Info) {
            li->info = malloc(sizeof(struct network_interface_info));
            li->info->name = malloc((strlen(intf->Info->Name) + 1) 
                * sizeof(char));
            strcpy(li->info->name, intf->Info->Name);

            li->mtu = *intf->Info->MTU;

            li->info->hw_addr = malloc((strlen(intf->Info->HwAddress) + 1) 
                * sizeof(char));
            strcpy(li->info->hw_addr, intf->Info->HwAddress);
        } else
            li->info = NULL;

        if (intf->Link) {
            li->link = malloc(sizeof(struct network_interface_link));
            li->link->interface_type = intf->Link->InterfaceType;

            if (intf->Link->AdminSettings){
                li->link->admin_settings = { 
                    auto_negotation = intf->Link->AdminSettings->AutoNegotiation,
                    speed = intf->Link->AdminSettings->Speed,
                    duplex = intf->Link->AdminSettings->Duplex
                };
            }

            if (intf->Link->OperSettings){
                li->link->oper_settings = { 
                    auto_negotation = intf->Link->OperSettings->AutoNegotiation,
                    speed = intf->Link->OperSettings->Speed,
                    duplex = intf->Link->OperSettings->Duplex
                };
            }

        } else
            li->link = NULL;

        if (intf->IPv4){
            li->ipv4 = malloc(sizeof(struct network_interface_ipv4));
            li->ipv4->enabled = intf->IPv4->Enabled;
            if (intf->IPv4->Config) {

            }
        } else
            li->ipv4 = NULL;

        if (intf->IPv6){

        } else
            li->ipv6 = NULL;

        if (intf->Extension){

        } else
            li->extension = NULL;

        SGLIB_LIST_ADD(network_interfaces_list_t, result, li, next_ptr);
    }
}

int get_network_interfaces(const char *service_url, 
        network_interfaces_list_t *res) {
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/GetNetworkInterfaces";

    struct _tds__GetNetworkInterfaces req;
    struct _tds__GetNetworkInterfacesResponse resp;

    res_code = soap_call___tds__GetNetworkInterfaces(soap, service_url,
        action_url, &req, &resp);

    if (res_code == SOAP_OK)
        parse_get_network_interfaces(&resp, res);
    else
        soap_print_fault(soap, stderr);

exit:
    end_soap(soap);
    return res_code;
}

static inline void parse_reboot_needed_t(void *resp, reboot_needed_t *res) {

}

static inline void parse_set_network_interface(
        const network_interfaces_list_t *intf,
        struct _tds__SetNetworkInterfaces* result) {

}

int set_network_interface(const char *service_url, 
        const network_interfaces_list_t *intf, reboot_needed_t *res){
    int res_code = 0;
    struct soap *soap = soap_new();
    static const char *action_url = 
        "http://www.onvif.org/ver10/device/wsdl/SetNetworkInterfaces";

    struct _tds__SetNetworkInterfaces req;
    struct _tds__SetNetworkInterfacesResponse resp;

    parse_set_network_interface(intf, &req);

    res_code = soap_call___tds__SetNetworkInterfaces(soap, service_url,
        action_url, &req, &resp);

    if (res_code == SOAP_OK)
        parse_reboot_needed_t(&resp, res);
    else
        soap_print_fault(soap, stderr);
exit:
    end_soap(soap);
    return res_code;   
}