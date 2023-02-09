#include <stdio.h>
#include <base64.h>
#include <unity.h>
#include <string.h>

#define STRING_MAX_SIZE 2048

#define TEST_ASSERT_BASE64_ENCODE_STRING(input, expected) do { \
		actual = base64_encode(input, strlen(input)); \
		TEST_ASSERT_EQUAL_STRING(expected, actual); \
		free(actual); \
	} while (0)

#define TEST_ASSERT_BASE64_DECODE_STRING(input, expected) do { \
		actual = base64_decode(input, &actual_size); \
		TEST_ASSERT_EQUAL_UINT64(strlen(expected), actual_size); \
		TEST_ASSERT_EQUAL_MEMORY(expected, actual, actual_size); \
		free(actual); \
	} while (0)

#define TEST_ASSERT_BASE64_ENCODE_STANDARD(input_filename, expected_filename, standard) do { \
		input_file = fopen(input_filename, "r"); \
		if (input_file == NULL) { \
			fprintf(stderr, "Error opening %s\n", input_filename); \
			perror(__FILE__); \
			exit(1); \
		} \
		expected_file = fopen(expected_filename, "r"); \
		if (expected_file == NULL) { \
			fprintf(stderr, "Error opening %s\n", expected_filename); \
			perror(__FILE__); \
			exit(1); \
		} \
		memset(input, 0x0, STRING_MAX_SIZE); \
		memset(expected, 0x0, STRING_MAX_SIZE); \
		input_size = fread(input, 1, STRING_MAX_SIZE, input_file); \
		fread(expected, 1, STRING_MAX_SIZE, expected_file); \
		fclose(input_file); \
		fclose(expected_file); \
		actual = base64_encode_standard(input, input_size, standard); \
		TEST_ASSERT_EQUAL_STRING(expected, actual); \
		free(actual); \
	} while (0)

#define TEST_ASSERT_BASE64_DECODE_STANDARD(input_filename, expected_filename, standard) do {\
		input_file = fopen(input_filename, "r"); \
		if (input_file == NULL) { \
			fprintf(stderr, "Error opening %s\n", input_filename); \
			perror(__FILE__); \
			exit(1); \
		} \
		expected_file = fopen(expected_filename, "r"); \
		if (expected_file == NULL) { \
			fprintf(stderr, "Error opening %s\n", expected_filename); \
			perror(__FILE__); \
			exit(1); \
		} \
		memset(input, 0x0, STRING_MAX_SIZE); \
		memset(expected, 0x0, STRING_MAX_SIZE); \
		fread(input, 1, STRING_MAX_SIZE, input_file); \
		expected_size = fread(expected, 1, STRING_MAX_SIZE, expected_file); \
		fclose(input_file); \
		fclose(expected_file); \
		actual = base64_decode_standard(input, &actual_size, standard); \
		TEST_ASSERT_EQUAL_UINT64(expected_size, actual_size); \
		TEST_ASSERT_EQUAL_MEMORY(expected, actual, actual_size); \
		free(actual); \
	} while (0)


void setUp() {}
void tearDown() {}

void test_base64_encode_string()
{
	char *actual;
	TEST_ASSERT_BASE64_ENCODE_STRING("Man", "TWFu");
	TEST_ASSERT_BASE64_ENCODE_STRING("Ma", "TWE=");
	TEST_ASSERT_BASE64_ENCODE_STRING("M", "TQ==");
	TEST_ASSERT_BASE64_ENCODE_STRING("light work.", "bGlnaHQgd29yay4=");
	TEST_ASSERT_BASE64_ENCODE_STRING("light work", "bGlnaHQgd29yaw==");
	TEST_ASSERT_BASE64_ENCODE_STRING("light wor", "bGlnaHQgd29y");
	TEST_ASSERT_BASE64_ENCODE_STRING("light wo", "bGlnaHQgd28=");
	TEST_ASSERT_BASE64_ENCODE_STRING("light w", "bGlnaHQgdw==");
}

