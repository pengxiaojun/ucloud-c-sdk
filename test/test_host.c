#include "test_ucloud.h"
#include <ucloud.h>

static int test_host_create();
static int test_image_describe();

int test_host()
{
	test_host_create();
	test_image_describe();
	return 0;
}

int test_host_create()
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	if (param == NULL)
	{
		return 0;
	}

	ucloud_uhost_param_mask_t mask;
	memset(&mask, 0, sizeof(mask));
	mask.region = 1;
	ucloud_uhost_set_str_param(param, &mask, "cn-north-03");

	memset(&mask, 0, sizeof(mask));
	mask.image_id = 1;
	ucloud_uhost_set_str_param(param, &mask, "0163c0c5-7481-4319-9c");

	memset(&mask, 0, sizeof(mask));
	mask.login_mode = 1;
	ucloud_uhost_set_int_param(param, &mask, uulm_passwd);

	memset(&mask, 0, sizeof(mask));
	mask.password = 1;
	ucloud_uhost_set_str_param(param, &mask, "passwd");

	memset(&mask, 0, sizeof(mask));
	mask.cpu = 1;
	ucloud_uhost_set_int_param(param, &mask, 1);

	memset(&mask, 0, sizeof(mask));
	mask.memory = 1;
	ucloud_uhost_set_int_param(param, &mask, 1024);

	memset(&mask, 0, sizeof(mask));
	mask.disk_space = 1;
	ucloud_uhost_set_int_param(param, &mask, 10);


#define MAX_HOST_INSTANCE  128
#define MAX_HOST_ID_LENGTH 64

	//create uhost
	int count = MAX_HOST_INSTANCE;
	char *host_ids[MAX_HOST_INSTANCE];
	char *ids = calloc(count, MAX_HOST_ID_LENGTH);
	int i;

	for (i = 0; i<count; ++i)
	{
		host_ids[i] = ids + (i*MAX_HOST_ID_LENGTH);
	}

	int ret = ucloud_uhost_create(param, host_ids, &count);
	if (ret != UCLOUDE_OK)
	{
		ucloud_http_params_deinit(param);
		return 0;
	}
	ucloud_http_params_deinit(param);
	//
	for (i = 0; i<count; ++i)
	{
		fprintf(stderr, "%d host id=%s\n", i, host_ids[i]);
	}
	return 0;
}

int test_image_describe()
{
	ucloud_uimageset_t *imageset;
	imageset = ucloud_uimageset_init();
	ucloud_uimage_describe(imageset, "cn-north-03");
	ucloud_uimageset_deinit(imageset);
	return 0;
}

