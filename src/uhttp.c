#include <uhttp.h>
#include <uinclude.h>
#include <uparams.h>
#include <curl/curl.h>
#include <sha1.h>

#define UCLOUD_HTTP_RESPONSE_INIT_LEN  409600
#define UCLOUD_HTTP_RESPONSE_INCR_LEN  1024

static int ucloud_uhttp_hash_param(ucloud_http_params_t *paramm);
static int ucloud_uhttp_request_internal(ucloud_http_params_t *params, ucloud_uhttp_response_t *response);

static int param_sort(const char *left, const char *right)
{
	return strcmp(left, right);
}

ucloud_uhttp_response_t *ucloud_uhttp_response_init()
{
	ucloud_uhttp_response_t *resp;
	resp = (ucloud_uhttp_response_t*)calloc(1, sizeof(*resp));
	if (resp == NULL)
	{
		return NULL;
	}
	resp->result = calloc(1, UCLOUD_HTTP_RESPONSE_INIT_LEN);
	resp->capacity = UCLOUD_HTTP_RESPONSE_INIT_LEN;
	return resp;
}

int ucloud_uhttp_response_deinit(ucloud_uhttp_response_t *resp)
{
	if (resp == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	if (resp->result)
	{
		free(resp->result);
	}
	free(resp);
	return UCLOUDE_OK;
}

int ucloud_uhttp_param_signature(ucloud_http_params_t *params, const char *private_key, char *signature)
{
	if (params == NULL || private_key == NULL || signature == NULL)
	{
		return UCLOUDE_INVALID_PARAM;
	}
	//sort params
	ucloud_http_params_sort(params, param_sort);

	int i;
	SHA1_CTX ctx;
	unsigned char hash[20] = {0};
	char hex[2];
	SHA1Init(&ctx);

	for(i = 0; i<params->count; ++i)
	{
		if (params->keys[i] && params->vals[i])
		{
			char kv[512] = {0};
			strcpy(kv, params->keys[i]);
			strcat(kv, params->vals[i]);
			SHA1Update(&ctx, (unsigned char*)kv, strlen(kv));
#ifdef __DEBUG__
			printf("%d %s\n", i, kv);
#endif
		}
	}
	SHA1Update(&ctx, (unsigned char*)private_key, strlen(private_key));
	SHA1Final(hash, &ctx);

	for (i = 0; i<sizeof(hash); ++i)
	{
		sprintf(hex, "%02x", hash[i]);
		strcat(signature, hex);
	}
	return UCLOUDE_OK;
}

int ucloud_uhttp_request(ucloud_http_params_t *params, ucloud_uhttp_response_t *response)
{
	return ucloud_uhttp_request_internal(params, response);
}

static int ucloud_uhttp_hash_param(ucloud_http_params_t *params)
{
	//add public key parameter
	char *public_key = getenv(UCLOUD_ENV_PUBLIC_KEY);
	if (public_key == NULL){
		ucloud_set_last_error("Not exist public key. please set environment 'UCLOUD_PUBLIC_KEY' at first");
		return UCLOUDE_NO_PUBLIC_KEY;
	}
	ucloud_http_params_add(params, "PublicKey", public_key);
	//sort params
	ucloud_http_params_sort(params, param_sort);

	int i;
	SHA1_CTX ctx;
	unsigned char hash[20] = {0};
	char signature[41] = {0};
	char hex[2];
	SHA1Init(&ctx);

	for(i = 0; i<params->count; ++i)
	{
		if (params->keys[i] && params->vals[i])
		{
			char kv[512] = {0};
			strcpy(kv, params->keys[i]);
			strcat(kv, params->vals[i]);
			SHA1Update(&ctx, (unsigned char*)kv, strlen(kv));
#ifdef __DEBUG__
			printf("%d %s\n", i, kv);
#endif
		}
	}
	char *private_key = getenv(UCLOUD_ENV_PRIVATE_KEY);

	if (private_key == NULL)
	{
		ucloud_set_last_error("Not exist private key. please set environment 'UCLOUD_PRIVATE_KEY' at first");
		return UCLOUDE_NO_PRIVATE_KEY;
	}
	SHA1Update(&ctx, (unsigned char*)private_key, strlen(private_key));
	SHA1Final(hash, &ctx);

	for (i = 0; i<sizeof(hash); ++i)
	{
		sprintf(hex, "%02x", hash[i]);
		strcat(signature, hex);
	}
#ifdef __DEBUG__
   	printf("\n");
   	printf("Hash=%s\n", signature);
#endif
	//add signatrue parameter
	ucloud_http_params_add(params, "Signature", signature);	
	return UCLOUDE_OK;
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t realsize = size * nmemb;
	//fprintf(stderr, "recv response %s len %d\n", ptr, (int)realsize);
	ucloud_uhttp_response_t *resp = (ucloud_uhttp_response_t*)userdata;
	if (resp->capacity < realsize)
	{
		size_t incr_size = realsize > UCLOUD_HTTP_RESPONSE_INCR_LEN ? realsize+1 : UCLOUD_HTTP_RESPONSE_INCR_LEN;
		resp->result = realloc(resp->result, resp->length + incr_size);
		if (resp->result == NULL)
		{
			return UCLOUDE_ALLOC_MEMORY;
		}
	}
	memcpy(resp->result + resp->length, ptr, realsize);
	resp->length += realsize;
	resp->capacity -= realsize;
	resp->result[resp->length] = 0;
	return realsize;
}

static int ucloud_uhttp_request_internal(ucloud_http_params_t *params, ucloud_uhttp_response_t *response)
{
	//do signature
	int ret = ucloud_uhttp_hash_param(params);
	if (ret != UCLOUDE_OK)
	{
		return ret;
	}

	//construct request url
	int querystr_len = ucloud_http_params_querystr_len(params);
	if (querystr_len == 0){
		return UCLOUDE_ERROR;
	}
	char *api_url = "http://api.ucloud.cn/";  //hard code api url
	char *url = calloc(1, querystr_len + strlen(api_url) + 1);
	strcpy(url, api_url);
	char *querystr = url + strlen(api_url);
	ret = ucloud_http_params_querystr(params, querystr, &querystr_len);
	if (ret != UCLOUDE_OK)
	{
		free(url);
		return ret;
	}

#ifdef __DEBUG__
    printf("Request url=%s\n", url);
#endif

	//do request
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	if (curl == NULL)
	{
		free(url);
		response->has_error = true;
		strcpy(response->error, "curl_easy_init() error");
		return UCLOUDE_ERROR;
	}

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
	{
		free(url);
		strcpy(response->error, curl_easy_strerror(res));
		fprintf(stderr, "curl_easy_perform() failed:%s\n", curl_easy_strerror(res));
		ucloud_set_last_error(response->error);
		return UCLOUDE_HTTP_REQUEST;
	}
	long retcode;
	ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &retcode);
	free(url);
	if (ret != CURLE_OK || retcode != 200)
	{
		fprintf(stderr, "curl_easy_getinfo() code:%ld\n", retcode);
		ucloud_set_last_error("curl_easy_get_info return code is not 200");
		return UCLOUDE_HTTP_REQUEST;
	}
	//handle response
	curl_easy_cleanup(curl);
	return UCLOUDE_OK;
}

int ucloud_uhttp_handle_resp_header(json_object *jobj)
{
	json_object *jval;
	//get retcode
	int retcode = -1;
	if (json_object_object_get_ex(jobj, "RetCode", &jval) &&
		json_object_get_type(jval) == json_type_int)
	{
		retcode = json_object_get_int(jval);
	}
	if (retcode != 0)
	{
		if (json_object_object_get_ex(jobj, "Message", &jval) &&
			json_object_get_type(jval) == json_type_string)
		{
			ucloud_set_last_error(json_object_get_string(jval));
		}
	}
	return retcode;
}


