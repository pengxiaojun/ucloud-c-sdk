#ifndef __UCLOUD_UERROR_H
#define __UCLOUD_UERROR_H

/**
 * @file uerror.h
 * @brief define ucloud sdk error code
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */

/** 
 * @brief success
 */
#define UCLOUDE_OK                      0

/** 
 * @brief error count of signature parameters
 */
#define UCLOUDE_SIGNATURE_PARAM_COUNT   101

/** 
 * @brief allocate memroy error
 */
#define UCLOUDE_ALLOC_MEMORY            102

/** 
 * @brief invalid paramater 
 */
#define UCLOUDE_INVALID_PARAM           103

/** 
 * @brief unknow error
 */
#define UCLOUDE_UNKNOW                  104

/** 
 * @brief failure
 */
#define UCLOUDE_ERROR 				    105

/** 
 * @brief http request error
 */
#define UCLOUDE_HTTP_REQUEST            106

/** 
 * @brief insufficient length error 
 */
#define UCLOUDE_INSUFF_LENGTH           107

/** 
 * @brief not exist public key
 */
#define UCLOUDE_NO_PUBLIC_KEY           108

/**
 * @brief not exist private key
 */
#define UCLOUDE_NO_PRIVATE_KEY          109
/**
 * @brief string describing error number
 *
 * @param code error number
 *
 * @return string describing error number
 */
const char *ucloud_strerror(int code);

/**
 * @brief get last error
 *
 * @return last error
 */
const char *ucloud_get_last_error();

/**
 * @brief set last error
 *
 * @param err string describing error
 */
void ucloud_set_last_error(const char *err);
#endif
