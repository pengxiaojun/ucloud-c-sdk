#include <uimage.h>
#include <uhttp.h>
#include <uparams.h>
#include <json-c/json.h>

ucloud_uimage_t* ucloud_uimage_init()
{
	ucloud_uimage_t *image;
	image = (ucloud_uimage_t*)calloc(1, sizeof(*image));
	if (image == NULL)
	{
		return NULL;
	}
	return image;
}

int ucloud_uimage_deinit(ucloud_uimage_t *image)
{
	if (image == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (image->id) free(image->id);
	if (image->name) free(image->name);
	if (image->desc) free(image->desc);
	if (image->os_name) free(image->os_name);
	free(image);
	return UCLOUDE_OK;
}

int ucloud_uimage_describe(ucloud_uimageset_t *imageset, const char *region)
{
	if (imageset == NULL || region == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_http_params_add(param, "Action", "DescribeImage");
	ucloud_http_params_add(param, "Region", region);

	ucloud_uhttp_response_t *resp;
	resp = ucloud_uhttp_response_init();

	int ret = ucloud_uhttp_request(param, resp);
	if (UCLOUDE_OK != ret)
	{
		goto error;
	}
	//parse result
	json_object *jobj = json_tokener_parse(resp->result);
	int retcode = -1;
	json_object *jval;
	//get retcode
	if (json_object_object_get_ex(jobj, "RetCode", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		retcode = json_object_get_int(jval);
	}
	if (retcode != 0)
	{
		sprintf(resp->error, "RetCode error: %d", retcode);
		goto error;
	}

	//get total count
	int total = 0;
	if (json_object_object_get_ex(jobj, "TotalCount", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		total = json_object_get_int(jval);
	}

	//parse image set 
	if (json_object_object_get_ex(jobj, "ImageSet", &jval) &&
		json_object_get_type(jval) == json_type_array)
	{
		int count = json_object_array_length(jval);
		if (count != total)
		{
			sprintf(resp->error, "Mismatch image set total=%d, actual=%d", total, count);
			goto error;
		}

		for (count = 0; count<total; ++count)
		{
            json_object *jitem = json_object_array_get_idx(jval, count);
			//create image
			ucloud_uimage_t *image;
			image = ucloud_uimage_init();

			json_object *jitemval;
			//get image id
			if (json_object_object_get_ex(jitem, "ImageId", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                image->id = strdup(json_object_get_string(jitemval));
            }

			//get image name
			if (json_object_object_get_ex(jitem, "ImageName", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                image->name = strdup(json_object_get_string(jitemval));
            }
			//get os type
			if (json_object_object_get_ex(jitem, "OsType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
				const char *os = json_object_get_string(jitemval);
				if (strcmp(os, "Windows") == 0)
				{
					image->os_type = uuot_windows;
				}
				else if (strcmp(os, "Linux") == 0)
				{
					image->os_type = uuot_linux;
				}
            }
			//get os name
			if (json_object_object_get_ex(jitem, "OsName", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                image->os_name = strdup(json_object_get_string(jitemval));
            }
			//get image type
			if (json_object_object_get_ex(jitem, "ImageType", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
				const char *type = json_object_get_string(jitemval);
				if (strcmp(type, "Base") == 0)
				{
					image->type = uut_base;
				}
            }
			//get state
			if (json_object_object_get_ex(jitem, "State", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
				const char *state = json_object_get_string(jitemval);
				if (strcmp(state, "Available") == 0)
				{
					image->state = uus_avail;
				}
            }
			//get image desc
			if (json_object_object_get_ex(jitem, "OsName", &jitemval) &&
                json_object_get_type(jitemval) == json_type_string)
            {
                image->desc = strdup(json_object_get_string(jitemval));
            }
			//get create time
			if (json_object_object_get_ex(jitem, "CreateTime", &jitemval) &&
                json_object_get_type(jitemval) == json_type_int)
            {
                image->created_time = json_object_get_int(jitemval);
            }
			//append to imageset
			ucloud_uimageset_put(imageset, image);
		}
	}

	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return UCLOUDE_OK;
error:
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return UCLOUDE_ERROR;
}

/* ucloud imageset */
ucloud_uimageset_t* ucloud_uimageset_init()
{
	ucloud_uimageset_t *imageset = (ucloud_uimageset_t*)calloc(1, sizeof(ucloud_uimageset_t));
	if (imageset == NULL)
	{
		return NULL;
	}
	imageset->head = imageset->tail = NULL;
	return imageset;
}

int ucloud_uimageset_deinit(ucloud_uimageset_t *imageset)
{
	if (imageset == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	
	ucloud_uimage_t *image;

	while ((image = ucloud_uimageset_get(imageset)) != NULL)
	{
		ucloud_uimage_deinit(image);
	}
	free(imageset);
	return UCLOUDE_OK;
}

bool ucloud_uimageset_isempty(ucloud_uimageset_t *imageset)
{
	if (imageset == NULL)
	{
		return true;
	}
	return (imageset->head == imageset->tail) && (imageset->head == NULL);
}

//append image to tail
int ucloud_uimageset_put(ucloud_uimageset_t *imageset, ucloud_uimage_t *image)
{
	if (imageset == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	image->next = NULL;
	if (imageset->tail != NULL)
	{
		imageset->tail->next = image;
	}
	imageset->tail = image;
	if (imageset->head == NULL)
	{
		imageset->head = image;
	}
	return UCLOUDE_OK;
}

ucloud_uimage_t* ucloud_uimageset_get(ucloud_uimageset_t *imageset)
{
	if (imageset == NULL)
	{
		return NULL;
	}

	ucloud_uimage_t *image;
	image = imageset->head;
	
	if (image != NULL)
	{
		imageset->head = imageset->head->next;
	}
	return image;
}

/* ucloud custom image */
ucloud_custom_uimage_t* ucloud_custom_uimage_init()
{
	ucloud_custom_uimage_t *image;
	image = (ucloud_custom_uimage_t*)calloc(1, sizeof(*image));
	if (image == NULL)
	{
		return NULL;
	}
	return image;
}

int ucloud_custom_uimage_deinit(ucloud_custom_uimage_t *image)
{
	if (image == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (image->id) free(image->id);
	if (image->region) free(image->region);
	if (image->host_id) free(image->host_id);
	if (image->desc) free(image->desc);
	free(image);
	return UCLOUDE_OK;
}

/* create custom image */
int ucloud_uimage_create(ucloud_custom_uimage_t *image)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_uhttp_response_t *resp;
	resp = ucloud_uhttp_response_init();

	ucloud_http_params_add(param, "Action", "CreateCustomImage");
	if (image->region)
	{
		ucloud_http_params_add(param, "Region", image->region);
	}
	else
	{
		goto error;
	}

	if (image->host_id)
	{
		ucloud_http_params_add(param, "UHostId", image->host_id);
	}
	else
	{
		goto error;
	}

	if (image->name)
	{
		ucloud_http_params_add(param, "ImageName", image->name);
	}
	else
	{
		goto error;
	}
	if (image->desc)
	{
		ucloud_http_params_add(param, "ImageDescription", image->desc);
	}
	int ret = ucloud_uhttp_request(param, resp);
	if (UCLOUDE_OK != ret)
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

	//parse vnc ip
	if (json_object_object_get_ex(jobj, "ImageId", &jval) &&
		json_object_get_type(jval) == json_type_string)
	{
		image->id = strdup(json_object_get_string(jval));
	}
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

int ucloud_uimage_terminate(const char *region, const char *image_id)
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_uhttp_response_t *resp;
	resp = ucloud_uhttp_response_init();

	ucloud_http_params_add(param, "Action", "TerminateCustomImage");
	if (region)
	{
		ucloud_http_params_add(param, "Region", region);
	}
	else
	{
		goto error;
	}

	if (image_id)
	{
		ucloud_http_params_add(param, "ImageId", image_id);
	}
	else
	{
		goto error;
	}

	int ret = ucloud_uhttp_request(param, resp);
	if (UCLOUDE_OK != ret)
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
	ucloud_uhttp_response_deinit(resp);
	ucloud_http_params_deinit(param);
	return UCLOUDE_OK;
error:
	ucloud_http_params_deinit(param);
	ucloud_uhttp_response_deinit(resp);
	return UCLOUDE_ERROR;
}

