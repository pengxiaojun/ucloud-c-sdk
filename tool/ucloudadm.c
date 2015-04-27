#include <uinclude.h>
#include <ucommon.h>
#include <uhost.h>
#include <uhttp.h>
#include <uimage.h>
#include <uparams.h>
#include <umonitor.h>
#include <unet.h>
#include <time.h>

#define MAX_LINE_LEN 128
#define MAX_HOST_INSTANCE  128
#define MAX_HOST_ID_LENGTH 64

static void startup();
static void cleanup();

/* utility */
static int get_str_val(const char *indent, const char *def, char *str);
static int get_int_val(const char *indent, int def, int *num);
static int confirm(const char *indent);
static void help();
static void quit();

/*region*/
static void choice_resource();
static void describe_image();
static void region_info();
static int get_region(char *region);
static int get_image(char *image);

/* host */
static int get_host_id(char *host_id);
static void create_host();
static void describe_host();
static void terminate_host();
static void start_host();
static void stop_host();
static void reboot_host();
static void ssr_host(int act);
static void resize_host();
static void reinstall_host();
static void reset_passwd();
static void modify_name();
static void modify_tag();
static void modify_remark();
static void modify_ntr(int act);
static void get_price();
static void get_vnc();
static void create_image();
static void terminate_image();

/* monitor */
static void get_metric();
static int choice_metric(int res_type, int *metric);

/* unet utitlity */
static int get_eip_id(char *eip);
/* unet */
static void allocate_eip();
static void describe_eip();
static void update_eip_attr();
static void release_eip();
static void bind_eip();
static void unbind_eip();
static void modify_eip_bandwidth();
static void modify_eip_weight();
static void get_eip_price();
static void allocate_vip();
static void describe_vip();
static void release_vip();
static void create_sg();
static void update_sg();
static void delete_sg();
static void describe_sg();
static void describe_sg_res();
static void grant_sg();

static const char *regions[] = {NULL, "cn-north-01", "cn-north-02", "cn-north-03", "cn-east-01", "cn-south-01", "hk-01", "us-west-01"};

typedef void (*command_proc_t)(void);
typedef struct
{
	command_proc_t proc;
	char command[32];
	char abbrev[16];
	char desc[128];
}ucloud_func_table_t;

static const ucloud_func_table_t ucloud_func_table[] = 
{
	{help, "help", "h", "show help message"},
	{quit, "quit", "q", "quit"},
	{choice_resource, "choice_resource", "cr", "choice resource"},
	{region_info, "region_info", "ri", "show current region information"},
	{describe_image, "describe_image", "di", "uhost: describe image"},
	{create_host, "create_host", "ch", "uhost: create host"},
	{terminate_host, "terminate_host", "th", "uhost: terminate host"},
	{describe_host, "describle_host", "dh", "uhost: describe host"},
	{start_host, "start_host", "start", "uhost: start host"},
	{stop_host, "stop_host", "stop", "uhost: stop host"},
	{reboot_host, "reboot_host", "reboot", "uhost: reboot host"},
	{resize_host, "resize_host", "resize", "uhost: resize host(CPU,Disk,Memory)"},
	{reinstall_host, "reinstall_host", "reinst", "uhost: reinstall host"},
	{reset_passwd, "reset_passwd", "rp", "uhost: reset host password"},
	{modify_name, "mod_host_name", "mhn", "uhost: modify host name"},
	{modify_tag, "mod_host_tag", "mht", "uhost: modify host tag"},
	{modify_remark, "mod_host_remark", "mhr", "uhost: modify host remark"},
	{get_price, "get_price", "gp", "uhost: get host price"},
	{get_vnc, "get_vnc", "gv", "uhost: get host vnc information"},
	{create_image, "create_image", "ci", "uhost: create custom image"},
	{terminate_image, "terminate_image", "ti", "uhost: terminate custom image"},
	{get_metric, "get_metric", "gm", "umonitor: get metric"},
	{allocate_eip, "allocate_eip", "ae", "unet: allocate eip"},
	{describe_eip, "describe_eip", "de", "unet: describe eip"},
	{update_eip_attr, "update_eip_attr", "uea", "unet: update eip attribute(Tag, Name,Remark)"},
	{release_eip, "release_eip", "re", "unet: release eip"},
	{bind_eip, "bind_eip", "be", "unet: bind eip"},
	{unbind_eip, "ubind_eip", "ue", "unet: unbind eip"},
	{modify_eip_bandwidth, "mod_eip_bandwidth", "meb", "unet: modify eip bandwidth"},
	{modify_eip_weight, "mod_eip_weight", "mew", "unet: modify eip weight"},
	{get_eip_price, "get_eip_price", "gep", "unet: get eip price"},
	{allocate_vip, "allocate_vip", "av", "unet: allocate vip"},
	{describe_vip, "describe_vip", "dv", "unet: describe vip"},
	{release_vip, "release_vip", "rv", "unet: release vip"},
	{create_sg, "create_sg", "csg", "unet: create security group"},
	{update_sg, "update_sg", "usg", "unet: update security group"},
	{delete_sg, "delete_sg", "dsg", "unet: delete security group"},
	{describe_sg, "describe_sg", "dbsg", "unet: describe security group"},
	{describe_sg_res, "describe_sg_res", "dsr", "unet: describe security group resource"},
	{grant_sg, "grant_sg", "gsg", "unet: grant security group"},
};

typedef struct
{
	char region[32];
	ucloud_uimageset_t *imageset;
}ucloud_resource_t;

/* global resorce */
ucloud_resource_t *resource = NULL;
ucloud_uhost_t *head_host = NULL;
ucloud_unet_eip_t *head_eip = NULL;
bool forever = true;

int main(int argc, const char *argv[])
{
	int i;
	char buf[MAX_LINE_LEN] = {0};
	fprintf(stderr, "Welcome to use ucloud command tool, Type 'help' to view a list of commands.\n");
	startup();

	while (forever)
	{
		fprintf(stderr, "(ucloud) ");
		fflush(stderr);
		
		if (fgets(buf, MAX_LINE_LEN-1, stdin) == NULL)
		{
			break;
		}
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		//lookup ucloud function table
		for (i = 0; i<sizeof(ucloud_func_table)/sizeof(ucloud_func_table[0]); ++i)
		{
			if (strcmp(buf, ucloud_func_table[i].command) == 0 ||
				(strcmp(buf, ucloud_func_table[i].abbrev) == 0 && strlen(buf) == strlen(ucloud_func_table[i].abbrev)))
			{
				ucloud_func_table[i].proc();
				break;
			}
		}
	}
	cleanup();
	return 0;
}

