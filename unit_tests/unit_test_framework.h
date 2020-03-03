/*
 * @file unit_test_framework.h
 *
 * @date Mar 4, 2018
 * @author Andrey Belomutskiy, (c) 2012-2020
 */

#pragma once

#include "engine.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
using ::testing::Return;

// This lets us inspect private state from unit tests
#define private public

/**
 * These are usually used with ASSETR_NEAR(val1, val2, abs_error)
 */
#define EPS0D 1
#define EPS1D 0.1
#define EPS2D 0.01
#define EPS3D 0.001
#define EPS4D 0.0001
#define EPS5D 0.00001

// todo: migrate to googletest, use EXPECT_* and ASSERT_*
void assertEqualsM2(const char *msg, float expected, float actual, float EPS);
void assertEqualsM(const char *msg, float expected, float actual);
void assertEqualsLM(const char *msg, long expected, long actual);
void assertEqualsM4(const char *prefix, const char *msg, float expected, float actual);

