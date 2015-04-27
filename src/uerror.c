#include <uerror.h>
#include <uinclude.h>

#define MAX_ERR_LEN  1024
/* store last error message */
static char last_error[MAX_ERR_LEN] = {0};

static char *ucloud_str_error[] = 
{
	"Success",
	"Error count of signature parameter",
	"Alloc memory error",
	"Invalid parameter",
	"Unknow error",
	"Error",
	"Http request error",
	"Insufficient length",
	"Not exist public key",
	"Not exist private key"
};

const char *ucloud_strerror(int code)
{
	const int error_base = 100;
	if (code < UCLOUDE_OK)
	{
		return NULL;
	}
	if (code == UCLOUDE_OK)
	{
		return ucloud_str_error[code];
	}
	int index = code - error_base;
	if (index >= (sizeof(ucloud_str_error)/sizeof(ucloud_str_error[0])))
	{
		return NULL;
	}
	return ucloud_str_error[index];
}

const char *ucloud_get_last_error()
{
	return last_error;
}

void ucloud_set_last_error(const char *err)
{
	memset(last_error, 0, sizeof(last_error));
	strncpy(last_error, err, MAX_ERR_LEN);
}
