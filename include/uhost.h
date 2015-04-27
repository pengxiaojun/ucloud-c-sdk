#ifndef __UCLOUD_UHOST_H
#define __UCLOUD_UHOST_H

#include <uinclude.h>
#include <uparams.h>

/**
 * @file uhost.h
 * @brief encapsulate uhost api
 * @author pengxj
 * @version 1.0
 * @date 2015-04-16
 */

/**
 * @brief uhost parameter mask definition. used by modify uhost attribute
 */
typedef struct
{
	uint64_t region:1;       /*!< region */
	uint64_t image_id:1;     /*!< host image id */
	uint64_t name:1;         /*!< host name */
	uint64_t password:1;     /*!< host password */
	uint64_t key_pair:1;     /*!< host key pair  */
	uint64_t login_mode:1;   /*!< login mode */
	uint64_t cpu:1;          /*!< cpu of host */
	uint64_t memory:1;       /*!< memory of host */
	uint64_t disk_space:1;   /*!< disk space of host */
	uint64_t charge_type:1;  /*!< charge type */
	uint64_t quantity:1;     /*!< quantity of host */
	uint64_t count:1;        /*!< count of host */
	uint64_t pad:52;         /*!< pad */
}ucloud_uhost_param_mask_t;

/**
 * @brief ucloud disk definition
 */
typedef struct ucloud_disk_t
{
	char *id;                    /*!< disk id */
	int type;                    /*!< disk type.reference enum ucloud_disk_type_t enum */
	int capacity;                /*!< disk capacity.GB */
	struct ucloud_disk_t *next;  /*!< next disk */
}ucloud_disk_t;

/**
 * @brief ucloud ip definition
 */
typedef struct ucloud_ip_t
{
	int type;                   /*!< ip type */
	char *ip;                   /*!< ip address */
	char *id;                   /*!< id of ip */
	int bandwidth;              /*!< bandwidth. MB */
	struct ucloud_ip_t *next;   /*!< next ip */
}ucloud_ip_t;

/**
 * @brief ucloud uhost definition
 */
typedef struct ucloud_uhost_t
{
	char *id;                     /*!< host id */
	char *name;                   /*!< host name */
	char *region;                 /*!< region host belong to */
	char *image_id;               /*!< host image id */
	char *basic_image_id;         /*!< basic image id */
	char *basic_image_name;       /*!< basic image name */
	char *tag;                    /*!< tag of host */
	char *remark;                 /*!< remark of host */
	int status;                   /*!< host status. reference ucloud_uhost_status_t */
	int created_time;             /*!< created time of host */
	int expired_time;             /*!< expired time of host */
	int type;                     /*!< host type. reference ucloud_uhost_type_t */
	int cpu;                      /*!< number of cpu in host. power of 2 */
	int memory;                   /*!< memcoy size unit:MB */
	int charge_type;              /*!< charge type. reference ucloud_charge_type_t */
	ucloud_disk_t *diskset;       /*!< disks in host */
	ucloud_ip_t *ipset;           /*!< ip set in host */
	struct ucloud_uhost_t *next;  /*!< point to next host */
}ucloud_uhost_t;



/**
 * @brief uhost ssr action. ssr = {start, stop, reboot}
 */
typedef enum
{
	uuis_start  = 0, /*!< start host */
	uuis_stop   = 1, /*!< stop host */
	uuis_reboot = 2  /*!< reboot host */
}ucloud_uhost_ssr_t;



/**
 * @brief host ntr attribute. ntr = {name, tag, remark}
 */
typedef enum
{
	uun_name,        /*!< host name */
	uun_tag,         /*!< host tag */
	uun_remark       /*!< host remark */
}ucloud_uhost_ntr_t;


/**
 * @brief uhost reinstall parameter mask definition
 */
typedef struct
{
	uint64_t password:1;
	uint64_t image_id:1;
	uint64_t reserve_disk:1;
	uint64_t pad:61;
}ucloud_uhost_reinstall_param_mask_t;

/**
 * @brief uhost price definition
 */
typedef struct ucloud_uhost_price_t
{
	int charge_type;                    /*!< charge type. reference ucloud_charge_type_t enum */
	float price;                        /*!< host price */
	struct ucloud_uhost_price_t *next;  /*!< next price */
}ucloud_uhost_price_t;


/**
 * @brief uhost vnc information definition
 */
typedef struct
{
	int port;     /*!< vnc port */
	char *ip;     /*!< vnc ip */
	char *passwd; /*!< vnc password */
}ucloud_uhost_vnc_t;

/**
 * @brief uhost constructor
 *
 * @return 
 */
ucloud_uhost_t* ucloud_uhost_init();

/**
 * @brief uhost deconstructor
 *
 * @param host
 *
 * @return 
 */
