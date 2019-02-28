/*
* Copyright (C) 2018-2019 Nick Naumenko (https://gitlab.com/nnaumenko)
* All rights reserved.
* This software may be modified and distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/

#include "gtest/gtest.h"
#include "metaf.h"

TEST(getSyntaxGroup, METAR) {
	const auto g = metaf::FixedGroup::parse("METAR", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::METAR);
}

TEST(getSyntaxGroup, SPECI) {
	const auto g = metaf::FixedGroup::parse("SPECI", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::SPECI);
}

TEST(getSyntaxGroup, TAF) {
	const auto g = metaf::FixedGroup::parse("TAF", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::TAF);
}

TEST(getSyntaxGroup, COR) {
	const auto g = metaf::FixedGroup::parse("COR", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::COR);
}

TEST(getSyntaxGroup, AMD) {
	const auto g = metaf::FixedGroup::parse("AMD", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::AMD);
}

TEST(getSyntaxGroup, NIL) {
	const auto g = metaf::FixedGroup::parse("NIL", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::NIL);
}

TEST(getSyntaxGroup, CNL) {
	const auto g = metaf::FixedGroup::parse("CNL", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::CNL);
}

TEST(getSyntaxGroup, RMK) {
	const auto g = metaf::FixedGroup::parse("RMK", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::RMK);
}

TEST(getSyntaxGroup, LOCATION) {
	const auto g = metaf::LocationGroup::parse("UKLL", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::LOCATION);
}

TEST(getSyntaxGroup, REPORT_TIME) {
	const auto g = metaf::ReportTimeGroup::parse("081230Z", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::REPORT_TIME);
}

TEST(getSyntaxGroup, TIME_SPAN) {
	const auto g = metaf::TrendGroup::parse("0812/0824", metaf::ReportPart::HEADER);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::TIME_SPAN);
}

TEST(getSyntaxGroup, OTHER_AUTO) {
	const auto g = metaf::FixedGroup::parse("AUTO", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_R_SNOCLO) {
	const auto g = metaf::FixedGroup::parse("SNOCLO", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CAVOK) {
	const auto g = metaf::FixedGroup::parse("CAVOK", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_NSW) {
	const auto g = metaf::FixedGroup::parse("NSW", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_NOSIG) {
	const auto g = metaf::TrendGroup::parse("NOSIG", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_TEMPO) {
	const auto g = metaf::TrendGroup::parse("TEMPO", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_BECMG) {
	const auto g = metaf::TrendGroup::parse("BECMG", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_INTER) {
	const auto g = metaf::TrendGroup::parse("INTER", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_TrendFrom) {
	const auto g = metaf::TrendGroup::parse("FM191445", metaf::ReportPart::TAF);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_TrendTime) {
	const auto g = metaf::TrendGroup::parse("TL1445", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_Probability) {
	const auto g = metaf::TrendGroup::parse("PROB40", metaf::ReportPart::TAF);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_PlainTextGroup) {
	const auto g = metaf::PlainTextGroup::parse("A1B2C3D4", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_WindGroup) {
	const auto g = metaf::WindGroup::parse("27005KT", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_VarWindGroup) {
	const auto g = metaf::WindGroup::parse("240V250", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_VisibilityGroup) {
	const auto g = metaf::VisibilityGroup::parse("9999NDV", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudGroup) {
	const auto g = metaf::CloudGroup::parse("BKN024", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudClr) {
	const auto g = metaf::CloudGroup::parse("CLR", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudSkc) {
	const auto g = metaf::CloudGroup::parse("SKC", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudNsc) {
	const auto g = metaf::CloudGroup::parse("NSC", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudNcd) {
	const auto g = metaf::CloudGroup::parse("NCD", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_CloudVerticalVisibility) {
	const auto g = metaf::CloudGroup::parse("VV015", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_WeatherGroup) {
	const auto g = metaf::WeatherGroup::parse("+SHSN", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_TemperatureGroup) {
	const auto g = metaf::TemperatureGroup::parse("07/02", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_TemperatureForecastGroup) {
	const auto g = metaf::TemperatureForecastGroup::parse("TX07/1105Z", metaf::ReportPart::TAF);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_PressureGroup) {
	const auto g = metaf::PressureGroup::parse("Q1033", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_RunwayVisualRangeGroup) {
	const auto g = metaf::RunwayVisualRangeGroup::parse("R30/5000VP6000FT/U", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_RunwayStateGroup) {
	const auto g = metaf::RunwayStateGroup::parse("R16/750155", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_RainfallGroup) {
	const auto g = metaf::RainfallGroup::parse("RF00.0/000.0", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

TEST(getSyntaxGroup, OTHER_SeaSurfaceGroup) {
	const auto g = metaf::SeaSurfaceGroup::parse("W15/S4", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}

 TEST(getSyntaxGroup, OTHER_ColourCodeGroup) {
	const auto g = metaf::ColourCodeGroup::parse("AMB", metaf::ReportPart::METAR);
	ASSERT_TRUE(g.has_value());
	EXPECT_EQ(metaf::getSyntaxGroup(g.value()), metaf::SyntaxGroup::OTHER);
}