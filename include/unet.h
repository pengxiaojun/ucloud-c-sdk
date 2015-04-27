#ifndef __UCLOUD_UNET_H
#define __UCLOUD_UNET_H

#include <uinclude.h>
#include <ucommon.h>

/**
 * @file unet.h
 * @brief encapsulate unet api
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */


/**
 * @brief bind resource definition
 */
typedef struct ucloud_unet_resource_t
{
	char *type;                            /*!< bind resource type. reference ucloud_resource_type_t enum */
	char *name;                            /*!< resource name */
	char *id;                              /*!< resouce id */
	struct ucloud_unet_resource_t *next;   /*!< point to next resource */
}ucloud_unet_resource_t;

/**
 * @brief eip addresss definition
 */
typedef struct ucloud_unet_eip_addr_t
{
	int type;                              /*!< operator name. see ucloud_ip_type_t */
	char *ip;                              /*!< ip address */
	struct ucloud_unet_eip_addr_t *next;   /*!< next eip address */
}ucloud_unet_eip_addr_t;

/**
 * @brief eip definition
 */
typedef struct ucloud_unet_eip_t
{
	char *id;                              /*!< eip id */
	char *name;                            /*!< eip name */
	char *tag;                             /*!< eip tag */
	char *remark;                          /*!< eip remark */
	int weight;                            /*!< eip weigth,[0-100] */
	int bandwidth_type;                    /*!< reference ucloud_bandwidth_type_t */
	int bandwidth;                         /*!< bandwidth [0-800]MB*/
	int status;                            /*!< eip status. reference ucloud_ip_status_t */
	int charge_type;                       /*!< charge type.reference ucloud_charge_type_t */
	int create_time;                       /*!< eip created time. Unix timestamp */
	int expired_time;                      /*!< eip expired time. Unix timestamp */
	ucloud_unet_resource_t *res;           /*!< eip resouces */
	ucloud_unet_eip_addr_t *addr;          /*!< eip addresses */
	struct ucloud_unet_eip_t *next;        /*!< next eip structure */
}ucloud_unet_eip_t;

/**
 * @brief allocate eip request parameter
 */
typedef struct
{
	char region[MAX_REGION_LEN];           /*!< region */
	int operator_name;                     /*!< operator name. reference ucloud_ip_type_t enum */
	int bandwidth;                         /*!< bandwidth [0-800]M */
	int charge_type;                       /*!< charge type. reference ucloud_charge_type_t enum */
	int quantity;                          /*!< count of eip */
}ucloud_unet_allocate_eip_param_t;

/**
 * @brief eip price definition
 */
typedef struct ucloud_unet_eip_price_t
{
	int charge_type;                       /*!< charget type. reference ucloud_charge_type_t enum  */
	double price;                          /*!< eip price */
	int purchase_value;                    /*!< purchase value */
	struct ucloud_unet_eip_price_t *next;  /*!< next eip price */
}ucloud_unet_eip_price_t;

/**
 * @brief unet vip definition
 */
typedef struct ucloud_unet_vip_t
{
	char *ip;                              /*!< ip */
	struct ucloud_unet_vip_t *next;        /*!< next vip */
}ucloud_unet_vip_t;


/**
 * @brief security group definition
 */
typedef struct ucloud_unet_security_rule_t
{
	char *src_ip;                             /*!< source ip */
	char *dst_port;                           /*!< destination port */
	int prot_type;                            /*!< protocol type. reference ucloud_prot_type_t enum */
	int priority;                             /*!< priority, value: [50, 100, 150] from high to low */
	int action;                               /*!< action. reference ucloud_sg_rule_action_t */
	struct ucloud_unet_security_rule_t *next; /*!< next rule */
}ucloud_unet_security_rule_t;

/**
 * @brief security group resource
 */
typedef struct ucloud_unet_sg_resource_t
{
	char *ip;                                /*!< resource ip */
	struct ucloud_unet_sg_resource_t *next;  /*!< next resouce */
}ucloud_unet_sg_resource_t;