static void startup()
{
	resource = (ucloud_resource_t*)calloc(1, sizeof(*resource));
	resource->imageset = ucloud_uimageset_init();
	//set default region
	strcpy(resource->region, regions[3]);
	ucloud_uimage_describe(resource->imageset, resource->region);
	fprintf(stderr, "Set default region:%s. You can run 'cr' to swith region.\n", resource->region);
}

static void cleanup()
{
	ucloud_uimageset_deinit(resource->imageset);
	free(resource);
}

static void help()
{
	int i;
	for (i = 0; i<sizeof(ucloud_func_table)/sizeof(ucloud_func_table[0]); ++i)
	{
		fprintf(stderr, "%18s|%-10s %-30s\n", ucloud_func_table[i].command, ucloud_func_table[i].abbrev, ucloud_func_table[i].desc);
	}
}

static void quit()
{
	forever = false;
}

static void region_info()
{
	fprintf(stderr, "Current region '%s'\n", resource->region);
}

static void choice_resource()
{
	char region[MAX_LINE_LEN] = {0};
	if (-1 == get_region(region))
	{
		fprintf(stderr, "Invalid region '%s'\n", region);
		return;
	}
	//save current region
	strcpy(resource->region, region);
	//free last imageset
	ucloud_uimageset_deinit(resource->imageset);	
	resource->imageset = ucloud_uimageset_init();
	ucloud_uimage_describe(resource->imageset, resource->region);
	fprintf(stderr, "Region '%s' selected.\n", resource->region);
}

static int get_region(char *region)
{
	int i;
	fprintf(stderr, "Availiable regions:\n");
	for (i = 1; i<sizeof(regions)/sizeof(regions[1]); ++i)
	{
		fprintf(stderr, "%d:%s\n", i, regions[i]);
	}
	fprintf(stderr, "Choice Region(1):");
	fflush(stdin);
	if (fgets(region, MAX_LINE_LEN, stdin) == NULL)
	{
		return -1;
	}

	//trim \n
	if (region[strlen(region)-1] == '\n')
	{
		region[strlen(region)-1] = '\0';
	}
	if (strlen(region) == 0)
	{
		strcpy(region, regions[1]);
	}
	else
	{
		int pos = atoi(region);
		if (pos <= 0 || pos > sizeof(regions)/sizeof(regions[1]))
		{
			return -1;
		}	
		strcpy(region, regions[pos]);
	}
	return 0;
}

static int get_image(char *imageid)
{
	describe_image();
	fprintf(stderr, "Select a image in region '%s'(1):", resource->region);
	fflush(stdin);
	if (fgets(imageid, MAX_LINE_LEN, stdin) == NULL)
	{
		return -1;
	}

	ucloud_uimage_t *image;
	image= resource->imageset->head;

	if (image == NULL){
		fprintf(stderr, "You need execute 'di' to describe all image at first\n");
		return -1;
	}

	//trim \n
	if (imageid[strlen(imageid)-1] == '\n')
	{
		imageid[strlen(imageid)-1] = '\0';
	}
	if (strlen(imageid) == 0)
	{
		strcpy(imageid, image->id);
	}
	else
	{
		int i = 0;
		int pos = atoi(imageid);
		while (image != NULL)
		{
			if (++i == pos)
			{
				break;
			}
			image = image->next;
		}
		if (i == pos)
		{
			strcpy(imageid, image->id);
		}
		else
		{
			fprintf(stderr, "Invalid image\n");
			return -1;
		}
	}
	return 0;
}

static int get_str_val(const char *indent, const char *def, char *str)
{
	fprintf(stderr, "%s", indent);
	fflush(stdin);
	if (fgets(str, MAX_LINE_LEN, stdin) == NULL)
	{
		return -1;
	}

	//trim \n
	if (str[strlen(str)-1] == '\n')
	{
		str[strlen(str)-1] = '\0';
	}
	if (strlen(str) == 0)
	{
		if (def) strcpy(str, def);
		else return -1;
	}
	return 0;
}

static int get_int_val(const char *indent, int def, int *num)
{
	char str[MAX_LINE_LEN] = {0};
	char defstr[MAX_LINE_LEN] = {0};
	sprintf(defstr, "%d", def);
	if (-1 == get_str_val(indent, def == 0 ? NULL : defstr, str))
	{
		return -1;
	}
	unsigned long int ul = strtoul(str, NULL, 10);
	if (ul == ULONG_MAX)
	{
		return -1;
	}
	*num = (int)ul; //XXX:May be missing data
	return 0;
}

static int confirm(const char *indent)
{
	char str[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val(indent, NULL, str))
	{
		return -1;
	}
	if (strcmp(str, "Yes") == 0||
		strcmp(str, "Y") == 0)
	{
		return 1;
	}
	if (strcmp(str, "No") == 0 ||
		strcmp(str, "N") == 0)
	{
		return 0;
	}
	else
	{
		fprintf(stderr, "Unrecognized input, try input 'Y' or 'N'\n");
	}
	return 0;
}

static void describe_image()
{
	int i=0;
	fprintf(stderr, "List all image in '%s'\n", resource->region);
	ucloud_uimage_t *image;
	image= resource->imageset->head;

	while (image != NULL)
	{
		fprintf(stderr, "%d:Image Id=%s name=%s\n", ++i, image->id, image->name);
		image = image->next;
	}
}

static int get_host_id(char *host_id)
{
	int i = 0;
	fprintf(stderr, "List all host instance in region %s\n", resource->region);
	ucloud_uhost_t *host = head_host;
	while (host != NULL)
	{
		fprintf(stderr, "%d: %s\n", ++i, host->id);
		host = host->next;
	}

	int val;
	i = 0;
	if (-1 == get_int_val("Choice a host:", 0, &val))
	{
		return -1;
	}
	host = head_host;

	while (host != NULL)
	{
		if (++i == val)
		{
			strcpy(host_id, host->id);
			break;
		}
		host = host->next;
	}
	if (i != val)
	{
		fprintf(stderr, "Invalid input host\n");
		return -1;
	}
	return 0;
}

