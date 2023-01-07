#include <base64.h>
#include <unity.h>
#include <string.h>

#define TEST_ASSERT_BASE64_ENCODE_STRING(input, expected, pad) do { \
		actual = base64_encode(input, strlen(input), pad); \
		TEST_ASSERT_EQUAL_STRING(expected, actual); \
		free(actual); \
	} while(0)

#define TEST_ASSERT_BASE64_DECODE_STRING(input, expected, strict) do { \
		actual = base64_decode(input, NULL, strict); \
		TEST_ASSERT_EQUAL_STRING(expected, actual); \
		free(actual); \
	} while(0)

void setUp() {}
void tearDown() {}

void test_base64_encode_string_no_pad()
{
	char *actual;
	TEST_ASSERT_BASE64_ENCODE_STRING("Man", "TWFu", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("Ma", "TWE", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("M", "TQ", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("light work.", "bGlnaHQgd29yay4", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("light work", "bGlnaHQgd29yaw", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("light wor", "bGlnaHQgd29y", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("light wo", "bGlnaHQgd28", false);
	TEST_ASSERT_BASE64_ENCODE_STRING("light w", "bGlnaHQgdw", false);
}

void test_base64_encode_string_pad()
{
	char *actual;
	TEST_ASSERT_BASE64_ENCODE_STRING("Man", "TWFu", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("Ma", "TWE=", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("M", "TQ==", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("light work.", "bGlnaHQgd29yay4=", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("light work", "bGlnaHQgd29yaw==", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("light wor", "bGlnaHQgd29y", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("light wo", "bGlnaHQgd28=", true);
	TEST_ASSERT_BASE64_ENCODE_STRING("light w", "bGlnaHQgdw==", true);
}

void test_base64_decode_string_strict()
{
	void *actual;
	TEST_ASSERT_BASE64_DECODE_STRING("TWFu", "Man", true);
	TEST_ASSERT_BASE64_DECODE_STRING("TWE=", "Ma", true);
	TEST_ASSERT_BASE64_DECODE_STRING("TQ==", "M", true);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29yay4=", "light work.", true);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29yaw==", "light work", true);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29y", "light wor", true);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd28=", "light wo", true);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgdw==", "light w", true);
}

void test_base64_decode_string_lenient()
{
	void *actual;
	TEST_ASSERT_BASE64_DECODE_STRING("!@#$TWFu", "Man", false);
	TEST_ASSERT_BASE64_DECODE_STRING("TWE=!@#$", "Ma", false);
	TEST_ASSERT_BASE64_DECODE_STRING("TQ!@#$==", "M", false);
	TEST_ASSERT_BASE64_DECODE_STRING("!@#$bGlna!@#$HQgd29yay4=!@#$", "light work.", false);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQ!@#$gd29yaw==", "light work", false);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29y!@#$", "light wor", false);
	TEST_ASSERT_BASE64_DECODE_STRING("b!@#$GlnaHQgd28=", "light wo", false);
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQg!@#$dw==", "light w", false);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_base64_encode_string_pad);
	RUN_TEST(test_base64_encode_string_no_pad);
	RUN_TEST(test_base64_decode_string_strict);
	RUN_TEST(test_base64_decode_string_lenient);
	return UNITY_END();
}
