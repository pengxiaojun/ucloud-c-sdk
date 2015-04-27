#include <unet.h>
#include <uparams.h>
#include <uhttp.h>
#include <uinclude.h>

/* construct/deconstructor for eip */
ucloud_unet_eip_t* ucloud_unet_eip_init()
{
	ucloud_unet_eip_t *eip;
	eip = (ucloud_unet_eip_t*)calloc(1, sizeof(*eip));
	if (eip == NULL)
	{
		return NULL;
	}
	return eip;
}

int ucloud_unet_eip_deinit(ucloud_unet_eip_t *eip)
{
	if (eip == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (eip->id) free(eip->id);
	if (eip->name) free(eip->name);
	if (eip->tag) free(eip->tag);
	if (eip->remark) free(eip->remark);

	//free resource
	ucloud_unet_resource_t *res;
	while (eip->res != NULL)
	{
		res = eip->res;
		eip->res = res->next;
		ucloud_unet_resource_deinit(res);
	}
	//free address
	ucloud_unet_eip_addr_t *addr;
	while (eip->addr != NULL)
	{
		addr = eip->addr;
		eip->addr = addr->next;
		ucloud_unet_eip_addr_deinit(addr);
	}
	free(eip);
	return UCLOUDE_OK;
}

int ucloud_unet_eipset_deinit(ucloud_unet_eip_t *eipset)
{
	ucloud_unet_eip_t *eip;
	while (eipset != NULL)
	{
		eip = eipset;
		eipset = eipset->next;
		ucloud_unet_eip_deinit(eip);
	}
	return UCLOUDE_OK;
}

/* construct/deconstructor for eip address */
ucloud_unet_eip_addr_t* ucloud_unet_eip_addr_init()
{
	ucloud_unet_eip_addr_t *addr;
	addr = (ucloud_unet_eip_addr_t*)calloc(1, sizeof(*addr));
	if (addr == NULL)
	{
		return NULL;
	}
	return addr;
}

int ucloud_unet_eip_addr_deinit(ucloud_unet_eip_addr_t *addr)
{
	if (addr == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (addr->ip) free(addr->ip);
	free(addr);
	return UCLOUDE_OK;
}

/* construct/deconstructor for resource*/
ucloud_unet_resource_t* ucloud_unet_resource_init()
{
	ucloud_unet_resource_t *res;
	res = (ucloud_unet_resource_t*)calloc(1, sizeof(*res));
	if (res == NULL)
	{
		return NULL;
	}
	return res;
}

int ucloud_unet_resource_deinit(ucloud_unet_resource_t *res)
{
	if (res == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (res->id) free(res->id);
	if (res->name) free(res->name);
	if (res->type) free(res->type);
	free(res);
	return UCLOUDE_OK;
}

/* construct/deconstructor for eip price */
ucloud_unet_eip_price_t* ucloud_unet_eip_price_init()
{
	ucloud_unet_eip_price_t *price;
	price = (ucloud_unet_eip_price_t*)calloc(1, sizeof(*price));
	if (price == NULL)
	{
		return NULL;
	}
	return price;
}

int ucloud_unet_eip_price_deinit(ucloud_unet_eip_price_t *price)
{
	if (price == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	free(price);
	return UCLOUDE_OK;
}

int ucloud_unet_eip_priceset_deinit(ucloud_unet_eip_price_t *priceset)
{
	if (priceset == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_unet_eip_price_t *price;
	while (priceset)
	{
		price = priceset;
		priceset = priceset->next;	
		ucloud_unet_eip_price_deinit(price);
	}
	return UCLOUDE_OK;
}

ucloud_unet_eip_t* ucloud_unet_allocate_eip(const ucloud_unet_allocate_eip_param_t *reqparam)
{
	ucloud_unet_eip_t *head_eip = NULL;

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL || reqparam == NULL)
	{
		ucloud_set_last_error("Invalid parameter");
		return NULL;
	}

	if (reqparam->region == NULL)
	{
		ucloud_set_last_error("Invalid parameter region");
		return NULL;
	}
	ucloud_http_params_add(param, "Action", "AllocateEIP");
	ucloud_http_params_add(param, "Region", reqparam->region);
	ucloud_http_params_add(param, "OperatorName", ucloud_from_operator_name(reqparam->operator_name));
	ucloud_http_params_add_int(param, "Bandwidth", reqparam->bandwidth);

	if (reqparam->charge_type > 0)
	{
		ucloud_http_params_add(param, "ChargeType", ucloud_from_charge_type(reqparam->bandwidth));
	}
	if (reqparam->quantity > 0)
	{
		ucloud_http_params_add_int(param, "Quantity", reqparam->quantity);
	}

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
	//parse eip set
	json_object *jval;
	if (json_object_object_get_ex(jobj, "EIPSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		//parse eip
		for (i = 0; i<count; ++i)
		{
			json_object *jitem = json_object_array_get_idx(jval, i);	
			ucloud_unet_eip_t *eip;
			eip = ucloud_unet_eip_init();

			json_object *jitemval;
			//get host id
			if (json_object_object_get_ex(jitem, "EIPId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->id = strdup(json_object_get_string(jitemval));
            }
			//get ip address set
			if (json_object_object_get_ex(jitem, "EIPAddr", &jitemval) &&
                json_object_get_type(jitemval) == json_type_array)
            {
				int addr_cnt = json_object_array_length(jitemval);
				int j;
				for (j = 0; j<addr_cnt; ++j)
				{
					ucloud_unet_eip_addr_t *addr;
					addr = ucloud_unet_eip_addr_init();

					json_object *jip = json_object_array_get_idx(jitemval, j);
					json_object *jipval;
					//parse ip
					if (json_object_object_get_ex(jip, "OperatorName", &jipval) &&
						json_object_get_type(jipval) == json_type_string)
					{
						addr->type = ucloud_to_ip_type(json_object_get_string(jipval));
					}
					//parse operator name
					if (json_object_object_get_ex(jip, "IP", &jipval) &&
						json_object_get_type(jipval) == json_type_string)
					{
						addr->ip = strdup(json_object_get_string(jipval));
					}
					addr->next = eip->addr;
					eip->addr = addr;
				}
            }
			eip->next = head_eip;
			head_eip = eip;
		}
	}

	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_eip;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

ucloud_unet_eip_t* ucloud_unet_describe_eip(const char *region)
{
	ucloud_unet_eip_t *head_eip = NULL;	
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL || region == NULL)
	{
		ucloud_set_last_error("Invalid parameter");
		return NULL;
	}
	ucloud_http_params_add(param, "Action", "DescribeEIP");
	ucloud_http_params_add(param, "Region", region);
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

	json_object *jval;
	//response do not contain TotalCount element
	/*int total;
	if (json_object_object_get_ex(jobj, "TotalCount", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		total = json_object_get_int(jval);
	}*/
	//parse eip set
	if (json_object_object_get_ex(jobj, "EIPSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		//parse eip
		for (i = 0; i<count; ++i)
		{
			json_object *jitem = json_object_array_get_idx(jval, i);	
			ucloud_unet_eip_t *eip;
			eip = ucloud_unet_eip_init();

			json_object *jitemval;
			//get eip id
			if (json_object_object_get_ex(jitem, "EIPId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->id = strdup(json_object_get_string(jitemval));
            }
			//get weight 
			if (json_object_object_get_ex(jitem, "Weight", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                eip->weight = json_object_get_int(jitemval);
            }
			//get band width type
			if (json_object_object_get_ex(jitem, "BandwidthType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                eip->bandwidth_type = json_object_get_int(jitemval);
            }
			//get bandwidth
			if (json_object_object_get_ex(jitem, "Bandwidth", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                eip->bandwidth = json_object_get_int(jitemval);
            }
			//get ip status
			if (json_object_object_get_ex(jitem, "Status", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->status = ucloud_to_ip_status(json_object_get_string(jitemval));
            }
			//get charge type
			if (json_object_object_get_ex(jitem, "ChargeType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->charge_type = ucloud_to_charge_type(json_object_get_string(jitemval));
            }
			//get create time
			if (json_object_object_get_ex(jitem, "CreateTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                eip->create_time = json_object_get_int(jitemval);
            }
			if (json_object_object_get_ex(jitem, "ExpireTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                eip->expired_time = json_object_get_int(jitemval);
            }
			if (json_object_object_get_ex(jitem, "Name", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->name = strdup(json_object_get_string(jitemval));
            }
			if (json_object_object_get_ex(jitem, "Tag", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->tag = strdup(json_object_get_string(jitemval));
            }
			if (json_object_object_get_ex(jitem, "Remark", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                eip->remark = strdup(json_object_get_string(jitemval));
            }
			//parse resource
			if (json_object_object_get_ex(jitem, "Resource", &jitemval) &&
                json_object_get_type(jitemval) == json_type_array)
            {
				int res_cnt = json_object_array_length(jitemval);
				int j;
				for (j = 0; j<res_cnt; ++j)
				{
					ucloud_unet_resource_t *res;
					res = ucloud_unet_resource_init();

					json_object *jres = json_object_array_get_idx(jitemval, j);
					json_object *jresval;
					//parse resource type
					if (json_object_object_get_ex(jres, "ResourceType", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						res->type = strdup(json_object_get_string(jresval));
					}
					//parse resource name
					if (json_object_object_get_ex(jres, "ResourceName", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						res->name = strdup(json_object_get_string(jresval));
					}
					//parse resource id
					if (json_object_object_get_ex(jres, "ResourceName", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						res->id = strdup(json_object_get_string(jresval));
					}
					res->next = eip->res;
					eip->res = res;
				}
            }
			//parse ip address
			if (json_object_object_get_ex(jitem, "EIPAddr", &jitemval) &&
                json_object_get_type(jitemval) == json_type_array)
            {
				int addr_cnt = json_object_array_length(jitemval);
				int j;
				for (j = 0; j<addr_cnt; ++j)
				{
					ucloud_unet_eip_addr_t *addr;
					addr = ucloud_unet_eip_addr_init();

					json_object *jip = json_object_array_get_idx(jitemval, j);
					json_object *jipval;
					//parse ip
					if (json_object_object_get_ex(jip, "OperatorName", &jipval) &&
						json_object_get_type(jipval) == json_type_string)
					{
						addr->type = ucloud_to_ip_type(json_object_get_string(jipval));
					}
					//parse operator name
					if (json_object_object_get_ex(jip, "IP", &jipval) &&
						json_object_get_type(jipval) == json_type_string)
					{
						addr->ip = strdup(json_object_get_string(jipval));
					}
					addr->next = eip->addr;
					eip->addr = addr;
				}
            }
			eip->next = head_eip;
			head_eip = eip;
		}
	}

	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_eip;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

int ucloud_unet_update_eip_attr(const char *region, const char *eipid, const char *name, const char *tag, const char *remark)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || eipid == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	if (name == NULL && tag == NULL && remark == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_add(param, "Action", "UpdateEIPAttribute");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "EIPId", eipid);

	if (name)
	{
		ucloud_http_params_add(param, "Name", name);
	}
	if (tag)
	{
		ucloud_http_params_add(param, "Tag", tag);
	}
	if (remark)
	{
		ucloud_http_params_add(param, "Remark", remark);
	}

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_unet_release_eip(const char *region, const char *eipid)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || eipid == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_add(param, "Action", "ReleaseEIP");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "EIPId", eipid);

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

static int ucloud_unet_bindunbind_eip(bool bind, const char *region, const char *eipid, int res, const char *resid)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || eipid == NULL || resid == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", bind ? "BindEIP" : "UnBindEIP");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "EIPId", eipid);
	ucloud_http_params_add(param, "ResourceType", ucloud_from_resource_type(res));
	ucloud_http_params_add(param, "ResourceId", resid);

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_unet_bind_eip(const char *region, const char *eipid, int res, const char *resid)
{
	return ucloud_unet_bindunbind_eip(true, region, eipid, res, resid);
}

int ucloud_unet_unbind_eip(const char *region, const char *eipid, int res, const char *resid)
{
	return ucloud_unet_bindunbind_eip(false, region, eipid, res, resid);
}

int ucloud_unet_modify_eip_bandwidth(const char *region, const char *eipid, int bandwidth)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || eipid == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "ModifyEIPBandwidth");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "EIPId", eipid);
	ucloud_http_params_add_int(param, "Bandwidth", bandwidth);

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_unet_modify_eip_weight(const char *region, const char *eipid, int weight)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || eipid == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "ModifyEIPWeight");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "EIPId", eipid);
	ucloud_http_params_add_int(param, "Weight", weight);

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

ucloud_unet_eip_price_t* ucloud_unet_get_eip_price(const char *region, int oper_type, int bandwidth, int charge_type)
{
	ucloud_unet_eip_price_t *head_price = NULL;
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return NULL;
	}
	if (region == NULL)
	{
		return NULL;
	}
	ucloud_http_params_add(param, "Action", "GetEIPPrice");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "OperatorName", ucloud_from_operator_name(oper_type));
	ucloud_http_params_add_int(param, "Bandwidth", bandwidth);
	ucloud_http_params_add(param, "ChargeType", ucloud_from_charge_type(charge_type));

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
	json_object *jval;
	//parse ip address
	if (json_object_object_get_ex(jobj, "PriceSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		for (i = 0; i<count; ++i)
		{
			ucloud_unet_eip_price_t *price;
			price = ucloud_unet_eip_price_init();

			json_object *jprice = json_object_array_get_idx(jval, i);
			json_object *jpriceval;
			if (json_object_object_get_ex(jprice, "ChargeType", &jpriceval) &&
				json_object_get_type(jpriceval) == json_type_string)
			{
				price->charge_type = ucloud_to_charge_type(json_object_get_string(jpriceval));
			}
			if (json_object_object_get_ex(jprice, "Price", &jpriceval))
			{
				if (json_object_get_type(jpriceval) == json_type_double)
					price->price = json_object_get_double(jpriceval);
				else if (json_object_get_type(jpriceval) == json_type_int)
					price->price = json_object_get_int(jpriceval);
			}
			if (json_object_object_get_ex(jprice, "PurchaseValue", &jpriceval) &&
				json_object_get_type(jpriceval) == json_type_int)
			{
				price->purchase_value = json_object_get_double(jpriceval);
			}
			price->next = head_price;
			head_price = price;
		}
	}
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_price;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

ucloud_unet_vip_t* ucloud_unet_vip_init()
{
	ucloud_unet_vip_t *vip;
	vip = (ucloud_unet_vip_t*)calloc(1, sizeof(*vip));
	if (vip == NULL)
	{
		return NULL;
	}
	return vip;
}

int ucloud_unet_vip_deinit(ucloud_unet_vip_t *vip)
{
	if (vip == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_unet_vip_t *head;
	while (vip)
	{
		head = vip;
		vip = vip->next;
		free(head->ip);
		free(head);
	}
	return UCLOUDE_OK;
}

ucloud_unet_vip_t* ucloud_unet_allocate_vip(const char *region, int count)
{
	ucloud_unet_vip_t *head_vip = NULL;
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return NULL;
	}
	if (region == NULL)
	{
		return NULL;
	}
	ucloud_http_params_add(param, "Action", count == -1 ? "DescribeVIP" : "AllocateVIP"); //XXX: hack
	ucloud_http_params_add(param, "Region", region);
	if (count > 0)
	{
		ucloud_http_params_add_int(param, "Count", count);
	}

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
	json_object *jval;
	//parse ip address
	if (json_object_object_get_ex(jobj, "DataSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		for (i = 0; i<count; ++i)
		{
			ucloud_unet_vip_t *vip;
			vip = ucloud_unet_vip_init();
			json_object *jprice = json_object_array_get_idx(jval, i);
			vip->ip = strdup(json_object_get_string(jprice));
			vip->next = head_vip;
			head_vip = vip;
		}
	}
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_vip;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

ucloud_unet_vip_t* ucloud_unet_describe_vip(const char *region)
{
	return ucloud_unet_allocate_vip(region, -1); //XXX: -1 represet perform describe vip
}

int ucloud_unet_release_vip(const char *region, const char *vip)
{
	if (region == NULL || vip == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","ReleaseVIP");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "VIP", vip);

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

ucloud_unet_sg_t* ucloud_unet_sg_init()
{
	ucloud_unet_sg_t *sg;
	sg = (ucloud_unet_sg_t*)calloc(1, sizeof(*sg));
	if (sg == NULL)
	{
		ucloud_set_last_error("allocate memory failure");
	}
	return sg;
}

int ucloud_unet_sg_deinit(ucloud_unet_sg_t *sg)
{
	if (sg == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_unet_sg_t *head;
	while (sg)
	{
		head = sg;
		sg = sg->next;

		if (head->name) free(head->name);
		if (head->desc) free(head->desc);
		ucloud_unet_security_rule_deinit(head->rule);
		free(head);
	}
	return UCLOUDE_OK;
}

ucloud_unet_security_rule_t* ucloud_unet_security_rule_init()
{
	ucloud_unet_security_rule_t *rule;
	rule = (ucloud_unet_security_rule_t*)calloc(1, sizeof(*rule));
	if (rule == NULL)
	{
		ucloud_set_last_error("allocate memory failure");
	}
	return rule;
}

int ucloud_unet_security_rule_deinit(ucloud_unet_security_rule_t *rule)
{
	if (rule == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_unet_security_rule_t *head;
	while (rule)
	{
		head = rule;
		rule = rule->next;

		if (head->src_ip) free(head->src_ip);
		if (head->dst_port) free(head->dst_port);
		free(head);
	}
	return UCLOUDE_OK;
}

ucloud_unet_sg_resource_t* ucloud_unet_sg_resource_init()
{
	ucloud_unet_sg_resource_t *res;
	res = (ucloud_unet_sg_resource_t*)calloc(1, sizeof(*res));
	if (res == NULL)
	{
		ucloud_set_last_error("allocate memory failure");
	}
	return res;
}

int ucloud_unet_sg_resource_deinit(ucloud_unet_sg_resource_t *res)
{
	if (res == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_unet_sg_resource_t *head;
	while (res)
	{
		head = res;
		res = res->next;

		if (head->ip) free(head->ip);
		free(head);
	}
	return UCLOUDE_OK;
}

int ucloud_unet_create_sg(const char *region, const ucloud_unet_sg_t *sg)
{
	if (region == NULL || sg == NULL || sg->name == NULL||sg->desc == NULL||sg->rule == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","CreateSecurityGroup");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "GroupName", sg->name);
	ucloud_http_params_add(param, "Description", sg->desc);
	int n = 0;
	ucloud_unet_security_rule_t *rule;
	rule = sg->rule;
	while (rule)
	{
		char rule_str[64] = {0};
		char rule_num[12] = {0};
		sprintf(rule_num, "Rule.%d", n++);
		sprintf(rule_str, "%s|%s|%s|%s|%d", ucloud_from_prot_type(rule->prot_type), rule->dst_port, rule->src_ip, ucloud_from_rule_action(rule->action), rule->priority);
		ucloud_http_params_add(param, rule_num, rule_str);
		rule = rule->next;
	}

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

ucloud_unet_sg_t* ucloud_unet_describe_sg(const char *region, int res_type, int res_id, int group_id)
{
	ucloud_unet_sg_t *head_sg = NULL;
	if (region == NULL)
	{
		ucloud_set_last_error("Invalid parameter region");
		return NULL;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","DescribeSecurityGroup");
	ucloud_http_params_add(param, "Region", region);
	if (res_type > 0)
	{
		ucloud_http_params_add(param, "ResourceType", ucloud_from_resource_type(res_type));
	}
	if (res_id>0)
	{
		ucloud_http_params_add_int(param, "ResourceId", res_id);
	}
	if (group_id>0)
	{
		ucloud_http_params_add_int(param, "GroupId", group_id);
	}
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
	json_object *jval;
	if (json_object_object_get_ex(jobj, "DataSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		//securit group
		for (i = 0; i<count; ++i)
		{
			json_object *jitem = json_object_array_get_idx(jval, i);	
			ucloud_unet_sg_t *sg;
			sg = ucloud_unet_sg_init();

			json_object *jitemval;
			if (json_object_object_get_ex(jitem, "GroupId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                sg->id = json_object_get_int(jitemval);
            }
			if (json_object_object_get_ex(jitem, "GroupName", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                sg->name = strdup(json_object_get_string(jitemval));
            }
			if (json_object_object_get_ex(jitem, "Description", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                sg->desc = strdup(json_object_get_string(jitemval));
            }
			if (json_object_object_get_ex(jitem, "Type", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                sg->type = json_object_get_int(jitemval);
            }
			if (json_object_object_get_ex(jitem, "CreateTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                sg->ctime = json_object_get_int(jitemval);
            }
			if (json_object_object_get_ex(jitem, "Rule", &jitemval) &&
                json_object_get_type(jitemval) == json_type_array)
            {
				int rule_cnt = json_object_array_length(jitemval);
				int j;
				for (j = 0; j<rule_cnt; ++j)
				{
					ucloud_unet_security_rule_t *rule;
					rule = ucloud_unet_security_rule_init();
					json_object *jres = json_object_array_get_idx(jitemval, j);
					json_object *jresval;
					if (json_object_object_get_ex(jres, "SrcIP", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						rule->src_ip = strdup(json_object_get_string(jresval));
					}
					if (json_object_object_get_ex(jres, "Priority", &jresval) &&
						json_object_get_type(jresval) == json_type_int)
					{
						rule->priority = json_object_get_int(jresval);
					}
					if (json_object_object_get_ex(jres, "ProtocolType", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						rule->prot_type = ucloud_to_prot_type(json_object_get_string(jresval));
					}
					if (json_object_object_get_ex(jres, "DstPort", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						rule->prot_type = ucloud_to_prot_type(json_object_get_string(jresval));
					}
					if (json_object_object_get_ex(jres, "RuleAction", &jresval) &&
						json_object_get_type(jresval) == json_type_string)
					{
						rule->action = ucloud_to_rule_action(json_object_get_string(jresval));
					}
					rule->next = sg->rule;
					sg->rule = rule;
				}
            }
			sg->next = head_sg;
			head_sg = sg;
		}
	}
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_sg;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

ucloud_unet_sg_resource_t* ucloud_unet_describe_sg_resource(const char *region, int group_id)
{
	ucloud_unet_sg_resource_t *head_res = NULL;
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		ucloud_set_last_error("Invalid parameter");
		return NULL;
	}

	if (region == NULL)
	{
		ucloud_set_last_error("Invalid parameter region");
		return NULL;
	}
	ucloud_http_params_add(param, "Action", "DescribeSecurityGroupResource");
	ucloud_http_params_add(param, "Region", region);

	if (group_id > 0)
	{
		ucloud_http_params_add_int(param, "GroupId", group_id);
	}
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
	json_object *jval;
	if (json_object_object_get_ex(jobj, "DataSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		int i;
		for (i = 0; i<count; ++i)
		{
			json_object *jitem = json_object_array_get_idx(jval, i);	
			ucloud_unet_sg_resource_t *res;
			res = ucloud_unet_sg_resource_init();
            res->ip = strdup(json_object_get_string(jitem));
			res->next = head_res;
			head_res = res;
		}
	}

	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_res;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

int ucloud_unet_update_sg(const char *region, const ucloud_unet_sg_t *sg)
{
	if (region == NULL || sg == NULL || !sg->id || sg->rule == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","UpdateSecurityGroup");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add_int(param, "GroupId", sg->id);
	int n = 0;
	ucloud_unet_security_rule_t *rule;
	rule = sg->rule;
	while (rule)
	{
		char rule_str[64] = {0};
		char rule_num[12] = {0};
		sprintf(rule_num, "Rule.%d", n++);
		sprintf(rule_str, "%s|%s|%s|%s|%d", ucloud_from_prot_type(rule->prot_type), rule->dst_port, rule->src_ip, ucloud_from_rule_action(rule->action), rule->priority);
		ucloud_http_params_add(param, rule_num, rule_str);
		rule = rule->next;
	}

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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_unet_delete_sg(const char *region, int group_id)
{
	if (region == NULL || !group_id)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","DeleteSecurityGroup");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add_int(param, "GroupId", group_id);
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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_unet_grant_sg(const char *region, int group_id, int res_type, const char *res_id)
{
	if (region == NULL || !group_id || !res_type || !res_id)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		goto error;
	}
	
	ucloud_http_params_add(param, "Action","GrantSecurityGroup");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add_int(param, "GroupId", group_id);
	ucloud_http_params_add(param, "ResourceType", ucloud_from_resource_type(res_type));
	ucloud_http_params_add(param, "ResourceId", res_id);
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
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