void test_base64_decode_string()
{
	void *actual;
	size_t actual_size;
	TEST_ASSERT_BASE64_DECODE_STRING("TWFu", "Man");
	TEST_ASSERT_BASE64_DECODE_STRING("TWE=", "Ma");
	TEST_ASSERT_BASE64_DECODE_STRING("TQ==", "M");
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29yay4=", "light work.");
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29yaw==", "light work");
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd29y", "light wor");
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgd28=", "light wo");
	TEST_ASSERT_BASE64_DECODE_STRING("bGlnaHQgdw==", "light w");
}

void test_base64_encode_standard()
{
	FILE *input_file;
	FILE *expected_file;

	unsigned char input[STRING_MAX_SIZE];
	unsigned char expected[STRING_MAX_SIZE];

	size_t input_size;
	char *actual;

	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-16", "./test/src/urandom-16-rfc4648-4.txt", RFC_4648_4);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-16", "./test/src/urandom-16-rfc4648-5.txt", RFC_4648_5);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-16", "./test/src/urandom-16-rfc3501.txt", RFC_3501);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-16", "./test/src/urandom-16-rfc2152.txt", RFC_2152);

	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-30", "./test/src/urandom-30-rfc4648-4.txt", RFC_4648_4);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-30", "./test/src/urandom-30-rfc4648-5.txt", RFC_4648_5);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-30", "./test/src/urandom-30-rfc3501.txt", RFC_3501);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-30", "./test/src/urandom-30-rfc2152.txt", RFC_2152);

	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-32", "./test/src/urandom-32-rfc4648-4.txt", RFC_4648_4);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-32", "./test/src/urandom-32-rfc4648-5.txt", RFC_4648_5);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-32", "./test/src/urandom-32-rfc3501.txt", RFC_3501);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-32", "./test/src/urandom-32-rfc2152.txt", RFC_2152);

	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-512", "./test/src/urandom-512-rfc4648-4.txt", RFC_4648_4);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-512", "./test/src/urandom-512-rfc4648-5.txt", RFC_4648_5);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-512", "./test/src/urandom-512-rfc3501.txt", RFC_3501);
	TEST_ASSERT_BASE64_ENCODE_STANDARD("./test/src/urandom-512", "./test/src/urandom-512-rfc2152.txt", RFC_2152);
}

void test_base64_decode_standard()
{
	FILE *input_file;
	FILE *expected_file;

	char input[STRING_MAX_SIZE];
	unsigned char expected[STRING_MAX_SIZE];

	size_t expected_size;
	size_t actual_size;
	void *actual;

	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-16-rfc4648-4.txt", "./test/src/urandom-16", RFC_4648_4);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-16-rfc4648-5.txt", "./test/src/urandom-16", RFC_4648_5);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-16-rfc3501.txt", "./test/src/urandom-16", RFC_3501);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-16-rfc2152.txt", "./test/src/urandom-16", RFC_2152);

	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-30-rfc4648-4.txt", "./test/src/urandom-30", RFC_4648_4);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-30-rfc4648-5.txt", "./test/src/urandom-30", RFC_4648_5);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-30-rfc3501.txt", "./test/src/urandom-30", RFC_3501);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-30-rfc2152.txt", "./test/src/urandom-30", RFC_2152);

	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-32-rfc4648-4.txt", "./test/src/urandom-32", RFC_4648_4);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-32-rfc4648-5.txt", "./test/src/urandom-32", RFC_4648_5);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-32-rfc3501.txt", "./test/src/urandom-32", RFC_3501);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-32-rfc2152.txt", "./test/src/urandom-32", RFC_2152);

	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-512-rfc4648-4.txt", "./test/src/urandom-512", RFC_4648_4);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-512-rfc4648-5.txt", "./test/src/urandom-512", RFC_4648_5);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-512-rfc3501.txt", "./test/src/urandom-512", RFC_3501);
	TEST_ASSERT_BASE64_DECODE_STANDARD("./test/src/urandom-512-rfc2152.txt", "./test/src/urandom-512", RFC_2152);
}

int main()
{
	UNITY_BEGIN();
	RUN_TEST(test_base64_encode_string);
	RUN_TEST(test_base64_decode_string);
	RUN_TEST(test_base64_encode_standard);
	RUN_TEST(test_base64_decode_standard);
	return UNITY_END();
}
