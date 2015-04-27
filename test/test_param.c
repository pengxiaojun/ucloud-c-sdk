#include "test_ucloud.h"
#include <uinclude.h>
#include <uparams.h>
#include <uerror.h>

int test_param()
{
	ucloud_http_params_t *params;
	int ret;

	params = ucloud_http_params_init();
	if (params == NULL){
		fprintf(stderr, "ucloud_http_params_init() error\n");
		return 0;
	}

	ucloud_http_params_add(params, "Action", "CreateUHostInstance");
	ucloud_http_params_add(params, "CPU", "1");
	ucloud_http_params_add(params, "Charge", "month");
	ucloud_http_params_add(params, "DiskSpace", "10");

	int querystr_len = ucloud_http_params_querystr_len(params);
	if (querystr_len == 0){
		fprintf(stderr, "param is empty\n");
		return 0;
	}
	char *querystr = calloc(1, querystr_len);
	ret = ucloud_http_params_querystr(params, querystr, &querystr_len);
	if (ret != UCLOUDE_OK)
	{
		if (ret == UCLOUDE_ERROR)
		{
			fprintf(stderr, "insuffent param len %d\n", querystr_len);
			return 0;
		}
	}

	fprintf(stderr, "query str: %s\n", querystr);

	ucloud_http_params_deinit(params);
	return 0;
}
