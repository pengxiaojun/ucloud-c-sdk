#include <umonitor.h>
#include <ucommon.h>
#include <uhttp.h>

#define METRIC_PARAM_PREFIX  "MetricName."

typedef struct
{
	char name[32];
	int value;
}ucloud_umon_metric_pair_t;

static const ucloud_umon_metric_pair_t uhost_metric[] = 
{
	{"NetworkIn", uhm_NetworkIn},
	{"NetworkOut", uhm_NetworkOut},
	{"CPUUtilization", uhm_CPUUtilization},
	{"IORead", uhm_IORead},
	{"IOWrite", uhm_IOWrite},
	{"DiskReadOps", uhm_DiskReadOps},
	{"NICIn", uhm_NICIn},
	{"NICOut", uhm_NICOut},
	{"MemUsage", uhm_MemUsage},
	{"DataSpaceUsage", uhm_DataSpaceUsage},
	{"RootSpaceUsage", uhm_RootSpaceUsage},
	{"ReadonlyDiskCount", uhm_ReadonlyDiskCount},
	{"RunnableProcessCount", uhm_RunnableProcessCount},
	{"BlockProcessCount", uhm_BlockProcessCount}
};

static const ucloud_umon_metric_pair_t udb_metric[] =
{
	{"CPUUtilization", udm_CPUUtilization},
	{"MemUsage", udm_MemUsage},
	{"QPS", udm_QPS},
	{"ExpensiveQuery", udm_ExpensiveQuery}
};

static const ucloud_umon_metric_pair_t ulb_metric[] =
{
	{"NetworkOut", ulm_NetworkOut},
	{"TotalNetworkOut", ulm_TotalNetworkOut},
	{"CurrentConnections", ulm_CurrentConnections}
};

static const ucloud_umon_metric_pair_t umem_metric[] =
{
	{"Usage", umm_Usage},
	{"QPS", umm_QPS},
	{"InstanceCount", umm_InstanceCount},
};

static int ucloud_umon_set_metric_param(ucloud_http_params_t *param, const ucloud_umon_t *umon)
{
	char metric_param_name[32] = {0};
	int number_of_metric;
	int i;

	number_of_metric = 0;
	if (umon->res_type == urt_uhost)
	{
		for (i = 0; i<sizeof(uhost_metric)/sizeof(uhost_metric[0]); ++i)
		{
			if (umon->res_metric & uhost_metric[i].value)
			{
				sprintf(metric_param_name, "%s%d", METRIC_PARAM_PREFIX, number_of_metric++);
				ucloud_http_params_add(param, metric_param_name, uhost_metric[i].name);
			}
		}
	}
	else if (umon->res_type == urt_udb)
	{
		for (i = 0; i<sizeof(udb_metric)/sizeof(udb_metric[0]); ++i)
		{
			if (umon->res_metric & udb_metric[i].value)
			{
				sprintf(metric_param_name, "%s%d", METRIC_PARAM_PREFIX, number_of_metric++);
				ucloud_http_params_add(param, metric_param_name, udb_metric[i].name);
			}
		}
	}
	else if (umon->res_type == urt_ulb)
	{
		for (i = 0; i<sizeof(ulb_metric)/sizeof(ulb_metric[0]); ++i)
		{
			if (umon->res_metric & ulb_metric[i].value)
			{
				sprintf(metric_param_name, "%s%d", METRIC_PARAM_PREFIX, number_of_metric++);
				ucloud_http_params_add(param, metric_param_name, ulb_metric[i].name);
			}
		}
	}
	else if (umon->res_type == urt_umem)
	{
		for (i = 0; i<sizeof(umem_metric)/sizeof(umem_metric[0]); ++i)
		{
			if (umon->res_metric & umem_metric[i].value)
			{
				sprintf(metric_param_name, "%s%d", METRIC_PARAM_PREFIX, number_of_metric++);
				ucloud_http_params_add(param, metric_param_name, umem_metric[i].name);
			}
		}
	}
	return number_of_metric;
}

