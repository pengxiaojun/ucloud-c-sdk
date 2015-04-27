#include <ucommon.h>
#include <uinclude.h>

static const char *host_type[] = {"Normal", "SSD", "BD"};
static const char *host_status[] = {"Initializing", "Starting", "Running", "Stopping", "Stopped", "Install Fail", "Rebooting"};
static const char *ip_type[] = {"China-telecom", "China-unicom", "Internation", "Bgp", "Private", "Duplet"};
static const char *disk_type[] = {"Boot", "Data", "Udisk"};
static const char *login_mode[] = {NULL, "Password", "KeyPair"};
static const char *charge_type[] = {NULL, "Year", "Month", "Dynamic", "Trial"};
static const char *ip_status[] = {"used", "free", "freeze"};
static const char *bandwidth_type[] = {"Non-Shared", "Shared"};
static const char *operator_name[] = {"Telecom", "Unicom", "International", "Bgp", "Duplet"};
static const char *resource_type[] = {"", "uhost", "udb", "ulb", "umem", "vrouter"};
static const char *prot_type[] = {"TCP", "UDP", "ICMP", "GRE"};
static const char *rule_action[] = {"ACCEPT", "DROP"};
static const char *sg_type[] = {"Custom", "Web", "Non-Web"};

/* charge type */
int ucloud_to_charge_type(const char *type)
{
	int i;
	for (i = 1; i<sizeof(charge_type)/sizeof(charge_type[1]); ++i)
	{
		if (strcmp(charge_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_charge_type(int type)
{
	static char *charge_type[] = {NULL, "Year", "Month", "Dynamic", "Trial"};
	if (type >= sizeof(charge_type)/sizeof(charge_type[0]))
	{
		return NULL;
	}
	return charge_type[type];
}

/* login mode */
int ucloud_to_uhost_login_mode(const char *mode)
{
	int i;
	for (i = 1; i<sizeof(login_mode)/sizeof(login_mode[1]); ++i)
	{
		if (strcmp(login_mode[i], mode) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_uhost_login_mode(int mode)
{
	if (mode >= sizeof(login_mode)/sizeof(login_mode[0]))
	{
		return NULL;
	}
	return login_mode[mode];
}

/* host type */
int ucloud_to_uhost_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(host_type)/sizeof(host_type[0]); ++i)
	{
		if (strcmp(host_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_uhost_type(int type)
{
	if (type >= sizeof(host_type)/sizeof(host_type[0]))
	{
		return NULL;
	}
	return host_type[type];
}

/* host status */
int ucloud_to_uhost_status(const char *status)
{
	int i;
	for (i = 0; i<sizeof(host_status)/sizeof(host_status[0]); ++i)
	{
		if (strcmp(host_status[i], status) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_uhost_status(int status)
{
	if (status >= sizeof(host_status)/sizeof(host_status[0]))
	{
		return NULL;
	}
	return host_status[status];
}

/*  ip type */
int ucloud_to_ip_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(ip_type)/sizeof(ip_type[0]); ++i)
	{
		if (strcmp(ip_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_ip_type(int type)
{
	if (type >= sizeof(ip_type)/sizeof(ip_type[0]))
	{
		return NULL;
	}
	return ip_type[type];
}

/* ip status */
int ucloud_to_ip_status(const char *type)
{
	int i;
	for (i = 0; i<sizeof(ip_status)/sizeof(ip_status[0]); ++i)
	{
		if (strcmp(ip_status[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_ip_status(int type)
{
	if (type >= sizeof(ip_status)/sizeof(ip_status[0]))
	{
		return NULL;
	}
	return ip_status[type];
}

/* bandwith type */
int ucloud_to_bandwidth_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(bandwidth_type)/sizeof(bandwidth_type[0]); ++i)
	{
		if (strcmp(bandwidth_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_bandwidth_type(int type)
{
	if (type >= sizeof(bandwidth_type)/sizeof(bandwidth_type[0]))
	{
		return NULL;
	}
	return bandwidth_type[type];
}

/* disk type */
int ucloud_to_disk_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(disk_type)/sizeof(disk_type[0]); ++i)
	{
		if (strcmp(disk_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_disk_type(int type)
{
	if (type >= sizeof(disk_type)/sizeof(disk_type[0]))
	{
		return NULL;
	}
	return disk_type[type];
}

/* operator name */
int ucloud_to_operator_name(const char *type)
{
	int i;
	for (i = 0; i<sizeof(operator_name)/sizeof(operator_name[0]); ++i)
	{
		if (strcmp(operator_name[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_operator_name(int type)
{
	if (type >= sizeof(operator_name)/sizeof(operator_name[0]))
	{
		return NULL;
	}
	return operator_name[type];

}

int ucloud_to_resource_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(resource_type)/sizeof(resource_type[0]); ++i)
	{
		if (strcmp(resource_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_resource_type(int type)
{
	if (type >= sizeof(resource_type)/sizeof(resource_type[0]))
	{
		return NULL;
	}
	return resource_type[type];
}

int ucloud_to_prot_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(prot_type)/sizeof(prot_type[0]); ++i)
	{
		if (strcmp(prot_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_prot_type(int type)
{
	if (type >= sizeof(prot_type)/sizeof(prot_type[0]))
	{
		return NULL;
	}
	return prot_type[type];
}

int ucloud_to_rule_action(const char *action)
{
	int i;
	for (i = 0; i<sizeof(rule_action)/sizeof(rule_action[0]); ++i)
	{
		if (strcmp(rule_action[i], action) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_rule_action(int action)
{
	if (action >= sizeof(rule_action)/sizeof(rule_action[0]))
	{
		return NULL;
	}
	return rule_action[action];
}

int ucloud_to_sg_type(const char *type)
{
	int i;
	for (i = 0; i<sizeof(sg_type)/sizeof(sg_type[0]); ++i)
	{
		if (strcmp(sg_type[i], type) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char *ucloud_from_sg_type(int type)
{
	if (type >= sizeof(sg_type)/sizeof(sg_type[0]))
	{
		return NULL;
	}
	return sg_type[type];
}
