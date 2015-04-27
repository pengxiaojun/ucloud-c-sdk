#ifndef __UCLOUD_UPARAMS_H
#define __UCLOUD_UPARAMS_H

/**
 * @file uparams.h
 * @brief encapsulate http request parameter and http signature
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */

/**
 * @brief http request parameter collection
 */
typedef struct
{
	char **keys; /*!< parameter keys array */
	char **vals; /*!< parameter values array */
	int count;   /*!< count of parameter */
	int alloc;   /*!< preallction number of parameter. used in sdk internal */
}ucloud_http_params_t;


/**
 * @brief intialize http paramter
 *
 * @return http paramter
 */
ucloud_http_params_t* ucloud_http_params_init();

/**
 * @brief deintialize http paramter
 *
 * @param params paramter will be deintialized
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_deinit(ucloud_http_params_t *params);

/**
 * @brief add parameter to http paramters collection
 *
 * @param params paramters collection
 * @param key parameter key
 * @param val parameter value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_add(ucloud_http_params_t *params, const char *key, const char *val);

/**
 * @brief add paramter to http paramters collection sortly
 *
 * @param params parameters collection
 * @param key paramter key
 * @param val paramter value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_sort_add(ucloud_http_params_t *params, const char *key, const char *val);

/**
 * @brief delete a specified parameter from paramters collection
 *
 * @param params parameter collection
 * @param key parameter key will be deleted
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_del(ucloud_http_params_t *params, const char *key);

/**
 * @brief add number value to parameter collection
 *
 * @param params parameter collection
 * @param key parameter key
 * @param val parameter number value
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_add_int(ucloud_http_params_t *params, const char *key, int val);

/**
 * @brief get the count of  parameter collection
 *
 * @param params parameter collection
 *
 * @return number of parameters in collection
 */
int ucloud_http_params_count(const ucloud_http_params_t *params);

/**
 * @brief get the length of parameter collection
 *
 * @param params parameter collection
 *
 * @return length of all paramters
 */
int ucloud_http_params_length(const ucloud_http_params_t *params);

/**
 * @brief get the length of parameter collection in query string format(include ?)
 *
 * @param params paramter collection
 *
 * @return length of parameter collection in query string format
 */
int ucloud_http_params_querystr_len(const ucloud_http_params_t *params);

/**
 * @brief convert paramter collection in query string
 *
 * @param params parameter collection
 * @param querystr query string
 * @param len length of query string
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_querystr(const ucloud_http_params_t *params, char *querystr, int *len);

/**
 * @brief parameter compare function
 *
 * @param left left parameter
 * @param right right parameter
 *
 * @return the same with strcmp()
 */
typedef int (*ucloud_http_params_sort_func_t)(const char *left, const char *right);

/**
 * @brief sort parameters
 *
 * @param params parameters to be sorted
 * @param func sort fucntion
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_http_params_sort(ucloud_http_params_t *params, ucloud_http_params_sort_func_t func);
#endif
