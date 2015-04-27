#ifndef __UCLOUD_UCOMMON_H
#define __UCLOUD_UCOMMON_H

/**
 * @file ucommon.h
 * @brief define common types
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */


/**
 * @brief define max length of region
 */
#define  MAX_REGION_LEN 64

/**
 * @brief ucloud charge type
 */
typedef enum
{
	uct_year    = 1,
	uct_month   = 2,
	uct_dynamic = 3,
	uct_trial   = 4
}ucloud_charge_type_t;

/**
 * @brief ucloud ip type
 */
typedef enum
{
	uit_telecom,
	uit_unicom,
	uit_internation,
	uit_BGP,
	uit_private,
	uit_duplet
}ucloud_ip_type_t;

/**
 * @brief ucloud ip status
 */
typedef enum
{
	uis_used,
	uis_free,
	uis_freeze
}ucloud_ip_status_t;

/**
 * @brief ucloud bandwidth type
 */
typedef enum
{
	ubt_nonshared,
	ubt_shared
}ucloud_bandwidth_type_t;

/**
 * @brief ucloud disk type
 */
typedef enum
{
	udt_boot,
	udt_data,
	udt_udisk
}ucloud_disk_type_t;

/**
 * @brief ucloud long mode
 */
typedef enum 
{
	uulm_passwd  = 1,
	uulm_keypair = 2
}ucloud_uhost_login_mode_t;


/**
 * @brief ucloud host type
 */
typedef enum{
	uut_normal,
	uut_ssd,
	uut_bd
}ucloud_uhost_type_t;


/**
 * @brief ucloud host status
 */
typedef enum
{
	uus_initializing,
	uus_starting,
	uus_running,
	uus_stopping,
	uus_stopped,
	uus_install_fail,
	uus_rebooting
}ucloud_uhost_status_t;

/**
 * @brief ucloud resource type
 */
typedef enum
{
	urt_uhost   = 1,
	urt_udb     = 2,
	urt_ulb     = 3,
	urt_umem    = 4,
	urt_vrouter = 5
}ucloud_resource_type_t;

/**
 * @brief ucloud protocol type
 */
typedef enum
{
	upt_tcp,
	upt_udp,
	upt_icmp,
	upt_gre
}ucloud_prot_type_t;

/**
 * @brief ucloud security group type
 */
typedef enum
{
	ust_custom,
	ust_web,
	ust_nonweb
}ucloud_sg_type_t;


/**
 * @brief action of security group rule
 */
typedef enum
{
	usr_accept,
	usr_drop
}uclud_sg_rule_action_t;

/**
 * @brief convert to charge type from string format
 *
 * @param type charge type in string format
 *
 * @return success return charge type else return -1
 */
int ucloud_to_charge_type(const char *type);

/**
 * @brief convert charge type to string format
 *
 * @param type charge type in number format
 *
 * @return success return charge type else return NULL
 */
const char *ucloud_from_charge_type(int type);

/**
 * @brief convert to ip type from string format
 *
 * @param type ip type in string format
 *
 * @return success return ip type else return -1
 */
int ucloud_to_ip_type(const char *type);

/**
 * @brief convert ip type to string format
 *
 * @param type ip type in number format
 *
 * @return success return ip type else return NULL
 */
const char *ucloud_from_ip_type(int type);

/**
 * @brief convert to ip stauts from string format
 *
 * @param type ip status in string format
 *
 * @return success return ip status else return -1
 */
int ucloud_to_ip_status(const char *type);

/**
 * @brief convert ip status to string format
 *
 * @param type ip status in number format
 *
 * @return success return ip status else return NULL
 */
const char *ucloud_from_ip_status(int type);

/**
 * @brief convert to bandwidth type from string format
 *
 * @param type bandwidth type in string format
 *
 * @return success return bandwidth type else return -1
 */
int ucloud_to_bandwidth_type(const char *type);

/**
 * @brief convert bandwidth type to string format
 *
 * @param type bandwidth type in number format
 *
 * @return success return bandwidth type else return NULL
 */
const char *ucloud_from_bandwidth_type(int type);

/**
 * @brief convert to disk type from string format
 *
 * @param type disk type in string format
 *
 * @return success return disk type else return -1
 */
int ucloud_to_disk_type(const char *type);

/**
 * @brief convert disk type to string format
 *
 * @param type disk type in number format
 *
 * @return success return disk type else return NULL
 */
const char *ucloud_from_disk_type(int type);

/**
 * @brief convert to uhost type from string format
 *
 * @param type uhost type in string format
 *
 * @return success return uhost type else return -1
 */
int ucloud_to_uhost_type(const char *type);

/**
 * @brief convert uhost type to string format
 *
 * @param type uhost type in number format
 *
 * @return success return uhost type else return NULL
 */
const char *ucloud_from_uhost_type(int type);

/**
 * @brief convert to uhost status from string format
 *
 * @param status uhost status in string format
 *
 * @return success return uhost status else return -1
 */
int ucloud_to_uhost_status(const char *status);

/**
 * @brief convert uhost status to string format
 *
 * @param status uhost status in number format
 *
 * @return success return uhost status else return NULL
 */
const char *ucloud_from_uhost_status(int status);

/**
 * @brief convert to login mode from string format
 *
 * @param mode uhost status in string format
 *
 * @return success return login mode else return -1
 */
int ucloud_to_uhost_login_mode(const char *mode);

/**
 * @brief convert login mode to string format
 *
 * @param mode long model in number format
 *
 * @return success return login mode else return NULL
 */
const char *ucloud_from_uhost_login_mode(int mode);

/**
 * @brief convert to operator name from string format
 *
 * @param name operator name in string format
 *
 * @return success return login mode else return -1
 */
int ucloud_to_operator_name(const char *name);

/**
 * @brief convert operator name to string format
 *
 * @param name operator name in number format
 *
 * @return success return operator name return NULL
 */
const char *ucloud_from_operator_name(int name);

/**
 * @brief convert to resource type from string format
 *
 * @param type resource type in string format
 *
 * @return success return resource type else return -1
 */
int ucloud_to_resource_type(const char *type);

/**
 * @brief convert resource type to string format
 *
 * @param type resource type number format
 *
 * @return success return resource type else return NULL
 */
const char *ucloud_from_resource_type(int type);

/**
 * @brief convert to protocol type from string format
 *
 * @param type protocol type in string format
 *
 * @return success return protocol type else return -1
 */
int ucloud_to_prot_type(const char *type);

/**
 * @brief convert protocol type to string format
 *
 * @param type protocol type number format
 *
 * @return success return protocol type else return NULL
 */
const char *ucloud_from_prot_type(int type);

/**
 * @brief convert to rule action from string format
 *
 * @param action rule action in string format
 *
 * @return success return rule action else return -1
 */
int ucloud_to_rule_action(const char *action);

/**
 * @brief convert rule action to string format
 *
 * @param action rule action number format
 *
 * @return success return rule action else return NULL
 */
const char *ucloud_from_rule_action(int action);

/**
 * @brief convert to security group type from string format
 *
 * @param type security group type in string format
 *
 * @return success return security group type else return -1
 */
int ucloud_to_sg_type(const char *type);

/**
 * @brief convert security group type to string format
 *
 * @param type security group type number format
 *
 * @return success return security group type else return NULL
 */
const char *ucloud_from_sg_type(int type);
#endif