/**
 * @brief security group definition
 */
typedef struct ucloud_unet_sg_t
{
	int type;                           /*!< security group type */
	int ctime;                          /*!< created time */
	int id;                             /*!< security group id */
	char *name;                         /*!< security group name */
	char *desc;                         /*!< security group description */
	ucloud_unet_security_rule_t *rule;  /*!< security rules */
	struct ucloud_unet_sg_t *next;      /*!< next security group */
}ucloud_unet_sg_t;

/**
 * @brief eip structure constructor
 *
 * @return  success return eip structure else return NULL
 */
ucloud_unet_eip_t* ucloud_unet_eip_init();

/**
 * @brief  eip deconstructor
 *
 * @param eip eip structure will be destructored
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_eip_deinit(ucloud_unet_eip_t *eip);

/**
 * @brief eip address constructor
 *
 * @return 
 */
ucloud_unet_eip_addr_t* ucloud_unet_eip_addr_init();

/**
 * @brief eip address deconstructor
 *
 * @param eip
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_eip_addr_deinit(ucloud_unet_eip_addr_t *eip);

/**
 * @brief destructor eip set
 *
 * @param eipset
 *
 * @return 
 */
int ucloud_unet_eipset_deinit(ucloud_unet_eip_t *eipset);

/**
 * @brief unet resource constructor
 *
 * @return 
 */
ucloud_unet_resource_t* ucloud_unet_resource_init();

/**
 * @brief unet resource deconsructor
 *
 * @param res
 *
 * @return 
 */
int ucloud_unet_resource_deinit(ucloud_unet_resource_t *res);

/**
 * @brief eip price construtor
 *
 * @return 
 */
ucloud_unet_eip_price_t* ucloud_unet_eip_price_init();

/**
 * @brief eip price deconstructor
 *
 * @param price
 *
 * @return 
 */
int ucloud_unet_eip_price_deinit(ucloud_unet_eip_price_t *price);

/**
 * @brief eip price set deconsructor
 *
 * @param priceset
 *
 * @return 
 */
int ucloud_unet_eip_priceset_deinit(ucloud_unet_eip_price_t *priceset);

/**
 * @brief allocate eip
 *
 * @param param allocate eip parameter
 *
 * @return success return allocated eip else return NULL. returned eip should be free by invoke eip deconstructor
 */
ucloud_unet_eip_t* ucloud_unet_allocate_eip(const ucloud_unet_allocate_eip_param_t *param);

/**
 * @brief describe eip
 *
 * @param region region
 *
 * @return success return described eip else return NULL. returned eip should be free by invoke eip deconstructor
 */
ucloud_unet_eip_t* ucloud_unet_describe_eip(const char *region);

/**
 * @brief update eip attribute
 *
 * @param region region
 * @param eipid eip id
 * @param name eip name
 * @param tag eip tag
 * @param remark eip remark
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_update_eip_attr(const char *region, const char *eipid, const char *name, const char *tag, const char *remark);

/**
 * @brief release eip
 *
 * @param region region
 * @param eipid eip id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_release_eip(const char *region, const char *eipid);

/**
 * @brief bind eip
 *
 * @param region region
 * @param eipid eip id
 * @param res resouce type(reference ucloud_resource_type_t enum)
 * @param resid resource id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_bind_eip(const char *region, const char *eipid, int res, const char *resid);

/**
 * @brief unbind eip
 *
 * @param region region
 * @param eipid eip id
 * @param res resouce type(reference ucloud_resource_type_t enum)
 * @param resid
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_unbind_eip(const char *region, const char *eipid, int res, const char *resid);

/**
 * @brief modify eip bandwidth
 *
 * @param region region
 * @param eipid eip id
 * @param bandwidth new bandwith value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_modify_eip_bandwidth(const char *region, const char *eipid, int bandwidth);

/**
 * @brief modify eip weight
 *
 * @param region region
 * @param eipid eip id
 * @param weight new weight value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_modify_eip_weight(const char *region, const char *eipid, int weight);

/**
 * @brief  get eip price
 *
 * @param region region
 * @param oper_type operator type. reference ucloud_ip_type_t enum
 * @param bandwidth bandwidth
 * @param charge charge type
 *
 * @return success return eip price set should be free by eip price deconstructor else return NULL
 */
