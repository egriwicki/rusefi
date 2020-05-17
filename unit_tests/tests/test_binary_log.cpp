#include "log_field.h"

#include <gmock/gmock.h>

using ::testing::ElementsAre;

TEST(BinaryLogField, FieldHeader) {
	scaled_channel<int8_t, 10> channel;
	LogField field(channel, "name", "units", 2);

	char buffer[56];
	memset(buffer, 0xAA, sizeof(buffer));

	// Should write 55 bytes
	EXPECT_EQ(55, field.writeHeader(buffer));

	// Expect correctly written header
	EXPECT_THAT(buffer, ElementsAre(
		1,	// type: int8_t
		// name - 34 bytes, 0 padded
		'n', 'a', 'm', 'e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		// units - 10 bytes, 0 padded
		'u', 'n', 'i', 't', 's', 0, 0, 0, 0, 0,
		// display style: float
		0,
		// Scale = 0.1 (float)
		0x3d, 0xcc, 0xcc, 0xcd,
		// Transform - we always use 0
		0, 0, 0, 0,
		// Digits - 2, as configured
		2,

		// After end should be 0xAA, not touched
		0xAA
	));
}

TEST(BinaryLogField, Value) {
	scaled_channel<uint32_t, 1> testValue = 12345678;
	LogField lf(testValue, "test", "unit", 0);

	char buffer[6];
	memset(buffer, 0xAA, sizeof(buffer));

	// Should write 4 bytes
	EXPECT_EQ(4, lf.writeData(buffer));

	// Check that big endian data was written, and bytes after weren't touched
	EXPECT_THAT(buffer, ElementsAre(0x00, 0xbc, 0x61, 0x4e, 0xAA, 0xAA));
}
