/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>

/* Includes ------------------------------------------------------------------*/
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include <setjmp.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"

#include "xt_bsp_rtu09c.h"

xt_s32 add_func(xt_s32 a, xt_s32 b)
{
	return a+b;
}

//Unity
xt_void setUp(void)
{
	PrintfXTOS("in setUp ... \n");
}

xt_void tearDown(void)
{
	PrintfXTOS("in tearDown ... \n");
}

xt_void test_add_new(void) {
	PrintfXTOS("in test_add_new #1 \n");
	TEST_ASSERT_EQUAL(6, add_func(3, 3));
	PrintfXTOS("in test_add_new #2 \n");
	TEST_ASSERT_EQUAL(0, add_func(3, -3));
}

#define RUN_TEST_NO_ARGS
#define RUN_TEST(TestFunc, TestLineNum, ...) \
{ \
  Unity.CurrentTestName = #TestFunc "(" #__VA_ARGS__ ")"; \
  Unity.CurrentTestLineNumber = TestLineNum; \
  Unity.NumberOfTests++; \
  if (TEST_PROTECT()) \
  { \
      setUp(); \
      TestFunc(__VA_ARGS__); \
  } \
  if (TEST_PROTECT() && !TEST_IS_IGNORED) \
  { \
    tearDown(); \
  } \
  UnityConcludeTest(); \
}

xt_s32 TestMain(xt_s32 argc, xt_s8* argv[])
{	
	Unity.TestFile = "test_main.c";
	UnityBegin();

	RUN_TEST(test_add_new,26,RUN_TEST_NO_ARGS);

	UnityEnd();

	return 0;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
