/*
* Copyright (C) 2018-2020 Nick Naumenko (https://gitlab.com/nnaumenko)
* All rights reserved.
* This software may be modified and distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/

#include "gtest/gtest.h"
#include "metaf.hpp"

///////////////////////////////////////////////////////////////////////////////
// Wind shear
///////////////////////////////////////////////////////////////////////////////

TEST(SecondaryLocationGroup, parseWs) {
	const auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->type(), metaf::SecondaryLocationGroup::Type::WIND_SHEAR_IN_LOWER_LAYERS);
	EXPECT_FALSE(slg->runway().has_value());
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());

	EXPECT_FALSE(slg->visibility().isReported());
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseWsAll) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("ALL", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);

	EXPECT_FALSE(slg->runway().has_value());
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());

	EXPECT_FALSE(slg->visibility().isReported());
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseWsAllRwy) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("ALL", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg->append("RWY", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);

	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_TRUE(slg->runway()->isAllRunways());
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());

	EXPECT_FALSE(slg->visibility().isReported());
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseWsR32) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("R32", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);

	EXPECT_EQ(slg->type(), metaf::SecondaryLocationGroup::Type::WIND_SHEAR_IN_LOWER_LAYERS);
	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 32u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, parseWsR27C) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("R27C", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);

	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 27u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::CENTER);
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, parseWsRWY32) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("RWY32", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);

	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 32u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, parseWsRWY27C) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("RWY27C", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);

	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 27u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::CENTER);
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, windShearAppendWrongGroupWs) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("X37", metaf::ReportPart::METAR), 
		metaf::AppendResult::GROUP_INVALIDATED);
	EXPECT_EQ(slg->append("TEST", metaf::ReportPart::METAR), 
		metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, windShearAppendWrongGroupWsAll) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("ALL", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg->append("R22", metaf::ReportPart::METAR), 
		metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, windShearAppendToCompleteGroups) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg1.has_value());

	EXPECT_EQ(slg1->append("ALL", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);

	EXPECT_EQ(slg1->append("R32", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("RWY32", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("ALL", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("RWY", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);

	ASSERT_TRUE(slg1->runway().has_value());
	EXPECT_TRUE(slg1->runway()->isAllRunways());
	EXPECT_FALSE(slg1->direction().has_value());

	EXPECT_FALSE(slg1->height().isReported());
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg2.has_value());

	EXPECT_EQ(slg2->append("R32", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);

	EXPECT_EQ(slg2->append("R32", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("RWY32", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("ALL", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("RWY", metaf::ReportPart::METAR), 
		metaf::AppendResult::NOT_APPENDED);

	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 32u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());

	EXPECT_FALSE(slg2->height().isReported());
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, parseWindShearWrongReportPart) {
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::UNKNOWN).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::HEADER).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::TAF).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::RMK).has_value());
}

TEST(SecondaryLocationGroup, isValidWindShearValidRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("RWY32", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
}

TEST(SecondaryLocationGroup, isValidWindShearWrongRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("RWY37", metaf::ReportPart::METAR), 
		metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidWindShearAllRunways) {
	auto slg = metaf::SecondaryLocationGroup::parse("WS", metaf::ReportPart::METAR);
	ASSERT_TRUE(slg.has_value());

	EXPECT_EQ(slg->append("ALL", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg->append("RWY", metaf::ReportPart::METAR), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());
}

///////////////////////////////////////////////////////////////////////////////
// Ceiling Height and Variable Ceiling Height
///////////////////////////////////////////////////////////////////////////////

TEST(SecondaryLocationGroup, parseCeiling) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("025", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::CEILING);
	EXPECT_FALSE(slg1->runway().has_value());
	EXPECT_FALSE(slg1->direction().has_value());
	ASSERT_TRUE(slg1->height().isInteger());
	EXPECT_EQ(slg1->height().integer().value(), 2500u);
	EXPECT_EQ(slg1->height().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg1->height().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());
	EXPECT_FALSE(slg1->visibility().isReported());
	EXPECT_FALSE(slg1->minVisibility().isReported());
	EXPECT_FALSE(slg1->maxVisibility().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::CEILING);
	EXPECT_EQ(slg2->append("025", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	ASSERT_TRUE(slg2->height().isInteger());
	EXPECT_EQ(slg2->height().integer().value(), 2500u);
	EXPECT_EQ(slg2->height().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg2->height().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
	EXPECT_FALSE(slg2->visibility().isReported());
	EXPECT_FALSE(slg2->minVisibility().isReported());
	EXPECT_FALSE(slg2->maxVisibility().isReported());

	auto slg3 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::CEILING);
	EXPECT_EQ(slg3->append("017", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg3->direction().has_value());
	ASSERT_TRUE(slg3->height().isInteger());
	EXPECT_EQ(slg3->height().integer().value(), 1700u);
	EXPECT_EQ(slg3->height().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg3->height().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg3->minHeight().isReported());
	EXPECT_FALSE(slg3->maxHeight().isReported());
	EXPECT_FALSE(slg3->visibility().isReported());
	EXPECT_FALSE(slg3->minVisibility().isReported());
	EXPECT_FALSE(slg3->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseVariableCeiling) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("008V014", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_CEILING);
	EXPECT_FALSE(slg1->runway().has_value());
	EXPECT_FALSE(slg1->direction().has_value());
	EXPECT_FALSE(slg1->height().isReported());
	ASSERT_TRUE(slg1->minHeight().isInteger());
	EXPECT_EQ(slg1->minHeight().integer().value(), 800u);
	EXPECT_EQ(slg1->minHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg1->minHeight().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg1->maxHeight().isInteger());
	EXPECT_EQ(slg1->maxHeight().integer().value(), 1400u);
	EXPECT_EQ(slg1->maxHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg1->maxHeight().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg1->visibility().isReported());
	EXPECT_FALSE(slg1->minVisibility().isReported());
	EXPECT_FALSE(slg1->maxVisibility().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("008V014", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_CEILING);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	EXPECT_FALSE(slg2->height().isReported());
	ASSERT_TRUE(slg2->minHeight().isInteger());
	EXPECT_EQ(slg2->minHeight().integer().value(), 800u);
	EXPECT_EQ(slg2->minHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg2->minHeight().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg2->maxHeight().isInteger());
	EXPECT_EQ(slg2->maxHeight().integer().value(), 1400u);
	EXPECT_EQ(slg2->maxHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg2->maxHeight().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg2->visibility().isReported());
	EXPECT_FALSE(slg2->minVisibility().isReported());
	EXPECT_FALSE(slg2->maxVisibility().isReported());

	auto slg3 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("002V006", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY24R", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_CEILING);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::RIGHT);
	EXPECT_FALSE(slg3->direction().has_value());
	EXPECT_FALSE(slg3->height().isReported());
	ASSERT_TRUE(slg3->minHeight().isInteger());
	EXPECT_EQ(slg3->minHeight().integer().value(), 200u);
	EXPECT_EQ(slg3->minHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg3->minHeight().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg3->maxHeight().isInteger());
	EXPECT_EQ(slg3->maxHeight().integer().value(), 600u);
	EXPECT_EQ(slg3->maxHeight().unit(), metaf::Distance::Unit::FEET);
	EXPECT_EQ(slg3->maxHeight().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg3->visibility().isReported());
	EXPECT_FALSE(slg3->minVisibility().isReported());
	EXPECT_FALSE(slg3->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, ceilingAppendWrongGroupCig) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("R22", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("///", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("///V///", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, ceilingAppendWrongGroupCigValue) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("002", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("004", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("002", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("X32", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("002", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("002", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
}

TEST(SecondaryLocationGroup, ceilingAppendWrongGroupCigVariable) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("004", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("X32", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
}

TEST(SecondaryLocationGroup, ceilingAppendToCompleteGroups) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("002", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY32", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY12", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("002", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY32", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY12", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("002", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("014V016", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg2->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
}

TEST(SecondaryLocationGroup, parseCeilingWrongReportPart) {
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::UNKNOWN).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::HEADER).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::TAF).has_value());
}

TEST(SecondaryLocationGroup, isValidCeilingValidRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("025", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("012V014", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg2->isValid());
}

TEST(SecondaryLocationGroup, isValidCeilingWrongRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("025", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("012V014", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg2->isValid());
}

TEST(SecondaryLocationGroup, isValidCeilingNoRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("025", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_TRUE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CIG", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("012V014", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_TRUE(slg2->isValid());
}

///////////////////////////////////////////////////////////////////////////////
// CHINO and VISNO
///////////////////////////////////////////////////////////////////////////////

TEST(SecondaryLocationGroup, parseChino) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::CHINO);
	EXPECT_FALSE(slg1->runway().has_value());
	EXPECT_FALSE(slg1->direction().has_value());
	EXPECT_FALSE(slg1->height().isReported());
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());
	EXPECT_FALSE(slg1->visibility().isReported());
	EXPECT_FALSE(slg1->minVisibility().isReported());
	EXPECT_FALSE(slg1->maxVisibility().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::CHINO);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	EXPECT_FALSE(slg2->height().isReported());
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
	EXPECT_FALSE(slg2->visibility().isReported());
	EXPECT_FALSE(slg2->minVisibility().isReported());
	EXPECT_FALSE(slg2->maxVisibility().isReported());

	auto slg3 = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::CHINO);
	EXPECT_EQ(slg3->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg3->direction().has_value());
	EXPECT_FALSE(slg3->height().isReported());
	EXPECT_FALSE(slg3->minHeight().isReported());
	EXPECT_FALSE(slg3->maxHeight().isReported());
	EXPECT_FALSE(slg3->visibility().isReported());
	EXPECT_FALSE(slg3->minVisibility().isReported());
	EXPECT_FALSE(slg3->maxVisibility().isReported());

	auto slg4 = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->type(), metaf::SecondaryLocationGroup::Type::CHINO);
	EXPECT_EQ(slg4->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg4->runway().has_value());
	ASSERT_TRUE(slg4->direction().has_value());
	EXPECT_EQ(slg4->direction()->status(), metaf::Direction::Status::VALUE_CARDINAL);
	EXPECT_EQ(slg4->direction()->cardinal(), metaf::Direction::Cardinal::SW);
	EXPECT_FALSE(slg4->height().isReported());
	EXPECT_FALSE(slg4->minHeight().isReported());
	EXPECT_FALSE(slg4->maxHeight().isReported());
	EXPECT_FALSE(slg4->visibility().isReported());
	EXPECT_FALSE(slg4->minVisibility().isReported());
	EXPECT_FALSE(slg4->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseVisno) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::VISNO);
	EXPECT_FALSE(slg1->runway().has_value());
	EXPECT_FALSE(slg1->direction().has_value());
	EXPECT_FALSE(slg1->height().isReported());
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());
	EXPECT_FALSE(slg1->visibility().isReported());
	EXPECT_FALSE(slg1->minVisibility().isReported());
	EXPECT_FALSE(slg1->maxVisibility().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::VISNO);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	EXPECT_FALSE(slg2->height().isReported());
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
	EXPECT_FALSE(slg2->visibility().isReported());
	EXPECT_FALSE(slg2->minVisibility().isReported());
	EXPECT_FALSE(slg2->maxVisibility().isReported());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::VISNO);
	EXPECT_EQ(slg3->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg3->direction().has_value());
	EXPECT_FALSE(slg3->height().isReported());
	EXPECT_FALSE(slg3->minHeight().isReported());
	EXPECT_FALSE(slg3->maxHeight().isReported());
	EXPECT_FALSE(slg3->visibility().isReported());
	EXPECT_FALSE(slg3->minVisibility().isReported());
	EXPECT_FALSE(slg3->maxVisibility().isReported());

	auto slg4 = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->type(), metaf::SecondaryLocationGroup::Type::VISNO);
	EXPECT_EQ(slg4->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg4->runway().has_value());
	ASSERT_TRUE(slg4->direction().has_value());
	EXPECT_EQ(slg4->direction()->status(), metaf::Direction::Status::VALUE_CARDINAL);
	EXPECT_EQ(slg4->direction()->cardinal(), metaf::Direction::Cardinal::SW);
	EXPECT_FALSE(slg4->height().isReported());
	EXPECT_FALSE(slg4->minHeight().isReported());
	EXPECT_FALSE(slg4->maxHeight().isReported());
	EXPECT_FALSE(slg4->visibility().isReported());
	EXPECT_FALSE(slg4->minVisibility().isReported());
	EXPECT_FALSE(slg4->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, isValidChinoValidRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidChinoWrongRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidChinoNoRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_TRUE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidChinoDirection) {
	auto slg = metaf::SecondaryLocationGroup::parse("CHINO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("NW", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidVisnoValidRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidVisnoWrongRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidVisnoNoRunway) {
	auto slg = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_TRUE(slg->isValid());
}

TEST(SecondaryLocationGroup, isValidVisnoDirection) {
	auto slg = metaf::SecondaryLocationGroup::parse("VISNO", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("NW", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg->isValid());
}

///////////////////////////////////////////////////////////////////////////////
// Runway visibility
///////////////////////////////////////////////////////////////////////////////

TEST(SecondaryLocationGroup, parseRunwayVariableVisibility) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("1V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_VISIBILITY);
	ASSERT_TRUE(slg1->runway().has_value());
	EXPECT_EQ(slg1->runway()->number(), 24u);
	EXPECT_EQ(slg1->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg1->direction().has_value());
	EXPECT_FALSE(slg1->height().isReported());
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());
	EXPECT_FALSE(slg1->visibility().isReported());
	ASSERT_TRUE(slg1->minVisibility().isInteger());
	EXPECT_EQ(slg1->minVisibility().integer().value(), 1u);
	EXPECT_EQ(slg1->minVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg1->minVisibility().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg1->maxVisibility().isInteger());
	EXPECT_EQ(slg1->maxVisibility().integer().value(), 3u);
	EXPECT_EQ(slg1->maxVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg1->maxVisibility().modifier(), metaf::Distance::Modifier::NONE);

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1/4V2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_VISIBILITY);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	EXPECT_FALSE(slg2->height().isReported());
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
	EXPECT_FALSE(slg2->visibility().isReported());
	ASSERT_TRUE(slg2->minVisibility().isFraction());
	EXPECT_EQ(slg2->minVisibility().numerator().value(), 1u);
	EXPECT_EQ(slg2->minVisibility().denominator().value(), 4u);
	EXPECT_EQ(slg2->minVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg2->minVisibility().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg2->maxVisibility().isInteger());
	EXPECT_EQ(slg2->maxVisibility().integer().value(), 2u);
	EXPECT_EQ(slg2->maxVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg2->maxVisibility().modifier(), metaf::Distance::Modifier::NONE);

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1/8V1/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY24R", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_VISIBILITY);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::RIGHT);
	EXPECT_FALSE(slg3->direction().has_value());
	EXPECT_FALSE(slg3->height().isReported());
	EXPECT_FALSE(slg3->minHeight().isReported());
	EXPECT_FALSE(slg3->maxHeight().isReported());
	EXPECT_FALSE(slg3->visibility().isReported());
	ASSERT_TRUE(slg3->minVisibility().isFraction());
	EXPECT_EQ(slg3->minVisibility().numerator().value(), 1u);
	EXPECT_EQ(slg3->minVisibility().denominator().value(), 8u);
	EXPECT_EQ(slg3->minVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg3->minVisibility().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg3->maxVisibility().isFraction());
	EXPECT_EQ(slg3->maxVisibility().numerator().value(), 1u);
	EXPECT_EQ(slg3->maxVisibility().denominator().value(), 4u);
	EXPECT_EQ(slg3->maxVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg3->minVisibility().modifier(), metaf::Distance::Modifier::NONE);

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("3/4V2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY24C", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->type(), metaf::SecondaryLocationGroup::Type::VARIABLE_VISIBILITY);
	ASSERT_TRUE(slg4->runway().has_value());
	EXPECT_EQ(slg4->runway()->number(), 24u);
	EXPECT_EQ(slg4->runway()->designator(), metaf::Runway::Designator::CENTER);
	EXPECT_FALSE(slg4->direction().has_value());
	EXPECT_FALSE(slg4->height().isReported());
	EXPECT_FALSE(slg4->minHeight().isReported());
	EXPECT_FALSE(slg4->maxHeight().isReported());
	EXPECT_FALSE(slg4->visibility().isReported());
	ASSERT_TRUE(slg4->minVisibility().hasInteger());
	ASSERT_TRUE(slg4->minVisibility().hasFraction());
	EXPECT_EQ(slg4->minVisibility().integer().value(), 1u);
	EXPECT_EQ(slg4->minVisibility().numerator().value(), 3u);
	EXPECT_EQ(slg4->minVisibility().denominator().value(), 4u);
	EXPECT_EQ(slg4->minVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg4->minVisibility().modifier(), metaf::Distance::Modifier::NONE);
	ASSERT_TRUE(slg4->maxVisibility().isInteger());
	EXPECT_EQ(slg4->maxVisibility().integer().value(), 2u);
	EXPECT_EQ(slg4->maxVisibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg4->maxVisibility().modifier(), metaf::Distance::Modifier::NONE);
}

TEST(SecondaryLocationGroup, parseRunwayNonVariableVisibility) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg1->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg1->runway().has_value());
	EXPECT_EQ(slg1->runway()->number(), 24u);
	EXPECT_EQ(slg1->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg1->direction().has_value());
	EXPECT_FALSE(slg1->height().isReported());
	EXPECT_FALSE(slg1->minHeight().isReported());
	EXPECT_FALSE(slg1->maxHeight().isReported());
	ASSERT_TRUE(slg1->visibility().isInteger());
	EXPECT_EQ(slg1->visibility().integer().value(), 2u);
	EXPECT_EQ(slg1->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg1->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg1->minVisibility().isReported());
	EXPECT_FALSE(slg1->maxVisibility().isReported());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("R18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg2->runway().has_value());
	EXPECT_EQ(slg2->runway()->number(), 18u);
	EXPECT_EQ(slg2->runway()->designator(), metaf::Runway::Designator::NONE);
	EXPECT_FALSE(slg2->direction().has_value());
	EXPECT_FALSE(slg2->height().isReported());
	EXPECT_FALSE(slg2->minHeight().isReported());
	EXPECT_FALSE(slg2->maxHeight().isReported());
	ASSERT_TRUE(slg2->visibility().isFraction());
	EXPECT_EQ(slg2->visibility().numerator().value(), 3u);
	EXPECT_EQ(slg2->visibility().denominator().value(), 4u);
	EXPECT_EQ(slg2->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg2->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg2->minVisibility().isReported());
	EXPECT_FALSE(slg2->maxVisibility().isReported());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("15", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY24R", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg3->runway().has_value());
	EXPECT_EQ(slg3->runway()->number(), 24u);
	EXPECT_EQ(slg3->runway()->designator(), metaf::Runway::Designator::RIGHT);
	EXPECT_FALSE(slg3->direction().has_value());
	EXPECT_FALSE(slg3->height().isReported());
	EXPECT_FALSE(slg3->minHeight().isReported());
	EXPECT_FALSE(slg3->maxHeight().isReported());
	ASSERT_TRUE(slg3->visibility().isInteger());
	EXPECT_EQ(slg3->visibility().integer().value(), 15u);
	EXPECT_EQ(slg3->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg3->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg3->minVisibility().isReported());
	EXPECT_FALSE(slg3->maxVisibility().isReported());

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY24C", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg4->runway().has_value());
	EXPECT_EQ(slg4->runway()->number(), 24u);
	EXPECT_EQ(slg4->runway()->designator(), metaf::Runway::Designator::CENTER);
	EXPECT_FALSE(slg4->direction().has_value());
	EXPECT_FALSE(slg4->height().isReported());
	EXPECT_FALSE(slg4->minHeight().isReported());
	EXPECT_FALSE(slg4->maxHeight().isReported());
	ASSERT_TRUE(slg4->visibility().hasInteger());
	ASSERT_TRUE(slg4->visibility().hasFraction());
	EXPECT_EQ(slg4->visibility().integer().value(), 1u);
	EXPECT_EQ(slg4->visibility().numerator().value(), 3u);
	EXPECT_EQ(slg4->visibility().denominator().value(), 4u);
	EXPECT_EQ(slg4->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg4->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg4->minVisibility().isReported());
	EXPECT_FALSE(slg4->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, parseVis) {
	auto slg = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());

	EXPECT_FALSE(slg->runway().has_value());
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());

	EXPECT_FALSE(slg->visibility().isReported());
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, appendOtherToVis) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("RWY32", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("CHINO", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("VISNO", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("CIG", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg5 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg5.has_value());
	EXPECT_EQ(slg5->append("WS", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
	
	auto slg6 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg6.has_value());
	EXPECT_EQ(slg6->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg7 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg7.has_value());
	EXPECT_EQ(slg7->append("100", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg8 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg8.has_value());
	EXPECT_EQ(slg8->append("5SM", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg9 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg9.has_value());
	EXPECT_EQ(slg9->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg10 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg10.has_value());
	EXPECT_EQ(slg10->append("M1/4", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg11 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg11.has_value());
	EXPECT_EQ(slg11->append("P6", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, parseVisInteger) {
	auto slg = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);

	EXPECT_FALSE(slg->runway().has_value());
	EXPECT_FALSE(slg->direction().has_value());

	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());

	EXPECT_TRUE(slg->visibility().isReported());
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, appendOtherToVisInteger) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("1/2SM", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("CHINO", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("VISNO", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("CIG", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg5 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg5.has_value());
	EXPECT_EQ(slg5->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg5->append("WS", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
	
	auto slg6 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg6.has_value());
	EXPECT_EQ(slg6->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg6->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg7 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg7.has_value());
	EXPECT_EQ(slg7->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg7->append("100", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg8 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg8.has_value());
	EXPECT_EQ(slg8->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg8->append("5SM", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg9 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg9.has_value());
	EXPECT_EQ(slg9->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg9->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg10 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg10.has_value());
	EXPECT_EQ(slg10->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg10->append("2", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg11 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg11.has_value());
	EXPECT_EQ(slg11->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg11->append("1V3", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, parseAppendOtherToVisNonVariable) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("3", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("R0A", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg5 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg5.has_value());
	EXPECT_EQ(slg5->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg5->append("R32Z", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg6 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg6.has_value());
	EXPECT_EQ(slg6->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg6->append("R182", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, parseAppendOtherToVisVariable) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("3", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("R0A", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg5 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg5.has_value());
	EXPECT_EQ(slg5->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg5->append("R32Z", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);

	auto slg6 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg6.has_value());
	EXPECT_EQ(slg6->append("2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg6->append("R182", metaf::ReportPart::RMK), metaf::AppendResult::GROUP_INVALIDATED);
}

TEST(SecondaryLocationGroup, parseAppendOtherToCompleteGroup) {
	auto slg = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg.has_value());
	EXPECT_EQ(slg->append("2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);

	EXPECT_EQ(slg->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 24u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg->direction().has_value());
	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
	ASSERT_TRUE(slg->visibility().isInteger());
	EXPECT_EQ(slg->visibility().integer().value(), 2u);
	EXPECT_EQ(slg->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());

	EXPECT_EQ(slg->append("RWY24R", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("RWY24L", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("SW", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("AND", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("ALL", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("RWY", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("VIS", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("WS", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("VISNO", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("CHINO", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("CIG", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("7", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("1V3", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("3/4", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);
	EXPECT_EQ(slg->append("10", metaf::ReportPart::RMK), metaf::AppendResult::NOT_APPENDED);

	EXPECT_EQ(slg->type(), metaf::SecondaryLocationGroup::Type::VISIBILITY);
	ASSERT_TRUE(slg->runway().has_value());
	EXPECT_EQ(slg->runway()->number(), 24u);
	EXPECT_EQ(slg->runway()->designator(), metaf::Runway::Designator::LEFT);
	EXPECT_FALSE(slg->direction().has_value());
	EXPECT_FALSE(slg->height().isReported());
	EXPECT_FALSE(slg->minHeight().isReported());
	EXPECT_FALSE(slg->maxHeight().isReported());
	ASSERT_TRUE(slg->visibility().isInteger());
	EXPECT_EQ(slg->visibility().integer().value(), 2u);
	EXPECT_EQ(slg->visibility().unit(), metaf::Distance::Unit::STATUTE_MILES);
	EXPECT_EQ(slg->visibility().modifier(), metaf::Distance::Modifier::NONE);
	EXPECT_FALSE(slg->minVisibility().isReported());
	EXPECT_FALSE(slg->maxVisibility().isReported());
}

TEST(SecondaryLocationGroup, isValidNonVariableVisibilityValidRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg2->isValid());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg3->isValid());
}

TEST(SecondaryLocationGroup, isValidNonVariableVisibilityWrongRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg2->isValid());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg3->isValid());
}

TEST(SecondaryLocationGroup, isValidVariableVisibilityValidRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3V5", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1/2V4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg2->isValid());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1/4V1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg3->isValid());

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("1/2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY36", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_TRUE(slg4->isValid());
}

TEST(SecondaryLocationGroup, isValidVariableVisibilityWrongRunway) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3V5", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1/2V4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg2->isValid());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1/4V1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg3->isValid());

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("1/2V3", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY37", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg4->isValid());
}

TEST(SecondaryLocationGroup, isValidVariableVisibilityWrongValues) {
	auto slg1 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg1.has_value());
	EXPECT_EQ(slg1->append("3V2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg1->append("RWY18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg1->isValid());

	auto slg2 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg2.has_value());
	EXPECT_EQ(slg2->append("1V1/2", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg2->append("RWY18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg2->isValid());

	auto slg3 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg3.has_value());
	EXPECT_EQ(slg3->append("1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("1/2V1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg3->append("RWY18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg3->isValid());

	auto slg4 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg4.has_value());
	EXPECT_EQ(slg4->append("1V1", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg4->append("RWY18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg4->isValid());

	auto slg5 = metaf::SecondaryLocationGroup::parse("VIS", metaf::ReportPart::RMK);
	ASSERT_TRUE(slg5.has_value());
	EXPECT_EQ(slg5->append("1/4V1/4", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_EQ(slg5->append("RWY18", metaf::ReportPart::RMK), metaf::AppendResult::APPENDED);
	EXPECT_FALSE(slg5->isValid());
}

///////////////////////////////////////////////////////////////////////////////
// Wrong format generally
///////////////////////////////////////////////////////////////////////////////

TEST(SecondaryLocationGroup, wrongFormat) {
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("W0", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("ALL", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("RWY", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("R32", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("R27C", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("010V012", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("010", metaf::ReportPart::METAR).has_value());
	EXPECT_FALSE(metaf::SecondaryLocationGroup::parse("CCG", metaf::ReportPart::METAR).has_value());
}