int ucloud_uhost_deinit(ucloud_uhost_t *host);

/**
 * @brief set uhost structure string format parameter
 *
 * @param param parameter collection
 * @param mask which parameter to be setted
 * @param val  parameter value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_set_str_param(ucloud_http_params_t *param, ucloud_uhost_param_mask_t *mask, const char *val);

/**
 * @brief set host integer format parameter
 *
 * @param param parameter collection
 * @param mask whick parameter to be setted
 * @param val parameter value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_set_int_param(ucloud_http_params_t *param, ucloud_uhost_param_mask_t *mask, int val);

/**
 * @brief  create uhost
 *
 * @param params parameter provided to uhost
 * @param host_ids host id if created success
 * @param count count of host ids
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_create(ucloud_http_params_t *params, char **host_ids, int *count);

/**
 * @brief terminate uhost
 *
 * @param region region
 * @param host_id uhost id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_terminate(const char *region, const char *host_id);

/**
 * @brief ssr(start,stop,reboot) uhost
 *
 * @param act ssr action. reference ucloud_uhost_ssr_t enum
 * @param region region
 * @param host_id host id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_ssr(int act, const char *region, const char *host_id);

/**
 * @brief describe uhost
 *
 * @param region region
 * @param host_id host id
 *
 * @return success return uhost should be free by ucloud_uhost_describe_deinit else return NULL
 */
ucloud_uhost_t* ucloud_uhost_describe(const char *region, const char *host_id);

/**
 * @brief free describe uhost data
 *
 * @param host host data returned from ucloud_uhost_describe
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_describe_deinit(ucloud_uhost_t *host);

/**
 * @brief resize host
 *
 * @param param which item will be resized(cpu, memory,disk capacity)
 * @param region region
 * @param host_id host id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_resize(ucloud_http_params_t *param, const char *region, const char *host_id);

/**
 * @brief reinstall host
 *
 * @param param reinstall parameter.(parameter is setted by ucloud_uhost_set_reinstall_param
 * @param region region
 * @param host_id host id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_reinstall(ucloud_http_params_t *param, const char *region, const char *host_id);

/**
 * @brief set reinstall parameter
 *
 * @param param parameter collection
 * @param mask reinstall parameter mask
 * @param val parameter value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_set_reinstall_param(ucloud_http_params_t *param, ucloud_uhost_reinstall_param_mask_t *mask, const char *val);

/**
 * @brief reset uhost password
 *
 * @param region region
 * @param host_id host id
 * @param passwd new host password
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_reset_passwd(const char *region, const char *host_id, const char *passwd);

/**
 * @brief modify uhost ntr information(name, tag, remark)
 *
 * @param act reference ucloud_uhost_ntr_t enum
 * @param region region
 * @param host_id host id
 * @param val new modified value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhost_ntr(int act, const char *region, const char *host_id, const char *val);

/**
 * @brief get host price
 *
 * @param param parameter need in price(ImageId, CPU, Memory,Count etc.)
 *
 * @return success return uhost price should be free by ucloud_uhost_get_price_deinit else return NULL
 */
ucloud_uhost_price_t* ucloud_uhost_get_price(ucloud_http_params_t* param);
int ucloud_uhost_get_price_deinit(ucloud_uhost_price_t* price);

/**
 * @brief get uhost vnc information
 *
 * @param region region
 * @param host_id host id
 *
 * @return success return vnc information should be free by vnc deconstructor else return NULL
 */
ucloud_uhost_vnc_t* ucloud_uhost_get_vnc(const char *region, const char *host_id);

/**
 * @brief uhost disk consructor
 *
 * @return 
 */
ucloud_disk_t* ucloud_disk_init();

/**
 * @brief uhost disk deconstructor
 *
 * @param disk
 *
 * @return 
 */
int ucloud_disk_deinit(ucloud_disk_t *disk);

/**
 * @brief uhost ip constructor
 *
 * @return 
 */
ucloud_ip_t* ucloud_ip_init();

/**
 * @brief uhost ip deconstructor
 *
 * @param ip
 *
 * @return 
 */
int ucloud_ip_deinit(ucloud_ip_t *ip);

/**
 * @brief uhost price constructor
 *
 * @return 
 */
ucloud_uhost_price_t* ucloud_uhost_price_init();

/**
 * @brief uhost price deconstructor
 *
 * @param price
 *
 * @return 
 */
int ucloud_uhost_price_deinit(ucloud_uhost_price_t *price);

/**
 * @brief uhost vnc constructor
 *
 * @return 
 */
ucloud_uhost_vnc_t* ucloud_uhost_vnc_init();

/**
 * @brief uhost vnc deconstructor
 *
 * @param vnc
 *
 * @return 
 */
int ucloud_uhost_vnc_deinit(ucloud_uhost_vnc_t *vnc);
#endif