static void create_host()
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();

	ucloud_uhost_param_mask_t mask;
	memset(&mask, 0, sizeof(mask));
	mask.region = 1;
	ucloud_uhost_set_str_param(param, &mask, resource->region);
	//set image
	char val[MAX_LINE_LEN] = {0};
	if (-1 == get_image(val))
	{
		return;
	}
	memset(&mask, 0, sizeof(mask));
	mask.image_id = 1;
	ucloud_uhost_set_str_param(param, &mask, val); 
	fprintf(stderr, "Set image id: %s\n", val);

	//set password
	memset(&mask, 0, sizeof(mask));
	mask.login_mode = 1;
	ucloud_uhost_set_int_param(param, &mask, uulm_passwd);
	if (-1 == get_str_val("Input Password:", "", val))
	{
		return;
	}
	memset(&mask, 0, sizeof(mask));
	mask.password = 1;
	ucloud_uhost_set_str_param(param, &mask, "passwd");

	//set number of cpu
	int num = 4;
	get_int_val("Input number of CPU(4):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.cpu = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set disk capacity
	num = 60;
	get_int_val("Input disk capacity(60G):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.disk_space = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set memcry size
	num = 8192;
	get_int_val("Input memory size(8192M):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.memory = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set host name
	get_str_val("Input host name(UHost):", "UHost", val);
	memset(&mask, 0, sizeof(mask));
	mask.name = 1;
	ucloud_uhost_set_str_param(param, &mask, val);

	//set charge type
	num = 2;
	get_int_val("Select charge type[1:Year 2:Month(default), 3:Dynamic 4:Trail]:", num, &num);	
	memset(&mask, 0, sizeof(mask));
	mask.charge_type = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set quantity
	get_int_val("Select quantity(1):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.quantity = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

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
		fprintf(stderr, "Create host failure\n");
		ucloud_http_params_deinit(param);
		return;
	}
	ucloud_http_params_deinit(param);
	fprintf(stderr, "Create success\n");
	for (i = 0; i<count; ++i)
	{
		fprintf(stderr, "%d Host id=%s\n", i, host_ids[i]);
	}
}

static void describe_host()
{
	if (head_host)
	{
		ucloud_uhost_describe_deinit(head_host);
	}
	char host_id[MAX_LINE_LEN] = {0};
	get_str_val("Input host id(All):", NULL, host_id);
	head_host = ucloud_uhost_describe(resource->region, host_id);
	if (head_host == NULL)
	{
		fprintf(stderr, "Describe host instance failure %s\n", ucloud_get_last_error());
		return;
	}

	ucloud_uhost_t *host = head_host;
	while (host != NULL)
	{
		fprintf(stderr, "------------------------Basic info--------------------------\n");
		fprintf(stderr, " Host id:%s\
				\n Host type: %s\
				\n Image Id: %s\
				\n Basic Image Id: %s\
				\n Basic Image Name: %s\
				\n Host Name: %s\
				\n Tag: %s\
				\n Remark: %s\
				\n Status: %s\
				\n CPU: %d\
				\n Memory: %dM\n",
				host->id,
				ucloud_from_uhost_type(host->type),
				host->image_id,
				host->basic_image_id,
				host->basic_image_name,
				host->name,
				host->tag,
				host->remark,
				ucloud_from_uhost_status(host->status),
				host->cpu,
				host->memory);

		fprintf(stderr, "-----------------------Disk Set-----------------------------\n");
		ucloud_disk_t *disk = host->diskset;
		while (disk != NULL)
		{
			fprintf(stderr, " Type=%s Id=%s Size=%dG\n", 
					ucloud_from_disk_type(disk->type),
					disk->id,
					disk->capacity);
			disk = disk->next;
		}

		fprintf(stderr, "-----------------------IP Set-------------------------------\n");
		ucloud_ip_t *ip = host->ipset;
		while (ip != NULL)
		{
			fprintf(stderr, " Type=%s IP=%s Id=%s Bandwidth=%dMb\n", 
					ucloud_from_ip_type(ip->type),
					ip->ip,
					ip->id,
					ip->bandwidth);
			ip = ip->next;
		}
		host = host->next;
	}
}

static void terminate_host()
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}
	char hint[MAX_LINE_LEN] = {0};
	sprintf(hint, "Confirm to terminate host %s?[Y/N]:", host_id);
	if (confirm(hint))
	{
		int ret;
		ret = ucloud_uhost_terminate(resource->region, host_id);
		fprintf(stderr, "Terminate host %s %s\n", host_id, ret == UCLOUDE_OK ? "success" : "failure");
	}
}

static void start_host()
{
	ssr_host(uuis_start);
}

static void stop_host()
{
	ssr_host(uuis_stop);
}

static void reboot_host()
{
	ssr_host(uuis_reboot);
}

static void ssr_host(int act)
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}
	char act_str[8] = {0};
	if (act == uuis_start)
		strcpy(act_str, "Start");
	else if(act == uuis_stop)
		strcpy(act_str, "Stop");
	else if (act == uuis_reboot)
		strcpy(act_str, "Reboot");


	if (act != uuis_start)
	{
		char hint[MAX_LINE_LEN] = {0};
		sprintf(hint, "Confirm to %s host %s?[Y/N]:", act_str, host_id);
		if (!confirm(hint))
		{
			return;
		}
	}
	int ret = ucloud_uhost_ssr(act, resource->region, host_id);
	fprintf(stderr, "%s host %s %s\n", act_str, host_id, ret == UCLOUDE_OK ? "success" : "failure");
}