static int ucloud_umon_parse_metric_item(json_object *jobj, const ucloud_umon_metric_pair_t *metrics, int nmetric, ucloud_umon_t *umon)
{
	int i,j;
	int cur_result = 0;

	for (i = 0; i<nmetric; ++i)
	{
		if (umon->res_metric & metrics[i].value)
		{
			json_object *jval;
			if (json_object_object_get_ex(jobj, uhost_metric[i].name, &jval) &&
				json_object_get_type(jval) == json_type_array)
			{
				int count = json_object_array_length(jval);
				ucloud_umon_result_t *result = &umon->results[cur_result++];
				result->res_metric = metrics[i].value;
				result->nmetric = count;
				result->metrics = (ucloud_umon_metric_item_t*)calloc(count, sizeof(ucloud_umon_metric_item_t));

				for (j = 0; j<count; ++j)
				{
					ucloud_umon_metric_item_t *metric = &result->metrics[j];
					json_object *jitem = json_object_array_get_idx(jval, j);

					json_object *jitemval;
					if (json_object_object_get_ex(jitem, "Value", &jitemval) &&
						json_object_get_type(jitemval) == json_type_int)
					{
						metric->value = json_object_get_double(jitemval);
					}
					if (json_object_object_get_ex(jitem, "Timestamp", &jitemval) &&
						json_object_get_type(jitemval) == json_type_int)
					{
						metric->timestamp = json_object_get_int(jitemval);
					}
				}
			}
		}
	}
	return 0;
}

ucloud_umon_t* ucloud_umon_init()
{
	ucloud_umon_t *umon;
	umon = (ucloud_umon_t*)calloc(1, sizeof(*umon));
	if (umon == NULL)
	{
		return NULL;
	}
	return umon;
}

int ucloud_umon_deinit(ucloud_umon_t *umon)
{
	if (umon == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}
	if (umon->region) free(umon->region);
	if (umon->resource_id) free(umon->resource_id);
	if (umon->results) free(umon->results);
	free(umon);
	return UCLOUDE_OK;
}

int ucloud_umon_get_metric(ucloud_umon_t *umon)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (umon->region == NULL || umon->resource_id == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_add(param, "Action", "GetMetric");
	ucloud_http_params_add(param, "Region", umon->region);
	ucloud_http_params_add(param, "ResourceType", ucloud_from_resource_type(umon->res_type));
	ucloud_http_params_add(param, "ResourceId", umon->resource_id);
	umon->nresult = ucloud_umon_set_metric_param(param, umon);

	ucloud_uhttp_response_t *resp;
	resp = ucloud_uhttp_response_init();
	int ret = ucloud_uhttp_request(param, resp);
	if (ret != UCLOUDE_OK)
	{
		goto error;
	}

	//handle response
	json_object *jobj = json_tokener_parse(resp->result);
	ret = ucloud_uhttp_handle_resp_header(jobj);

	if (ret != 0)
	{
		goto error;
	}

	umon->results = (ucloud_umon_result_t*)calloc(umon->nresult, sizeof(*umon->results));

	json_object *jval;
	if (json_object_object_get_ex(jobj, "DataSets", &jval) &&
		json_object_get_type(jval) == json_type_object)
	{
		if (umon->res_type == urt_uhost)
		{
			ucloud_umon_parse_metric_item(jval, uhost_metric, sizeof(uhost_metric)/sizeof(uhost_metric[0]), umon);
		}
		else if (umon->res_type == urt_udb)
		{
			ucloud_umon_parse_metric_item(jval, udb_metric, sizeof(uhost_metric)/sizeof(udb_metric[0]), umon);
		}
		else if (umon->res_type == urt_ulb)
		{
			ucloud_umon_parse_metric_item(jval, ulb_metric, sizeof(ulb_metric)/sizeof(ulb_metric[0]), umon);
		}
		else if (umon->res_type == urt_umem)
		{
			ucloud_umon_parse_metric_item(jval, umem_metric, sizeof(umem_metric)/sizeof(umem_metric[0]), umon);
		}
	}
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

const char *ucloud_umon_get_metric_name(int res_type, int metric)
{
	int i;
	if (res_type == urt_uhost)
	{
		for (i = 0; i < sizeof(uhost_metric)/sizeof(uhost_metric[0]); ++i)
		{
			if (uhost_metric[i].value == metric)
				return uhost_metric[i].name;
		}
	}
	else if (res_type == urt_udb)
	{
		for (i = 0; i<sizeof(udb_metric)/sizeof(udb_metric[0]); ++i)
		{
			if (udb_metric[i].value == metric)
				return udb_metric[i].name;
		}
	}
	else if (res_type == urt_ulb)
	{
		for (i = 0; i<sizeof(ulb_metric)/sizeof(ulb_metric[0]); ++i)
		{
			if (ulb_metric[i].value == metric)
				return ulb_metric[i].name;
		}
	}
	else if (res_type == urt_umem)
	{
		for (i = 0; i<sizeof(umem_metric)/sizeof(umem_metric[0]); ++i)
		{
			if (umem_metric[i].value == metric)
				return umem_metric[i].name;
		}
	}
	return NULL;
}
