#include "test_ucloud.h"
#include <uhttp.h>

#define UCLOUD_TEST_PUBLIC_KEY  "ucloudsomeone@example.com1296235120854146120"
#define UCLOUD_TEST_PRIVATE_KEY "46f09bb9fab4f12dfc160dae12273d5332b5debe"

int test_signature()
{
	//oonstrut parameter
	ucloud_http_params_t *params;
	params = ucloud_http_params_init();
	ucloud_http_params_add(params, "Action", "CreateUHostInstance");
	ucloud_http_params_add(params, "Region", "cn-north-01");
	ucloud_http_params_add(params, "ImageId", "f43736e1-65a5-4bea-ad2e-8a46e18883c2");
	ucloud_http_params_add(params, "LoginMode", "Password");
	ucloud_http_params_add_int(params, "CPU", 2);
	ucloud_http_params_add_int(params, "Memory", 2048);
	ucloud_http_params_add_int(params, "DiskSpace", 10);
	ucloud_http_params_add_int(params, "Quantity", 1);
	ucloud_http_params_add(params, "Name", "Host01");
	ucloud_http_params_add(params, "Password", "UCloudexample01");
	ucloud_http_params_add(params, "ChargeType", "Month");
	ucloud_http_params_add(params, "PublicKey", UCLOUD_TEST_PUBLIC_KEY);
	
	char signature[41] = {0};
	char *private_key = UCLOUD_TEST_PRIVATE_KEY;
	//do signature
	ucloud_uhttp_param_signature(params, private_key, signature);

   	printf("Signature=%s\n", signature);
	ucloud_http_params_deinit(params);
	return 0;
}
