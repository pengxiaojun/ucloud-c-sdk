#include <uparams.h>
#include <uinclude.h>

#define UCLOUD_HTTP_PARAMS_INIT_LEN 	16

int ucloud_http_params_check(ucloud_http_params_t *params)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (params->count >= params->alloc)
	{
		//resize params	
		int newsize = params->count + UCLOUD_HTTP_PARAMS_INIT_LEN;
		char **keys = (char**)realloc(params->keys, newsize * sizeof(char*));
		char **vals = (char**)realloc(params->vals, newsize * sizeof(char*));

		if (keys == NULL || vals == NULL)
		{
			return UCLOUDE_ALLOC_MEMORY;
		}
		params->keys = keys;
		params->vals = vals;
		params->alloc = newsize;
	}
	return UCLOUDE_OK;
}

ucloud_http_params_t* ucloud_http_params_init()
{
	ucloud_http_params_t *params;
	params = (ucloud_http_params_t*)calloc(1, sizeof(*params));
	if (params == NULL)
	{
		return NULL;
	}
	params->keys = (char**)calloc(UCLOUD_HTTP_PARAMS_INIT_LEN, sizeof(char*));
	params->vals = (char**)calloc(UCLOUD_HTTP_PARAMS_INIT_LEN, sizeof(char*));

	if (params->keys == NULL || params->vals == NULL)
	{
		return NULL;
	}
	params->alloc = UCLOUD_HTTP_PARAMS_INIT_LEN;
	return params;
}

int ucloud_http_params_deinit(ucloud_http_params_t *params)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	int i;
	for (i = 0; i<params->count; ++i)
	{
		if (params->keys[i])
		{
			free(params->keys[i]);
		}
		if (params->vals[i])
		{
			free(params->vals[i]);
		}
	}
	if (params->keys)
	{
		free(params->keys);
	}
	if (params->vals)
	{
		free(params->vals);
	}
	free(params);
	return UCLOUDE_OK;
}

int ucloud_http_params_add(ucloud_http_params_t *params, const char *key, const char *val)
{
	if (key == NULL || val == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	int ret = ucloud_http_params_check(params);
	if (UCLOUDE_OK != ret)
	{
		return ret;
	}
	params->keys[params->count] = strdup(key);
	params->vals[params->count] = strdup(val);
	params->count++;
	return UCLOUDE_OK;
}

int ucloud_http_params_sort_add(ucloud_http_params_t *params, const char *key, const char *val)
{
	if (key == NULL || val == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	int ret = ucloud_http_params_check(params);
	if (UCLOUDE_OK != ret)
	{
		return ret;
	}
	//find slot to add
	int i = params->count;
	while ( --i >= 0 && strcmp(key, params->keys[i]) < 0)
	{
		params->keys[i+1] = params->keys[i];
		params->vals[i+1] = params->vals[i];
	}
	params->keys[i+1] = strdup(params->keys[i]);
	params->vals[i+1] = strdup(params->vals[i]);
	return UCLOUDE_OK;
}

int ucloud_http_params_add_int(ucloud_http_params_t *params, const char *key, int val)
{
	char charvar[64] = {0};
	sprintf(charvar, "%d", val);
	return ucloud_http_params_add(params, key, charvar);
}

int ucloud_http_params_length(const ucloud_http_params_t *params)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	int i;
	int len = 0;
	for (i=0; i<params->count; ++i)
	{
		len += strlen(params->keys[i]);
		len += strlen(params->vals[i]);
	}
	return len;
}

int ucloud_http_params_count(const ucloud_http_params_t *params)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	return params->count;
}

int ucloud_http_params_querystr_len(const ucloud_http_params_t *params)
{
	int len = ucloud_http_params_length(params);
	len += params->count;  //add = between key and value
	if (params->count > 0)
	{
		len += 1; 		//add ?
		len += (params->count - 1); //add & between parameter
	}
	return len;
}

int ucloud_http_params_querystr(const ucloud_http_params_t *params, char *querystr, int *len)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	int i;
	int querystr_len = ucloud_http_params_querystr_len(params);
	if (querystr_len == 0)
	{
		return UCLOUDE_ERROR;
	}

	if ((*len) < querystr_len)
	{
		*len = querystr_len+1;
		return UCLOUDE_ERROR;
	}
	char *buf = calloc(1, querystr_len+1);
	char *pos;

	pos = buf;
	*pos++ = '?';

	for (i = 0; i<params->count; ++i)
	{
		if (i > 0)
		{
			*pos++ = '&';
		}
		strcpy(pos, params->keys[i]);
		pos += strlen(params->keys[i]);
		*pos++ = '=';
		strcpy(pos, params->vals[i]);
		pos += strlen(params->vals[i]);
	}
	strcpy(querystr, buf);
	free(buf);
	return UCLOUDE_OK;
}

int ucloud_http_params_sort(ucloud_http_params_t *params, ucloud_http_params_sort_func_t func)
{
	if (params == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}

	int i, j;
	bool exchange;
	char *tmp_key;
	char *tmp_val;

	//sort parameters
	for (i = 0; i<params->count; ++i)
	{
		exchange = false;
		for (j = params->count-1; j > i; --j)
		{
			if ((*func)(params->keys[j], params->keys[j-1]) < 0)
			{
				//swap
				tmp_key = params->keys[j-1];
				tmp_val = params->vals[j-1];

				params->keys[j-1] = params->keys[j];
				params->vals[j-1] = params->vals[j];
				params->keys[j] = tmp_key;
				params->vals[j] = tmp_val;
				exchange = true;
			}
		}
		if (!exchange) break;
	}
	return UCLOUDE_OK;
}