static void resize_host()
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_uhost_param_mask_t mask;

	//set number of cpu
	int cpu, disk, memory;
	bool cpu_flag, disk_flag, memory_flag;

	cpu = disk = memory = 0;
	cpu_flag = disk_flag = memory_flag = false;

	if (-1 != get_int_val("Input number of CPU(Press Enter to skip):", cpu, &cpu))
	{
		memset(&mask, 0, sizeof(mask));
		mask.cpu = 1;
		ucloud_uhost_set_int_param(param, &mask, cpu);
		cpu_flag = true;
	}

	//set disk capacity
	if (-1 != get_int_val("Input disk capacity(GB)(Press Enter to skip):", disk, &disk))
	{
		memset(&mask, 0, sizeof(mask));
		mask.disk_space = 1;
		ucloud_uhost_set_int_param(param, &mask, disk);
		disk_flag = 1;
	}
	//set memcry size
	if (-1 != get_int_val("Input memory size(MB)(Press Enter to skip):", memory, &memory))
	{
		memset(&mask, 0, sizeof(mask));
		mask.memory = 1;
		ucloud_uhost_set_int_param(param, &mask, memory);
		memory_flag = true;
	}

	char hint[128] = {0};
	char cpu_hint[32] = {0};
	char disk_hint[32] = {0};
	char memory_hint[32] = {0};
	if (cpu_flag) sprintf(cpu_hint, "CPU=%d", cpu);
	if (disk_flag) sprintf(disk_hint, "Disk=%dG", disk);
	if (memory_flag) sprintf(memory_hint, "Memory=%dM", memory);

	sprintf(hint, "Confirm to set host %s %s %s %s [Y/N]:", host_id, cpu_hint, disk_hint, memory_hint);
	if (confirm(hint))
	{
		if (UCLOUDE_OK != ucloud_uhost_resize(param, resource->region, host_id))
		{
			fprintf(stderr, "Reisze host %s error:%s\n", host_id, ucloud_get_last_error());
		}
		else
		{
			fprintf(stderr, "Resize host %s suceess\n", host_id);
		}
	}
	//set host name
}

static void reinstall_host()
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}

	ucloud_http_params_t *param;
	param = ucloud_http_params_init();
	ucloud_uhost_reinstall_param_mask_t mask;

	char str[MAX_LINE_LEN] = {0};
	if (-1 != get_str_val("Input host password:", str, str))
	{
		memset(&mask, 0, sizeof(mask));
		mask.password = 1;
		ucloud_uhost_set_reinstall_param(param, &mask, str);
	}
	if (-1 != get_image(str))
	{
		memset(&mask, 0, sizeof(mask));
		mask.image_id = 1;
		ucloud_uhost_set_reinstall_param(param, &mask, str);
	}
	if (-1 != get_str_val("Reserve disk ? [Yes|No](Press Enter to skip):", NULL, str))
	{
		memset(&mask, 0, sizeof(mask));
		mask.reserve_disk = 1;
		ucloud_uhost_set_reinstall_param(param, &mask, str);
	}
	if (UCLOUDE_OK == ucloud_uhost_reinstall(param, resource->region, host_id))
	{
		fprintf(stderr, "Resintall host %s success\n", host_id);
	}
	else
	{
		fprintf(stderr, "Resintall host %s failure: %s\n", host_id, ucloud_get_last_error());
	}
}

static void reset_passwd()
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}

	char passwd[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input new password:", NULL, passwd))
	{
		fprintf(stderr, "Invalid password\n");
		return;
	}
	if (UCLOUDE_OK == ucloud_uhost_reset_passwd(resource->region, host_id, passwd))
	{
		fprintf(stderr, "Reset host %s password success, New password is %s\n", host_id, passwd);
	}
	else
	{
		fprintf(stderr, "Reset host %s password failure: %s\n", host_id, ucloud_get_last_error());
	}
}

static void modify_name()
{
	modify_ntr(uun_name);
}

static void modify_tag()
{
	modify_ntr(uun_tag);
}

static void modify_remark()
{
	modify_ntr(uun_remark);
}

static void modify_ntr(int act)
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}

	char name[MAX_LINE_LEN] = {0};
	char hint[64] = {0};
	if (act == uun_name)
	{
		sprintf(hint, "Input new host name:");	
	}
	else if (act == uun_tag)
	{
		sprintf(hint, "Input new tag:");	
	}
	else if (act == uun_remark)
	{
		sprintf(hint, "Input new remark:");	
	}
	if (-1 == get_str_val(hint, NULL, name))
	{
		fprintf(stderr, "Invalid input\n");
		return;
	}
	if (UCLOUDE_OK == ucloud_uhost_ntr(act, resource->region, host_id, name))
	{
		fprintf(stderr, "Modify host %s success\n", host_id);
	}
	else
	{
		fprintf(stderr, "Modify %s failure: %s\n", host_id, ucloud_get_last_error());
	}
}

