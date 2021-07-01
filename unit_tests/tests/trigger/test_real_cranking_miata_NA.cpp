/*
 * @file test_real_cranking_miata_NA.cpp
 *
 * @date May 22, 2021
 * @author Andrey Belomutskiy, (c) 2012-2021
 */

#include "engine_test_helper.h"
#include "logicdata_csv_reader.h"
#include <string>

TEST(cranking, realCrankingFromFile) {
	CsvReader reader;
	int indeces[2] = {1, 0}; // this logic data file has first trigger channel in second column and second trigger channel in first column
	reader.open("tests/trigger/recourses/cranking_na_3.csv", indeces);

	WITH_ENGINE_TEST_HELPER (MIATA_NA6_MAP);

	ssize_t read;

	for (int i = 0; i < 18; i++) {
		reader.readLine(&eth);
	}

	EXPECT_EQ( 109, GET_RPM())<< reader.lineIndex << " @ 0";
	EXPECT_EQ( 0, eth.recentWarnings()->getCount())<< "warningCounter#got synch";

	reader.readLine(&eth);

	EXPECT_EQ( 57, GET_RPM())<< reader.lineIndex << " @ 1";

	for (int i = 0; i < 30; i++) {
		reader.readLine(&eth);
	}
	EXPECT_EQ( 224, GET_RPM())<< reader.lineIndex;


	for (int i = 0; i < 30; i++) {
		reader.readLine(&eth);
	}
	EXPECT_EQ( 461, GET_RPM())<< reader.lineIndex << " @ 2";

	while (reader.haveMore()) {
		reader.processLine(&eth);
	}
	EXPECT_EQ( 0, eth.recentWarnings()->getCount())<< "warningCounter#realCranking";
	EXPECT_EQ( 400, GET_RPM())<< reader.lineIndex;
}
