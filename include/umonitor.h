#ifndef __UCLOUD_UMONITOR_H
#define __UCLOUD_UMONITOR_H

#include <uinclude.h>
#include <uparams.h>

/**
 * @file umonitor.h
 * @brief encapsulate umon api
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */


/**
 * @brief uhost monitor metric
 */
typedef enum
{
	uhm_NetworkIn            = 0x01,
	uhm_NetworkOut           = 0x02,
	uhm_CPUUtilization       = 0x04,
	uhm_IORead               = 0x08,
	uhm_IOWrite              = 0x10,
	uhm_DiskReadOps          = 0x20,
	uhm_NICIn                = 0x40,
	uhm_NICOut               = 0x80,
	uhm_MemUsage             = 0x100,
	uhm_DataSpaceUsage       = 0x200,
	uhm_RootSpaceUsage       = 0x400,
	uhm_ReadonlyDiskCount    = 0x800,
	uhm_RunnableProcessCount = 0x1000,
	uhm_BlockProcessCount    = 0x2000
}ucloud_umon_uhost_metric_t;

/**
 * @brief udb monitor metric
 */
typedef enum
{
	udm_CPUUtilization       = 0x01,
	udm_MemUsage             = 0x02,
	udm_QPS                  = 0x04,
	udm_ExpensiveQuery       = 0x08
}ucloud_umon_udb_metric_t;

/**
 * @brief ulb monitor metric
 */
typedef enum
{
	ulm_NetworkOut           = 0x01,
	ulm_TotalNetworkOut      = 0x02,
	ulm_CurrentConnections   = 0x04
}ucloud_umon_ulb_metric_t;

/**
 * @brief umem monitor metric
 */
typedef enum
{
	umm_Usage                = 0x01,
	umm_QPS                  = 0x02,
	umm_InstanceCount        = 0x04
}ucloud_umon_umem_metric_t;


/**
 * @brief definition of metric item
 */
typedef struct
{
	uint64_t timestamp:48;  /*!< timestamp of metric */
	uint64_t pad:16;
	double value;           /*!< metric value */
	char *other;            /*!< store extra data. not used */
}ucloud_umon_metric_item_t;


/**
 * @brief monitor result definition
 */
typedef struct
{
	int res_metric;                      /*!< type of metric resource */
	int nmetric;                         /*!< number of metric items */
	ucloud_umon_metric_item_t *metrics;  /*!< metric items */
}ucloud_umon_result_t;

/**
 * @brief definition of umon
 */
typedef struct
{
	uint64_t res_type:4;                 /*!< resource type. reference ucloud_resource_type_t enum */
	uint64_t pad:60;                     /*!< pad */
	uint64_t res_metric;                 /*!< type of metric resource. uhost, udb, ulb, umem */
	uint64_t time_range;                 /*!< time range of metric */
	uint64_t begin_time;                 /*!< begin time of metric */
	uint64_t end_time;                   /*!< end time of metric */
	char *region;                        /*!< region */
	char *resource_id;                   /*!< resource id */
	int nresult;                         /*!< number of results */
	ucloud_umon_result_t *results;       /*!< result items */
}ucloud_umon_t;

/**
 * @brief initialize umon structure
 *
 * @return success return umon structure else return NULL
 */
ucloud_umon_t* ucloud_umon_init();

/**
 * @brief deinitialize umon structure
 *
 * @param umon umon initialized by ucloud_umon_init()
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_umon_deinit(ucloud_umon_t *umon);

/**
 * @brief get metric
 *
 * @param umon metric paramter
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_umon_get_metric(ucloud_umon_t *umon);

/**
 * @brief get metric name by value
 *
 * @param res_type resource type. reference ucloud_resource_type_t enum
 * @param metric metric metric enum value
 *
 * @return metric name specified by resource type and metric
 */
const char *ucloud_umon_get_metric_name(int res_type, int metric);

#endif
