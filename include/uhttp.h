#ifndef __UCLOUD_UHTTP_H
#define __UCLOUD_UHTTP_H

#include <uinclude.h>
#include <uparams.h>
#include <json-c/json.h>

/**
 * @file uhttp.h
 * @brief encapsulate ucluod http request
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */


/**
 * @brief ucloud private key environment name
 */
#define UCLOUD_ENV_PRIVATE_KEY "UCLOUD_PRIVATE_KEY"

/**
 * @brief ucloud public key envronment name
 */
#define UCLOUD_ENV_PUBLIC_KEY  "UCLOUD_PUBLIC_KEY"

/**
 * @brief definition of http response
 */
typedef struct
{
	int capacity;              /*!< prealloc length */
	int length;    	           /*!< length of response content */
	char *result;  	           /*!< repsone content */
	bool has_error;            /*!< whether if response has error */
	char error[1024];          /*!< store error message if response has error */
}ucloud_uhttp_response_t;


/**
 * @brief ucloud http parameter signature
 *
 * @param params parameter collection
 * @param private_key private key
 * @param[out] signature
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhttp_param_signature(ucloud_http_params_t *params, const char *private_key, char *signature);

/**
 * @brief request ucloud http command
 *
 * @param params request parameters
 * @param response response of request
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhttp_request(ucloud_http_params_t* params, ucloud_uhttp_response_t *response);

/**
 * @brief initilize http response structure
 *
 * @return success return http response intialized else return NULL
 */
ucloud_uhttp_response_t *ucloud_uhttp_response_init();

/**
 * @brief deintialize http response
 *
 * @param response http response intialized ucloud_uhttp_response_init()
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhttp_response_deinit(ucloud_uhttp_response_t *response);

/**
 * @brief handle response header
 *
 * @param jobj json data format of response data
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uhttp_handle_resp_header(json_object *jobj);

#endif
