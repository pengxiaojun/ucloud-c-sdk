#include "test_ucloud.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	fprintf(stderr, "Start to test...\n");
#ifdef __TEST_PARAM__
	test_param();
#endif

#ifdef __TEST_UHOST__
	test_host();
#endif

#ifdef __TEST_SIGNATURE__
	test_signature();
#endif

#ifdef __TEST_UNET__
	test_unet();
#endif

#ifdef __TEST_UMON__
	test_umon();
#endif
	fprintf(stderr, "Test completed!!!\n");
	return 0;
}
