#include <uhost.h>
#include <uinclude.h>
#include <ucommon.h>
#include <uparams.h>
#include <uhttp.h>
#include <base64.h>

ucloud_uhost_t* ucloud_uhost_init()
{
	ucloud_uhost_t *host;
	host = (ucloud_uhost_t*)calloc(1, sizeof(*host));
	if (host == NULL)
	{
		return NULL;
	}
	return host;
}

int ucloud_uhost_deinit(ucloud_uhost_t *host)
{
	if (host)
	{
		free(host);
		host = NULL;
	}
	return UCLOUDE_OK;
}

int ucloud_uhost_set_str_param(ucloud_http_params_t *param, ucloud_uhost_param_mask_t *mask, const char *val)
{
	if (param == NULL || mask == NULL || val == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	if (mask->region) ucloud_http_params_add(param, "Region", val);
	if (mask->image_id) ucloud_http_params_add(param, "ImageId", val);
	if (mask->name) ucloud_http_params_add(param, "Name", val);
	if (mask->password)
	{
		char base64_passwd[128] = {0};
		encode_base64((uint8_t*)base64_passwd, (uint8_t*)val, strlen(val));
		ucloud_http_params_add(param, "Password", base64_passwd);
	}
	if (mask->key_pair) ucloud_http_params_add(param, "KeyPair", val);
	return UCLOUDE_OK;
}

int ucloud_uhost_set_int_param(ucloud_http_params_t *param, ucloud_uhost_param_mask_t *mask, int val)
{
	if (param == NULL || mask == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	if (mask->cpu)
	{
		//verify cpu value
		if ((val & (val - 1)) != 0)
		{
			// not power of 2
			return UCLOUDE_INVALID_PARAM;
		}
		ucloud_http_params_add_int(param, "CPU", val);
	}
	if (mask->login_mode)
	{
		const char *login_mode = ucloud_from_uhost_login_mode(val);
		ucloud_http_params_add(param, "LoginMode", login_mode);
	}
	if (mask->memory) ucloud_http_params_add_int(param, "Memory", val);
	if (mask->disk_space) ucloud_http_params_add_int(param, "DiskSpace", val);
	if (mask->charge_type)
	{
		const char *charge_type = ucloud_from_charge_type(val);
		if (charge_type != NULL)
		{
			ucloud_http_params_add(param, "ChargeType", charge_type);
		}
	}
	if (mask->quantity) ucloud_http_params_add_int(param, "Quantity", val);
	if (mask->count) ucloud_http_params_add_int(param, "Count", val);
	return UCLOUDE_OK;
}

int ucloud_uhost_create(ucloud_http_params_t *param, char **host_ids, int *num)
{
	if (param == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "CreateUHostInstance");
	
	//do request
	ucloud_uhttp_response_t *resp;
	resp = ucloud_uhttp_response_init();

	int ret = ucloud_uhttp_request(param, resp);
	if (ret != UCLOUDE_OK)
	{
		goto error;
	}

	//handle response
	json_object *jobj = json_tokener_parse(resp->result);
	if (ucloud_uhttp_handle_resp_header(jobj) != 0)
	{
		goto error;
	}
	json_object *jval;
	if (json_object_object_get_ex(jobj, "UHostIds", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int i = 0;
		int count = json_object_array_length(jval);

		if (count > *num)
		{
			*num = count;
			ucloud_uhttp_response_deinit(resp);
			return UCLOUDE_INSUFF_LENGTH;
		}
		*num = count;
		for (i= 0; i<count; ++i)
		{
            json_object *jitem = json_object_array_get_idx(jval, i);
			strcpy(host_ids[i], json_object_get_string(jitem));
		}
	}
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_OK;
error:
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}


/* ---------------uhost instance--------------- */
int ucloud_uhost_terminate(const char *region, const char *host_id)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || host_id == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "TerminateUHostInstance");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);

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

int ucloud_uhost_ssr(int act, const char *region, const char *host_id)
{
	const char *ssr[] = {"StartUHostInstance", "StopUHostInstance", "RebootUHostInstance"};

	if (act < 0 || act >= sizeof(ssr)/sizeof(ssr[0]))
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return UCLOUDE_ALLOC_MEMORY;
	}

	if (region == NULL || host_id == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", ssr[act]);
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);

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

ucloud_uhost_t* ucloud_uhost_describe(const char *region, const char *host_id)
{
	ucloud_uhost_t *head_host = NULL;
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
	ucloud_http_params_add(param, "Action", "DescribeUHostInstance");
	ucloud_http_params_add(param, "Region", region);
	if (host_id && strlen(host_id) > 0)
	{
		ucloud_http_params_add(param, "UHostId.0", host_id);
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
	if (ucloud_uhttp_handle_resp_header(jobj) != 0)
	{
		goto error;
	}
	json_object *jval;
	//get total count
	int total = 0;
	if (json_object_object_get_ex(jobj, "TotalCount", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		total = json_object_get_int(jval);
	}
	//parse image set 
	if (json_object_object_get_ex(jobj, "UHostSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		if (count != total)
		{
			sprintf(resp->error, "Mismatch host set total=%d, actual=%d", total, count);
			goto error;
		}

		for (count = 0; count<total; ++count)
		{
			json_object *jitem = json_object_array_get_idx(jval, count);
			//create host 
			ucloud_uhost_t *host;
			host = ucloud_uhost_init();

			json_object *jitemval;
			//get host id
			if (json_object_object_get_ex(jitem, "UHostId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->id = strdup(json_object_get_string(jitemval));
            }

			//get image id
			if (json_object_object_get_ex(jitem, "ImageId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->image_id = strdup(json_object_get_string(jitemval));
            }

			//get basic image id
			if (json_object_object_get_ex(jitem, "BasicImageId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->basic_image_id = strdup(json_object_get_string(jitemval));
            }

			//get basic image name
			if (json_object_object_get_ex(jitem, "BasicImageName", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->basic_image_name = strdup(json_object_get_string(jitemval));
            }

			//get tag
			if (json_object_object_get_ex(jitem, "Tag", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->tag = strdup(json_object_get_string(jitemval));
            }
	
			//get name
			if (json_object_object_get_ex(jitem, "Name", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->name = strdup(json_object_get_string(jitemval));
            }

			//get remark
			if (json_object_object_get_ex(jitem, "Remark", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->remark = strdup(json_object_get_string(jitemval));
            }

			//get status
			if (json_object_object_get_ex(jitem, "State", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->status = ucloud_to_uhost_status(json_object_get_string(jitemval));
            }

			//get created time 
			if (json_object_object_get_ex(jitem, "CreateTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                host->created_time = json_object_get_int(jitemval);
            }

			//get charge type
			if (json_object_object_get_ex(jitem, "ChargeType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                host->charge_type = ucloud_to_charge_type(json_object_get_string(jitemval));
            }

			//get expired time 
			if (json_object_object_get_ex(jitem, "ExpireTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                host->expired_time = json_object_get_int(jitemval);
            }

			//get cpu
			if (json_object_object_get_ex(jitem, "CPU", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                host->cpu = json_object_get_int(jitemval);
            }

			//get memory
			if (json_object_object_get_ex(jitem, "Memory", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                host->memory = json_object_get_int(jitemval);
            }

			//get disk set
			json_object *jdisk;
			if (json_object_object_get_ex(jitem, "DiskSet", &jdisk) &&
				json_object_get_type(jdisk) == json_type_array)
			{
				int i;
				int count = json_object_array_length(jdisk);
				for (i = 0; i<count; ++i)
				{
					json_object *jitem = json_object_array_get_idx(jdisk, i);
					//create disk
					ucloud_disk_t *disk;
					disk = ucloud_disk_init();

					json_object *jitemval;
					//get disk type
					if (json_object_object_get_ex(jitem, "Type", &jitemval) &&
						json_object_get_type(jitemval) == json_type_string)
					{
						disk->type = ucloud_to_disk_type(json_object_get_string(jitemval));
					}

					//get disk id
					if (json_object_object_get_ex(jitem, "DiskId", &jitemval) &&
						json_object_get_type(jitemval) == json_type_string)
					{
						disk->id = strdup(json_object_get_string(jitemval));
					}

					//get disk size
					if (json_object_object_get_ex(jitem, "Size", &jitemval) &&
						json_object_get_type(jitemval) == json_type_int)
					{
						disk->capacity = json_object_get_int(jitemval);
					}
					disk->next = host->diskset;
					host->diskset = disk;
				}
			}

			//get ip set
			json_object *jip;
			if (json_object_object_get_ex(jitem, "IPSet", &jip) &&
				json_object_get_type(jip) == json_type_array)
			{
				int i;
				int count = json_object_array_length(jip);
				for (i = 0; i<count; ++i)
				{
					json_object *jitem = json_object_array_get_idx(jip, i);
					//create ip 
					ucloud_ip_t *ip;
					ip = ucloud_ip_init();

					json_object *jitemval;
					//get ip type
					if (json_object_object_get_ex(jitem, "Type", &jitemval) &&
						json_object_get_type(jitemval) == json_type_string)
					{
						ip->type = ucloud_to_ip_type(json_object_get_string(jitemval));
					}

					//get ip address
					if (json_object_object_get_ex(jitem, "IP", &jitemval) &&
						json_object_get_type(jitemval) == json_type_string)
					{
						ip->ip = strdup(json_object_get_string(jitemval));
					}

					if (ip->type != uit_private)
					{
						//get ip id
						if (json_object_object_get_ex(jitem, "IPId", &jitemval) &&
								json_object_get_type(jitemval) == json_type_string)
						{
							ip->id = strdup(json_object_get_string(jitemval));
						}

						//get ip bandwidth
						if (json_object_object_get_ex(jitem, "bandwidth", &jitemval) &&
								json_object_get_type(jitemval) == json_type_int)
						{
							ip->bandwidth = json_object_get_int(jitemval);
						}
					}
					ip->next = host->ipset;
					host->ipset = ip;
				}
			}

			//append to hostset
			host->next = head_host;
			head_host = host;
		}
	}//end of host set
     
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return head_host;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

int ucloud_uhost_describe_deinit(ucloud_uhost_t *host)
{
	ucloud_uhost_t *head;
	while (host != NULL)
	{
		head = host->next;
		ucloud_uhost_deinit(host);
		host = head;
	}
	return UCLOUDE_OK;
}

int ucloud_uhost_resize(ucloud_http_params_t *param, const char *region, const char *host_id)
{
	if (param == NULL || region == NULL || host_id == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "ResizeUHostInstance");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);
	
	//do request
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
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_uhost_set_reinstall_param(ucloud_http_params_t *param, ucloud_uhost_reinstall_param_mask_t *mask, const char *val)
{
	if (param == NULL || mask == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (mask->password && val)
	{
		char base64_passwd[128] = {0};
		encode_base64((uint8_t*)base64_passwd, (uint8_t*)val, strlen(val));
		ucloud_http_params_add(param, "Password", base64_passwd);
	}
	if (mask->image_id && val)
	{
		ucloud_http_params_add(param, "ImageId", val);
	}
	if (mask->reserve_disk && val)
	{
		ucloud_http_params_add(param, "ReserveDisk", val);
	}
	return UCLOUDE_OK;
}

int ucloud_uhost_reinstall(ucloud_http_params_t *param, const char *region, const char *host_id)
{
	if (param == NULL || region == NULL || host_id == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_add(param, "Action", "ReinstallUHostInstance");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);
	
	//do request
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
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_uhost_reset_passwd(const char *region, const char *host_id, const char *passwd)
{
	if (region == NULL || host_id == NULL || passwd == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_http_params_add(param, "Action", "ResetUHostInstancePassword");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);
	char base64_passwd[128] = {0};
	encode_base64((uint8_t*)base64_passwd, (uint8_t*)passwd, strlen(passwd));
	ucloud_http_params_add(param, "Password", base64_passwd);

	//do request
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
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_uhost_ntr(int act, const char *region, const char *host_id, const char *val)
{
	static const char *ntr[] = {"ModifyUHostInstanceName", "ModifyUHostInstanceTag", "ModifyUHostInstanceRemark"};
	static const char *ntr_param[] = {"Name", "Tag", "Remark"};

	if (act < 0 || act >= sizeof(ntr)/sizeof(ntr[0]))
	{
		return UCLOUDE_INVALID_PARAM;
	}

	if (region == NULL || host_id == NULL || val == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_http_params_add(param, "Action", ntr[act]);
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);
	ucloud_http_params_add(param, ntr_param[act], val);

	//do request
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
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return ret == 0 ? UCLOUDE_OK : UCLOUDE_ERROR;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

ucloud_uhost_price_t* ucloud_uhost_get_price(ucloud_http_params_t* param)
{
	ucloud_uhost_price_t *head_price = NULL;
	ucloud_http_params_add(param, "Action", "GetUHostInstancePrice");

	//do request
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
	//parse price set 
	if (json_object_object_get_ex(jobj, "PriceSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int i;
		int count = json_object_array_length(jval);

		for (i = 0; i<count; ++i)
		{
			json_object *jitem = json_object_array_get_idx(jval, i);
			//create host 
			ucloud_uhost_price_t *price;
			price = ucloud_uhost_price_init();

			json_object *jitemval;
			//get charge type
			if (json_object_object_get_ex(jitem, "ChargeType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                price->charge_type = ucloud_to_charge_type(json_object_get_string(jitemval));
            }

			//get price
			if (json_object_object_get_ex(jitem, "Price", &jitemval))
			{
                if (json_object_get_type(jitemval) == json_type_double)
                	price->price = json_object_get_double(jitemval);
				else if (json_object_get_type(jitemval) == json_type_int)
                	price->price = json_object_get_int(jitemval);
			}
			price->next = head_price;
			head_price = price;
		}
	}
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return head_price;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;

}

int ucloud_uhost_get_price_deinit(ucloud_uhost_price_t* price)
{
	ucloud_uhost_price_t *head;
	while (price != NULL)
	{
		head = price->next;
		ucloud_uhost_price_deinit(price);
		price = head;
	}
	return UCLOUDE_OK;
}

ucloud_uhost_vnc_t* ucloud_uhost_get_vnc(const char *region, const char *host_id)
{
	if (region == NULL || host_id == NULL)
	{
		return NULL;
	}
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_http_params_add(param, "Action", "GetUHostInstanceVncInfo");
	ucloud_http_params_add(param, "Region", region);
	ucloud_http_params_add(param, "UHostId", host_id);
	
	//do request
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
	//handle response
	json_object *jval;

	ucloud_uhost_vnc_t *vnc;
	vnc = ucloud_uhost_vnc_init();
	//parse vnc ip
	if (json_object_object_get_ex(jobj, "VncIP", &jval) &&
		json_object_get_type(jval) == json_type_string)
	{
		vnc->ip = strdup(json_object_get_string(jval));
	}

	//parse vnc port
	if (json_object_object_get_ex(jobj, "VncPort", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		vnc->port = json_object_get_int(jval);
	}
	//parse vnc password
	if (json_object_object_get_ex(jobj, "VncPassword", &jval) &&
		json_object_get_type(jval) == json_type_string)
	{
		vnc->passwd = strdup(json_object_get_string(jval));
	}

	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return vnc;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return NULL;
}

/* ------------disk set------------- */
ucloud_disk_t* ucloud_disk_init()
{
	ucloud_disk_t *disk;
	disk = (ucloud_disk_t*)calloc(1, sizeof(*disk));
	if (disk == NULL)
	{
		return NULL;
	}
	return disk;
}

int ucloud_disk_deinit(ucloud_disk_t *disk)
{
	if (disk == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (disk->id) free(disk->id);
	free(disk);
	return UCLOUDE_OK;
}

/* ------------ipset------------- */
ucloud_ip_t* ucloud_ip_init()
{
	ucloud_ip_t *ip;
	ip = (ucloud_ip_t*)calloc(1, sizeof(*ip));
	if (ip == NULL)
	{
		return NULL;
	}
	return ip;
}

int ucloud_ip_deinit(ucloud_ip_t *ip)
{
	if (ip == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (ip->id) free(ip->id);
	if (ip->ip) free(ip->ip);
	free(ip);
	return UCLOUDE_OK;
}

/* -----------price-------------- */
ucloud_uhost_price_t* ucloud_uhost_price_init()
{
	ucloud_uhost_price_t *price;
	price = (ucloud_uhost_price_t*)calloc(1, sizeof(*price));
	if (price == NULL)
	{
		return NULL;
	}
	return price;
}

int ucloud_uhost_price_deinit(ucloud_uhost_price_t *price)
{
	if (price == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	free(price);
	return UCLOUDE_OK;
}

ucloud_uhost_vnc_t* ucloud_uhost_vnc_init()
{
	ucloud_uhost_vnc_t* vnc;
	vnc = (ucloud_uhost_vnc_t*)calloc(1, sizeof(*vnc));
	if (vnc == NULL)
	{
		return NULL;
	}
	return vnc;
}

int ucloud_uhost_vnc_deinit(ucloud_uhost_vnc_t *vnc)
{
	if (vnc == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (vnc->ip) free(vnc->ip);
	if (vnc->passwd) free(vnc->passwd);
	free(vnc);
	return UCLOUDE_OK;
}