static void get_price()
{
	ucloud_http_params_t *param;
	param = ucloud_http_params_init();

	//set region
	fprintf(stderr, "Current region %s, Press 'cr' to swith region\n", resource->region);
	ucloud_uhost_param_mask_t mask;
	memset(&mask, 0, sizeof(mask));
	mask.region = 1;
	ucloud_uhost_set_str_param(param, &mask, resource->region);

	//set image
	char val[MAX_LINE_LEN] = {0};
	if (-1 == get_image(val))
	{
		return;
	}
	memset(&mask, 0, sizeof(mask));
	mask.image_id = 1;
	ucloud_uhost_set_str_param(param, &mask, val); 
	fprintf(stderr, "Set image id: %s\n", val);

	//set number of cpu
	int num;
	get_int_val("Input number of CPU:", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.cpu = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set memcry size
	get_int_val("Input memory size(MB):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.memory = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set count of host
	get_int_val("Input count:", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.count = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	//set charge type
	if (-1 != get_int_val("Select charge type(All)[1:Year 2:Month, 3:Dynamic]:", 0, &num))
	{
		memset(&mask, 0, sizeof(mask));
		mask.charge_type = 1;
		ucloud_uhost_set_int_param(param, &mask, num);
	}

	//set disk capacity
	num = 60;
	get_int_val("Input disk capacity(GB):", num, &num);
	memset(&mask, 0, sizeof(mask));
	mask.disk_space = 1;
	ucloud_uhost_set_int_param(param, &mask, num);

	ucloud_uhost_price_t* price;
	price = ucloud_uhost_get_price(param);
	
	if (price == NULL)
	{
		fprintf(stderr, "Get price failure: %s\n", ucloud_get_last_error());
	}
	else
	{
		while (price != NULL)
		{
			fprintf(stderr, "Charge Type=%s price=%f\n", 
					ucloud_from_charge_type(price->charge_type),
					price->price);
			price = price->next;
		}
		ucloud_uhost_price_deinit(price);
	}
}

static void get_vnc()
{
	char host_id[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(host_id))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}

	ucloud_uhost_vnc_t *vnc = ucloud_uhost_get_vnc(resource->region, host_id);

	if (vnc == NULL)
	{
		fprintf(stderr, "Get host %s vnc failure: %s\n", host_id, ucloud_get_last_error());
	}
	else
	{
		fprintf(stderr, "Host %s vnc ip=%s port=%d password=%s\n", host_id, vnc->ip, vnc->port, vnc->passwd);
		ucloud_uhost_vnc_deinit(vnc);
	}
}

static void create_image()
{
	char val[MAX_LINE_LEN] = {0};
	if (-1 == get_host_id(val))
	{
		fprintf(stderr, "Invalid host id\n");
		return;
	}
	ucloud_custom_uimage_t *image;
	image = ucloud_custom_uimage_init();
	image->region = strdup(resource->region);
	image->host_id = strdup(val);

	if (-1 == get_str_val("Input custom image name:", NULL, val))
	{
		fprintf(stderr, "Invalid custom image name\n");
		ucloud_custom_uimage_deinit(image);
		return;
	}
	image->name = strdup(val);

	if (-1 != get_str_val("Input custom image description:", "", val))
	{
		image->desc = strdup(val);
	}
	if (UCLOUDE_OK != ucloud_uimage_create(image))
	{
		fprintf(stderr, "Create custom image failure: %s\n", ucloud_get_last_error());
	}
	else
	{
		fprintf(stderr, "Create custom image %s suceess\n", image->id);
	}
	ucloud_custom_uimage_deinit(image);

}

static void terminate_image()
{
	char image_id[MAX_LINE_LEN] = {0};
	if (-1 == get_image(image_id))
	{
		fprintf(stderr, "Invalid image id\n");
		return;
	}
	if(UCLOUDE_OK != ucloud_uimage_terminate(resource->region, image_id))
	{
		fprintf(stderr, "Terminate custome image %s failure: %s\n", image_id, ucloud_get_last_error());
	}
	else
	{
		fprintf(stderr, "Terminate custome image %s success\n", image_id);
	}
}

static void get_metric()
{
	int res_type = urt_uhost;
	if (-1 == get_int_val("Choice metric resource[1:uhost 2:udb 3:ulb 4:umem]:", res_type, &res_type))
	{
		fprintf(stderr, "Invalid metric resource type\n");
		return;
	}
		
	int res_metric = 0;
	char res_id[MAX_LINE_LEN] = {0};
	if (res_type == urt_uhost)
	{
		if (-1 == get_host_id(res_id))
		{
			fprintf(stderr, "Invalid resource id\n");
			return;
		}

		if (-1 == choice_metric(res_type, &res_metric))
		{
			fprintf(stderr, "Invalid metric\n");
			return;
		}
	}
	else{
		fprintf(stderr, "Not support resource type\n");
		return;
	}
	ucloud_umon_t *umon;
	umon = ucloud_umon_init();
	umon->region = strdup(resource->region);
	umon->resource_id = strdup(res_id);
	umon->res_type = res_type;
	umon->res_metric = res_metric;

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

static int parse_metric(char *metric, int len, int *item, int *nitem)
{
	int num = 0;
	//except
	char *pos = metric;
	if (*pos == '^')
	{
		pos++; //skip ^
		unsigned long int exclude = strtoul(pos, NULL, 10);
		if (exclude == ULONG_MAX)
		{
			return -1;
		}
		int i;
		for (i = 0; i<len; i++)
		{
			if (i != exclude){/*fprintf(stderr, "--:%d\n", i);*/ item[num++] = i;}
		}
	}
	else
	{
		char *sep = strchr(pos, '-');
		if (sep)
		{
			*sep = '\0';
			unsigned long int start = strtoul(pos, NULL, 10);
			unsigned long int stop = strtoul(sep+1, NULL, 10);
			if (start != ULONG_MAX && stop != ULONG_MAX)
			{
				while (start <= stop && start < len)
				{
					item[num++] = start++;
				}
			}
			else{
				return -1;
			}
		}
		else
		{
			unsigned long int start = strtoul(pos, NULL, 10);
			if (start != ULONG_MAX && start < len)
			{
				item[num++] = start;
			}
		}
	}
	*nitem = num;
	return 0;
}

static int choice_metric(int res_type, int *metric)
{
	int i;
	if (res_type == urt_uhost)
	{
		const char* uhost_metric_name[] = {"Network In", "Network Out", "CPU Utilization", "IO Read", "IO Write", "Disk Read Ops", "NIC In", "NIC Out", "Memory Usage", "Data Sapce Usage", "Root Space Usage", "Read Only Disk Count", "Runnable Process Count", "Block Process Count"};
		const int uhost_metric_value[] = {uhm_NetworkIn, uhm_NetworkOut, uhm_CPUUtilization, uhm_IORead, uhm_IOWrite, uhm_DiskReadOps, uhm_NICIn, uhm_NICOut, uhm_MemUsage, uhm_DataSpaceUsage, uhm_RootSpaceUsage, uhm_ReadonlyDiskCount, uhm_RunnableProcessCount, uhm_BlockProcessCount};
		fprintf(stderr, "UHost metric:");
		for (i = 0; i<sizeof(uhost_metric_name)/sizeof(uhost_metric_name[0]); ++i)
		{
			fprintf(stderr, "%2d: %s\n", i, uhost_metric_name[i]);
		}
		char metric_val[MAX_LINE_LEN] = {0};
		if (-1 == get_str_val("Choice uhost metric[0-n|^n]:", NULL, metric_val))
		{
			fprintf(stderr, "Invalid metric");
			return -1;
		}
		//parse input
		int items[MAX_LINE_LEN];
		int nitem = 0;
		if (-1 == parse_metric(metric_val, sizeof(uhost_metric_name)/sizeof(uhost_metric_name[0]), items, &nitem))
		{
			return -1;
		}
		for (i = 0; i<nitem; ++i)
		{
			*metric = *metric | uhost_metric_value[items[i]];
		}
	}
	else if (res_type == urt_udb)
	{

	}
	else if (res_type == urt_ulb)
	{

	}
	else if (res_type == urt_umem)
	{

	}
	else
	{
		return -1;
	}
	return 0;
}

/***************************unet************************/
static int get_eip_id(char *eip_id)
{
	int i = 0;
	fprintf(stderr, "List all eip resource in region %s\n", resource->region);
	ucloud_unet_eip_t *eip = head_eip;
	while (eip != NULL)
	{
		fprintf(stderr, "%d: %s\n", ++i, eip->id);
		eip = eip->next;
	}

	int val;
	i = 0;
	if (-1 == get_int_val("Choice a eip:", 0, &val))
	{
		return -1;
	}
	eip = head_eip;

	while (eip!= NULL)
	{
		if (++i == val)
		{
			strcpy(eip_id, eip->id);
			break;
		}
		eip = eip ->next;
	}
	if (i != val)
	{
		fprintf(stderr, "Invalid input eip\n");
		return -1;
	}
	return 0;
}

static void allocate_eip()
{
	ucloud_unet_allocate_eip_param_t param;
	memset(&param, 0, sizeof(param));
	strcpy(param.region, resource->region);
	if (-1 == get_int_val("Choice operator[0:Telecom 1:Unicom 2:International 3:Bgp 4:Duplet]:", 0, &param.operator_name))
	{
		fprintf(stderr, "Invalid operator name\n");
		return;
	}
	if (-1 == get_int_val("Input Band width[0M-800M](Tip:Shared=0, Non-shared!=0):", 0, &param.bandwidth))
	{
		fprintf(stderr, "Invalid operator name\n");
		return;
	}

	ucloud_unet_eip_t *tmp_eip;
	ucloud_unet_eip_t *eip = ucloud_unet_allocate_eip(&param);
	if (eip == NULL)
	{
		fprintf(stderr, "Allocate eip failure:%s\n", ucloud_get_last_error());
		ucloud_unet_eipset_deinit(eip);
	}
	tmp_eip = eip;
	while (tmp_eip)
	{
		fprintf(stderr, "EIPId: %s\n", eip->id);	
		ucloud_unet_eip_addr_t *addr;
		addr = tmp_eip->addr;
		while (addr)
		{
			fprintf(stderr, "Operator name:%s IP: %s\n", ucloud_from_operator_name(addr->type), addr->ip);
			addr = addr->next;
		}
		tmp_eip = eip->next;
	}

	ucloud_unet_eipset_deinit(eip);
}

static void describe_eip()
{
	if (head_eip)
	{
		ucloud_unet_eipset_deinit(head_eip);
	}
	head_eip = ucloud_unet_describe_eip(resource->region);
	if (head_eip == NULL)
	{
		fprintf(stderr, "Describe eip failure:%s\n", ucloud_get_last_error());
	}
	ucloud_unet_eip_t *eip;
	eip = head_eip;
	while (eip)
	{
		fprintf(stderr, "-----------EIP Basic Information---------\n");	
		fprintf(stderr, "EIPId: %s\n", eip->id);
		fprintf(stderr, "Bandwidth: %d\n", eip->bandwidth);
		fprintf(stderr, "Bandwidth type: %s\n", ucloud_from_bandwidth_type(eip->bandwidth_type));
		fprintf(stderr, "Charge type: %s\n", ucloud_from_charge_type(eip->charge_type));	
		fprintf(stderr, "Create time: %d\n", eip->create_time);
		fprintf(stderr, "Expire time: %d\n", eip->expired_time);	
		fprintf(stderr, "Status: %s\n", ucloud_from_ip_status(eip->status));	
		fprintf(stderr, "Weight: %d\n", eip->weight);	
		fprintf(stderr, "Name: %s\n", eip->name);	
		fprintf(stderr, "Tag: %s\n", eip->tag);	
		fprintf(stderr, "Remark: %s\n", eip->remark);	

		ucloud_unet_eip_addr_t *addr;
		addr = eip->addr;
		fprintf(stderr, "-----------------EIPAddr----------------\n");	
		while (addr)
		{
			fprintf(stderr, "Operator name:%s IP: %s\n", ucloud_from_operator_name(addr->type), addr->ip);
			addr = addr->next;
		}

		ucloud_unet_resource_t *res;
		res = eip->res;
		fprintf(stderr, "-----------------Resource---------------\n");	
		while (res)
		{
			fprintf(stderr, "Resource type:%s Resource name: %s Resource id: %s\n", res->type, res->name, res->name);
			res = res->next;
		}
		eip = eip->next;
	}
}

static void update_eip_attr()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	bool modify_name, modify_tag, modify_remark;
	char name[MAX_LINE_LEN];
	char tag[MAX_LINE_LEN];
	char remark[MAX_LINE_LEN];
	modify_name = modify_tag = modify_remark = false;

	if (-1 != get_str_val("Input eip name:", NULL, name))
	{
		modify_name = true;	
	}
	if (-1 != get_str_val("Input eip tag:", NULL, tag))
	{
		modify_tag = true;	
	}
	if (-1 != get_str_val("Input eip remark:", NULL, remark))
	{
		modify_remark = true;	
	}
	if (UCLOUDE_OK == ucloud_unet_update_eip_attr(resource->region, eip_id,
			(modify_name ? name : NULL),
			(modify_tag ? tag : NULL),
			(modify_remark ? remark : NULL)))
	{
		fprintf(stderr, "update eip %s attribute success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "update eip %s attribute failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void release_eip()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	if (UCLOUDE_OK == ucloud_unet_release_eip(resource->region, eip_id))
	{
		fprintf(stderr, "Relase eip %s success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "Release eip %s failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void bind_eip()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	int res;
	if (-1 == get_int_val("Choice resource type[1:uhost 3:ulb 5:vrouter]:", 0, &res))
	{
		fprintf(stderr, "Invalid resource type\n");
		return;
	}
	char res_id[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input resource id:", NULL, res_id))
	{
		fprintf(stderr, "Invalid resource id\n");
		return;
	}

	if (UCLOUDE_OK == ucloud_unet_bind_eip(resource->region, eip_id, res, res_id))
	{
		fprintf(stderr, "Bind eip %s success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "Bind eip %s failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void unbind_eip()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	int res;
	if (-1 == get_int_val("Choice resource type[0:uhost 3:ulb 5:vrouter]:", 0, &res))
	{
		fprintf(stderr, "Invalid resource type\n");
		return;
	}
	char res_id[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input resource id:", NULL, res_id))
	{
		fprintf(stderr, "Invalid resource id\n");
		return;
	}

	if (UCLOUDE_OK == ucloud_unet_unbind_eip(resource->region, eip_id, res, res_id))
	{
		fprintf(stderr, "UnBind eip %s success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "UnBind eip %s failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void modify_eip_bandwidth()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	int bandwidth;
	if (-1 == get_int_val("Input bandwidth[MB]:", 0, &bandwidth))
	{
		fprintf(stderr, "Invalid bandwidth\n");
		return;
	}
	if (UCLOUDE_OK == ucloud_unet_modify_eip_bandwidth(resource->region, eip_id, bandwidth))
	{
		fprintf(stderr, "Modify eip %s bandwidth success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "Modify eip %s bandwidth failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void modify_eip_weight()
{
	char eip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_eip_id(eip_id))
	{
		return;
	}
	int weight;
	if (-1 == get_int_val("Input bandwidth:", 0,&weight))
	{
		fprintf(stderr, "Invalid bandwidth\n");
		return;
	}
	if (UCLOUDE_OK == ucloud_unet_modify_eip_weight(resource->region, eip_id, weight))
	{
		fprintf(stderr, "Modify eip %s bandwidth success\n", eip_id);
	}
	else
	{
		fprintf(stderr, "Modify eip %s bandwidth failure %s\n", eip_id, ucloud_get_last_error());
	}
}

static void get_eip_price()
{
	int oper_name;
	if (-1 == get_int_val("Choice operator[0:Telecom 1:Unicom 2:International 3:Bgp 4:Duplet]:", 0, &oper_name))
	{
		fprintf(stderr, "Invalid operator name\n");
		return;
	}
	int bandwidth;
	if (-1 == get_int_val("Input bandwidth[MB]:", 0,&bandwidth))
	{
		fprintf(stderr, "Invalid bandwidth\n");
		return;
	}
	int charge_type = 2;
	get_int_val("Select charge type[1:Year 2:Month(default), 3:Dynamic 4:Trail]:", charge_type, &charge_type);	

	ucloud_unet_eip_price_t *priceset;
	priceset = ucloud_unet_get_eip_price(resource->region, oper_name, bandwidth, charge_type);
	if (priceset)
	{
		ucloud_unet_eip_price_t *price = priceset;
		while (price)
		{
			fprintf(stderr, "Charge type: %s Price: %.2f Purchase Value: %d\n", ucloud_from_charge_type(price->charge_type), price->price, price->purchase_value);
			price = price->next;
		}
		ucloud_unet_eip_priceset_deinit(priceset);
	}
	else
	{
		fprintf(stderr, "Get eip price failure %s\n", ucloud_get_last_error());
	}
}

static void allocate_vip()
{
	int count = 1;
	if (-1 == get_int_val("Input vip count[1]:", count, &count))
	{
		fprintf(stderr, "Invalid count\n");
		return;
	}
	ucloud_unet_vip_t *vip;
	vip = ucloud_unet_allocate_vip(resource->region, count);
	if (vip)
	{
		fprintf(stderr, "VIP:\n");
		ucloud_unet_vip_t *head = vip;
		while (head)
		{
			fprintf(stderr, "%s\n", head->ip);
			head = head->next;
		}
		ucloud_unet_vip_deinit(vip);
	}
	else
	{
		fprintf(stderr, "Allocate vip failure %s\n", ucloud_get_last_error());
	}
}

static void describe_vip()
{
	ucloud_unet_vip_t *vip;
	vip = ucloud_unet_describe_vip(resource->region);
	if (vip)
	{
		fprintf(stderr, "VIP:\n");
		ucloud_unet_vip_t *head = vip;
		while (head)
		{
			fprintf(stderr, "%s\n", head->ip);
			head = head->next;;
		}
		ucloud_unet_vip_deinit(vip);
	}
	else
	{
		fprintf(stderr, "Describe vip failure %s\n", ucloud_get_last_error());
	}
}

static void release_vip()
{
	char vip_id[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input vip id:", NULL, vip_id))
	{
		fprintf(stderr, "Invalid vip id\n");
		return;
	}
	if (UCLOUDE_OK == ucloud_unet_release_vip(resource->region, vip_id))
	{
		fprintf(stderr, "Relase vip %s success\n", vip_id);
	}
	else
	{
		fprintf(stderr, "Release vip %s failure %s\n", vip_id, ucloud_get_last_error());
	}
}

static void create_sg()
{
	int ret;
	char name[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input security group name:", NULL, name))
	{
		fprintf(stderr, "Invalid security group name\n");
		return;
	}
	char desc[MAX_LINE_LEN] = {0};
	if (-1 == get_str_val("Input security group description:", NULL, desc))
	{
		fprintf(stderr, "Invalid security group description\n");
		return;
	}
	ucloud_unet_sg_t *sg;
	sg = ucloud_unet_sg_init();
	sg->name = strdup(name);
	sg->desc = strdup(desc);

	fprintf(stderr, "Rule format: Prot Dst_port Src_ip Action Priority\nExample: TCP 3306 0.0.0.0/0 DROP 50\n");
	do
	{
		char rule_str[MAX_LINE_LEN] = {0};
		if (-1 == get_str_val("Input Rule[Press 'Ctrl+D' to finish input]:", NULL, rule_str))
		{
			break;
		}
		char prot[8] = {0};
		char dst_port[MAX_LINE_LEN] = {0};
		char src_ip[MAX_LINE_LEN] = {0};
		char action[8] = {0};
		int priority;
		if (EOF == sscanf(rule_str, "%s %s %s %s %d", prot, dst_port, src_ip, action, &priority))
		{
			goto end;
		}
		ucloud_unet_security_rule_t *rule;
		rule = ucloud_unet_security_rule_init();
		rule->prot_type = ucloud_to_prot_type(prot);
		rule->action = ucloud_to_rule_action(action);
		if (rule->action == -1 || rule->prot_type == -1)
		{
			fprintf(stderr, "Invalid rule action '%s' or protocol type '%s'\n", action, prot);
			goto end;
		}
		rule->src_ip = strdup(src_ip);
		rule->dst_port = strdup(dst_port);
		rule->priority = priority;
		rule->next = sg->rule;
		sg->rule = rule;
	}while (true);

	if (sg->rule == NULL)
	{
		fprintf(stderr, "No rules\n");
		goto end;
	}
	ret = ucloud_unet_create_sg(resource->region, sg);
	if (ret == UCLOUDE_OK)
	{
		fprintf(stderr, "Create security group %s success, You can use 'dsg' command to describe security group\n", sg->name);
	}
	else
	{
		fprintf(stderr, "Create security group %s failure:%s\n", sg->name, ucloud_get_last_error());
	}
end:
	ucloud_unet_sg_deinit(sg);
}

static void update_sg()
{
	int ret;
	int group_id;
	if (-1 == get_int_val("Input security group id:", 0, &group_id))
	{
		fprintf(stderr, "Invalid security group id\n");
		return;
	}
	ucloud_unet_sg_t *sg;
	sg = ucloud_unet_sg_init();
	sg->id = group_id;

	fprintf(stderr, "Rule format: Prot Dst_port Src_ip Action Priority\nExample: TCP 3306 0.0.0.0/0 DROP 50\n");
	do
	{
		char rule_str[MAX_LINE_LEN] = {0};
		if (-1 == get_str_val("Input Rule[Press 'Ctrl+D' to finish input]:", NULL, rule_str))
		{
			break;
		}
		char prot[8] = {0};
		char dst_port[MAX_LINE_LEN] = {0};
		char src_ip[MAX_LINE_LEN] = {0};
		char action[8] = {0};
		int priority;
		if (EOF == sscanf(rule_str, "%s %s %s %s %d", prot, dst_port, src_ip, action, &priority))
		{
			goto end;
		}
		ucloud_unet_security_rule_t *rule;
		rule = ucloud_unet_security_rule_init();
		rule->prot_type = ucloud_to_prot_type(prot);
		rule->action = ucloud_to_rule_action(action);
		if (rule->action == -1 || rule->prot_type == -1)
		{
			fprintf(stderr, "Invalid rule action '%s' or protocol type '%s'\n", action, prot);
			goto end;
		}
		rule->src_ip = strdup(src_ip);
		rule->dst_port = strdup(dst_port);
		rule->priority = priority;
		rule->next = sg->rule;
		sg->rule = rule;
	}while (true);

	if (sg->rule == NULL)
	{
		fprintf(stderr, "No rules\n");
		goto end;
	}
	ret = ucloud_unet_update_sg(resource->region, sg);
	if (ret == UCLOUDE_OK)
	{
		fprintf(stderr, "Update security group %d success, You can use 'dsg' command to describe security group\n", sg->id);
	}
	else
	{
		fprintf(stderr, "Update security group %d failure:%s\n", sg->id, ucloud_get_last_error());
	}
end:
	ucloud_unet_sg_deinit(sg);
}

static void delete_sg()
{
	int ret;
	int group_id;
	if (-1 == get_int_val("Input security group id:", 0, &group_id))
	{
		fprintf(stderr, "Invalid security group id\n");
		return;
	}
	ret = ucloud_unet_delete_sg(resource->region, group_id);
	if (ret == UCLOUDE_OK)
	{
		fprintf(stderr, "Delete security group %d success\n", group_id);
	}
	else
	{
		fprintf(stderr, "Delete security group %d failure:%s\n", group_id, ucloud_get_last_error());
	}
}

static void describe_sg()
{
	ucloud_unet_sg_t *sg;
	sg = ucloud_unet_describe_sg(resource->region, 0, 0, 0);
	if (sg)
	{
		ucloud_unet_sg_t *head;
		head = sg;
		while (head)
		{
			fprintf(stderr, "Group %s information:\n", head->name);
			fprintf(stderr, " Id: %d\n", head->id);
			fprintf(stderr, " Name: %s\n", head->name);
			fprintf(stderr, " Description: %s\n", head->desc);
			fprintf(stderr, " Type: %s\n", ucloud_from_sg_type(head->type));
			fprintf(stderr, " Create Time: %d\n", head->ctime);
			fprintf(stderr, " Rules:\n");
			ucloud_unet_security_rule_t *head_rule;
			head_rule = sg->rule;
			while (head_rule)
			{
				fprintf(stderr, "  %s|%s|%s|%s|%d", ucloud_from_prot_type(head_rule->prot_type),
						head_rule->dst_port,
						head_rule->src_ip,
						ucloud_from_rule_action(head_rule->action),
						head_rule->priority);
				head_rule = head_rule->next;
			}
			head = head->next;
		}
		ucloud_unet_sg_deinit(sg);	
	}
	else
	{
		fprintf(stderr, "Describe security group failure:%s\n", ucloud_get_last_error());
	}
}

static void describe_sg_res()
{
	int group_id = 0;
	if (-1 == get_int_val("Input security group id:", 0, &group_id))
	{
		fprintf(stderr, "Invalid security group id\n");
		return;
	}
	ucloud_unet_sg_resource_t *sgr;
	sgr = ucloud_unet_describe_sg_resource(resource->region, group_id);
	if (sgr)
	{
		ucloud_unet_sg_resource_t *head;
		head = sgr;
		while (head)
		{
			fprintf(stderr, " Ip: %s", head->ip);
			head = head->next;
		}
		ucloud_unet_sg_resource_deinit(sgr);	
	}
	else
	{
		fprintf(stderr, "Describe security resource failure %s\n", ucloud_get_last_error());
	}
}

static void grant_sg()
{
	int ret;
	int group_id;
	if (-1 == get_int_val("Input security group id:", 0, &group_id))
	{
		fprintf(stderr, "Invalid security group id\n");
		return;
	}
	int res_type = urt_uhost;
	if (-1 == get_int_val("Choice resource[1:uhost 2:udb 3:ulb 4:umem]:", res_type, &res_type))
	{
		fprintf(stderr, "Invalid resource type\n");
		return;
	}
	char res_id[MAX_LINE_LEN] = {0};
	if (res_type == urt_uhost)
	{
		if (-1 == get_host_id(res_id))
		{
			fprintf(stderr, "Invalid resource id\n");
			return;
		}
		ret = ucloud_unet_grant_sg(resource->region, group_id, res_type, res_id);
		if (ret != UCLOUDE_OK)
		{
			fprintf(stderr, "Grant group %d to %s %s failure: %s\n", group_id, ucloud_from_resource_type(res_type), res_id, ucloud_get_last_error());
		}
		else
		{
			fprintf(stderr, "Grant group %d to %s %s success\n", group_id, ucloud_from_resource_type(res_type), res_id); 
		}
	}
	else{
		fprintf(stderr, "Not support resource type\n");
		return;
	}
}