ucloud_unet_eip_price_t* ucloud_unet_get_eip_price(const char *region, int oper_type, int bandwidth, int charge);

/**
 * @brief vip constructor
 *
 * @return 
 */
ucloud_unet_vip_t* ucloud_unet_vip_init();

/**
 * @brief vip deconstructor
 *
 * @param vip
 *
 * @return 
 */
int ucloud_unet_vip_deinit(ucloud_unet_vip_t *vip);

/**
 * @brief allocate vip
 *
 * @param region region
 * @param count count of vip.default is 1
 *
 * @return success return allocated vip should be free by vip deconstructor else return NULL
 */
ucloud_unet_vip_t* ucloud_unet_allocate_vip(const char *region, int count);

/**
 * @brief describe eip
 *
 * @param region region
 *
 * @return success return described vip should be free by vip deconstructor else return NULL
 */

ucloud_unet_vip_t* ucloud_unet_describe_vip(const char *region);

/**
 * @brief release vip
 *
 * @param region region
 * @param vip vip id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_release_vip(const char *region, const char *vip);

/**
 * @brief security group constructor
 *
 * @return 
 */
ucloud_unet_sg_t* ucloud_unet_sg_init();

/**
 * @brief security group deconstructor
 *
 * @param sg
 *
 * @return 
 */
int ucloud_unet_sg_deinit(ucloud_unet_sg_t *sg);

/**
 * @brief security rule constructor
 *
 * @return 
 */
ucloud_unet_security_rule_t* ucloud_unet_security_rule_init();

/**
 * @brief security rule deconstructor
 *
 * @param rule
 *
 * @return 
 */
int ucloud_unet_security_rule_deinit(ucloud_unet_security_rule_t *rule);

/**
 * @brief security group resoure constructor
 *
 * @return 
 */
ucloud_unet_sg_resource_t* ucloud_unet_sg_resource_init();

/**
 * @brief security group resouce deconstructor
 *
 * @param res
 *
 * @return 
 */
int ucloud_unet_sg_resource_deinit(ucloud_unet_sg_resource_t *res);

/**
 * @brief create security group. name,description and rule is required
 *
 * @param region region
 * @param sg parameter to create security group(such as security name, description and rules)
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_create_sg(const char *region, const ucloud_unet_sg_t *sg);


/**
 * @brief describe security group
 *
 * @param region region
 * @param res_type resource type. reference ucloud_resource_type_t enum
 * @param res_id resouce id
 * @param group_id security group id. fill 0 to describe all security group
 *
 * @return success return security group set should be free by security group deconstructor else return NULL
 */
ucloud_unet_sg_t* ucloud_unet_describe_sg(const char *region, int res_type, int res_id, int group_id);

/**
 * @brief describe security group resouce
 *
 * @param region region
 * @param group_id security group id
 *
 * @return success return security group resouce should be free by security group resouce deconstructor else return NULL
 */
ucloud_unet_sg_resource_t* ucloud_unet_describe_sg_resource(const char *region, int group_id);

/**
 * @brief update security group
 *
 * @param region region
 * @param sg parameter to update security group(such as security name, description and rules)
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_update_sg(const char *region, const ucloud_unet_sg_t *sg);

/**
 * @brief delete security group
 *
 * @param region region
 * @param group_id security group id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_delete_sg(const char *region, int group_id);

/**
 * @brief grant securitu group to specified resouce
 *
 * @param region region
 * @param group_id group id
 * @param res_type resouce type. reference ucloud_resource_type_t enum
 * @param res_id resouce id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_unet_grant_sg(const char *region, int group_id, int res_type, const char *res_id);
#endif
