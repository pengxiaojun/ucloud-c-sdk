#ifndef __UCLOUD_BASE64_H
#define __UCLOUD_BASE64_H

#include <uinclude.h>

/**
 * @file base64.h
 * @brief base64 encoder/decoder
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */

/**
 * @brief calculate length of encoded data
 */
#define base64_encoded_length(len)  (((len + 2) / 3) * 4)

/**
 * @brief calculate length of decoded data
 */
#define base64_decoded_length(len)  (((len + 3) / 4) * 3)

/**
 * @brief base64 encode
 *
 * @param dst data after encoded
 * @param src data will be encoded
 * @param slen length of source data
 *
 * @return length of destination data
 */
size_t encode_base64(uint8_t *dst, uint8_t *src, size_t slen);


/**
 * @brief base64 decode
 *
 * @param dst data after decoded
 * @param src data will be decoded
 * @param slen length of source data
 *
 * @return legnth of destination data
 */
size_t decode_base64(uint8_t *dst, uint8_t *src, size_t slen);


/**
 * @brief base64 decode for url
 *
 * @param dst data after decoded
 * @param src data will be decoded
 * @param slen source length of data
 *
 * @return length of destination data
 */
size_t decode_base64url(uint8_t *dst, uint8_t *src, size_t slen);

#endif

