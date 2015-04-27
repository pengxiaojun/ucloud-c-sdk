#ifndef __UCLOUD_UIMAGE_H
#define __UCLOUD_UIMAGE_H

#include <uinclude.h>
/**
 * @file uimage.h
 * @brief encapsulate uhost image api
 * @author pengxj
 * @version 1.0
 * @date 2015-04-15
 */

/**
 * @brief image type
 */
typedef enum
{
	uut_base     = 0x01,
	uut_business = 0x02,
	uut_custom   = 0x04,
	uut_all      = 0x07
}ucloud_uimage_type_t;

/**
 * @brief uimage os type
 */
typedef enum
{
	uuot_linux   = 1,
	uuot_windows = 2
}ucloud_uimage_os_type_t;

/**
 * @brief uimage status
 */
typedef enum
{
	uus_avail   = 1,
	uus_making  = 2,
	uus_unavail = 3 
}ucloud_uimage_state_t;


/**
 * @brief uimage definition
 */
typedef struct ucloud_uimage_t
{
	char *id;                    /*!< image id */
	char *name;                  /*!< image name */
	char *desc;                  /*!< image description */
	char *os_name;               /*!< os name of image */
	int type;                    /*!< image type. reference ucloud_uimage_type_t enum */
	int state;                   /*!< image status. reference ucloud_uimage_state_t enum */
	int os_type;                 /*!< image os type. reference ucloud_uimage_os_type_t enum */
	int created_time;            /*!< created time of image */
	struct ucloud_uimage_t *next;/*!< pointer to next uimage */
}ucloud_uimage_t;

/**
 * @brief uimage set definition
 */
typedef struct
{
	int count;                   /*!< count of image */
	ucloud_uimage_t *head, *tail;/*!< head, tail of link */
}ucloud_uimageset_t;

/**
 * @brief custom image definition
 */
typedef struct
{
	char *id;        /*!< custom image id */
	char *name;      /*!< custom image name */
	char *region;    /*!< region */
	char *host_id;   /*!< host id */
	char *desc;      /*!< description of custom image */
}ucloud_custom_uimage_t;

/**
 * @brief initialize ucloud uimage 
 *
 * @return success return ucloud image else return NULL
 */
ucloud_uimage_t* ucloud_uimage_init();

/**
 * @brief deinitialize ucloud uimage
 *
 * @param image image initialize by ucloud_uimage_init()
 *
 * @return success return ucloud uimage else return NULL
 */
int ucloud_uimage_deinit(ucloud_uimage_t *image);

/**
 * @brief describe uimage under region
 *
 * @param[out] imageset uimage under region
 * @param region region
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uimage_describe(ucloud_uimageset_t *imageset, const char *region);


/**
 * @brief initialize image set
 *
 * @return image set structure
 */
ucloud_uimageset_t* ucloud_uimageset_init();

/**
 * @brief deinitialize image set
 *
 * @param imageset 
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uimageset_deinit(ucloud_uimageset_t *imageset);

/**
 * @brief wheter the imageset is emtpy
 *
 * @param imageset
 *
 * @return return true if image queue is emtpy
 */
bool ucloud_uimageset_isempty(ucloud_uimageset_t *imageset);

/**
 * @brief put a image to imageset queue
 *
 * @param imageset
 * @param image
 *
 * @return 
 */
int ucloud_uimageset_put(ucloud_uimageset_t *imageset, ucloud_uimage_t *image);

/**
 * @brief get a image from imageset
 *
 * @param imageset
 *
 * @return return the head element
 */
ucloud_uimage_t* ucloud_uimageset_get(ucloud_uimageset_t *imageset);


/**
 * @brief initilize custom image
 *
 * @return custom image structure
 */
ucloud_custom_uimage_t* ucloud_custom_uimage_init();

/**
 * @brief deinitialize custom image
 *
 * @param image
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_custom_uimage_deinit(ucloud_custom_uimage_t *image);

/**
 * @brief create custom image
 *
 * @param image custom image paramter
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uimage_create(ucloud_custom_uimage_t *image);

/**
 * @brief terminate custom image
 *
 * @param region region
 * @param image_id custom image id
 *
 * @return success return UCLOUD_OK else return other error
 */
int ucloud_uimage_terminate(const char *region, const char *image_id);
#endif
