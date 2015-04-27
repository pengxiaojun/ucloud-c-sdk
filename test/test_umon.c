#include "test_ucloud.h"
#include <time.h>
#include <ucommon.h>
#include <ucloud.h>

static void test_get_metric();

int test_umon()
{
	test_get_metric();
	return 0;
}

static void test_get_metric()
{
	ucloud_umon_t *umon;
	umon = ucloud_umon_init();
	umon->region = strdup("cn-region-03");
	umon->resource_id = strdup("yy02-host2");
	umon->res_type = urt_uhost;
	umon->res_metric = uhm_NetworkIn | uhm_NetworkOut | uhm_CPUUtilization;

	if (UCLOUDE_OK != ucloud_umon_get_metric(umon))
	{
		fprintf(stderr, "Get Metric failure: %s\n", ucloud_get_last_error());
	}
	else
	{
		int i, j;
		fprintf(stderr, "Metric result: %d\n", umon->nresult);
		for (i = 0; i<umon->nresult; ++i)
		{
			ucloud_umon_result_t *result = &umon->results[i];
			fprintf(stderr, "%s(%d):\n", ucloud_umon_get_metric_name(umon->res_type, result->res_metric), result->nmetric);
			char strtime[32]={0};
			time_t stamp;
			struct tm *t;
			for (j = 0; j<result->nmetric; ++j)
			{
				stamp = (time_t)result->metrics[j].timestamp;
				t = localtime(&stamp);
				strftime(strtime, sizeof(strtime), "%Y-%m-%d %T", t);
				fprintf(stderr, "  Time:%s value:%.2f\n", strtime, result->metrics[j].value);
			}
		}
	}
	ucloud_umon_deinit(umon);
}
