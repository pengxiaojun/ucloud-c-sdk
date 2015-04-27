#include "test_ucloud.h"
#include <ucloud.h>

static void test_allocate_eip();

int test_unet()
{
	test_allocate_eip();
	return 0;
}

static void test_allocate_eip()
{
	ucloud_unet_allocate_eip_param_t param;
	memset(&param, 0, sizeof(param));
	strcpy(param.region, "cn-region-03");
	param.operator_name = uit_BGP;
	param.bandwidth = 100; //MB
	param.charge_type = uct_month;
	param.quantity = 1;
	ucloud_unet_eip_t *eip = ucloud_unet_allocate_eip(&param);
	if (eip == NULL)
	{
		fprintf(stderr, "Allocate eip failure:%s\n", ucloud_get_last_error());
		return;
	}
	ucloud_unet_eip_t *head;
	head = eip;

	while (head)
	{
		fprintf(stderr, "EIPId: %s\n", eip->id);	
		ucloud_unet_eip_addr_t *addr;
		addr = head ->addr;
		while (addr)
		{
			fprintf(stderr, "Operator name:%s IP: %s\n", ucloud_from_operator_name(addr->type), addr->ip);
			addr = addr->next;
		}
		head = head->next;
	}
	ucloud_unet_eipset_deinit(eip);
}
