/*
* Copyright (C) 2018-2020 Nick Naumenko (https://gitlab.com/nnaumenko)
* All rights reserved.
* This software may be modified and distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/

#include "gtest/gtest.h"
#include "metaf.hpp"

using namespace std;

/// Get an index from type included in variant type at compile time.
/// @tparam V Variant type to search for index.
/// @tparam T Type which index must be returned.
/// @return Index of type T in variant V. If T is not in V, this won't compile.
template<typename V, typename T, size_t I = 0>
constexpr size_t variant_index() {
    if constexpr (I >= variant_size_v<V>) {
    	return (variant_size_v<V>);
    } else {	
    	if constexpr (is_same_v<variant_alternative_t<I, V>, T>) {
    		return (I);
    	} else {
    		return (variant_index<V, T, I + 1>());
    	}
    }
}

// Visitor which does not return a value
class visitorCounter : public metaf::Visitor<void> {
public:
	std::array<int, std::variant_size_v<metaf::Group>> count = {0};
	int otherGroups = 0; 
protected:
	virtual void visitFixedGroup(const metaf::FixedGroup & group, 
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::FixedGroup>()]++;
	}
	virtual void visitLocationGroup(const metaf::LocationGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::LocationGroup>()]++;
	}
	virtual void visitReportTimeGroup(const metaf::ReportTimeGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::ReportTimeGroup>()]++;
	}
	virtual void visitTrendGroup(const metaf::TrendGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::TrendGroup>()]++;
	}
	virtual void visitWindGroup(const metaf::WindGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::WindGroup>()]++;
	}
	virtual void visitVisibilityGroup(const metaf::VisibilityGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::VisibilityGroup>()]++;
	}
	virtual void visitCloudGroup(const metaf::CloudGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::CloudGroup>()]++;
	}
	virtual void visitWeatherGroup(const metaf::WeatherGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::WeatherGroup>()]++;
	}
	virtual void visitTemperatureGroup(const metaf::TemperatureGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::TemperatureGroup>()]++;
	}
	virtual void visitTemperatureForecastGroup(
		const metaf::TemperatureForecastGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::TemperatureForecastGroup>()]++;
	}
	virtual void visitPressureGroup(const metaf::PressureGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::PressureGroup>()]++;
	}
	virtual void visitRunwayVisualRangeGroup(
		const metaf::RunwayVisualRangeGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::RunwayVisualRangeGroup>()]++;
	}
	virtual void visitRunwayStateGroup(const metaf::RunwayStateGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::RunwayStateGroup>()]++;
	}
	virtual void visitSecondaryLocationGroup(const metaf::SecondaryLocationGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::SecondaryLocationGroup>()]++;
	}
	virtual void visitRainfallGroup(const metaf::RainfallGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::RainfallGroup>()]++;
	}
	virtual void visitSeaSurfaceGroup(const metaf::SeaSurfaceGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::SeaSurfaceGroup>()]++;
	}
	virtual void visitColourCodeGroup(const metaf::ColourCodeGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::ColourCodeGroup>()]++;
	}
	virtual void visitMinMaxTemperatureGroup(const metaf::MinMaxTemperatureGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::MinMaxTemperatureGroup>()]++;
	}
	virtual void visitPrecipitationGroup(const metaf::PrecipitationGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::PrecipitationGroup>()]++;
	}
	virtual void visitLayerForecastGroup(const metaf::LayerForecastGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::LayerForecastGroup>()]++;
	}
	virtual void visitPressureTendencyGroup(const metaf::PressureTendencyGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::PressureTendencyGroup>()]++;
	}
	virtual void visitCloudTypesGroup(const metaf::CloudTypesGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::CloudTypesGroup>()]++;
	}
	virtual void visitCloudLayersGroup(const metaf::CloudLayersGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::CloudLayersGroup>()]++;
	}

	virtual void visitLightningGroup(const metaf::LightningGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::LightningGroup>()]++;
	}

	virtual void visitVicinityGroup(const metaf::VicinityGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::VicinityGroup>()]++;
	}

	virtual void visitMiscGroup(const metaf::MiscGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::MiscGroup>()]++;
	}

	virtual void visitUnknownGroup(const metaf::UnknownGroup & group,
		metaf::ReportPart reportPart,
		const std::string & rawString)
	{
		(void)group; (void)reportPart; (void)rawString;
		count[variant_index<metaf::Group, metaf::UnknownGroup>()]++;
	}
};

static const std::vector<std::string> testReports = {
	"TAF BGTL 060900Z 0609/0715 VRB06KT 8000 -SHRASN OVC003 610105 QNH2980INS"
	" BECMG 0614/0615 VRB06KT 9999 NSW BKN005 OVC015 620105 QNH2983INS "
	"TX03/0620Z TNM02/0709Z"s,

	"METAR EGYD 281050Z 11015KT 5000 -SHSN FEW004 BKN012CB BKN025"
	" M04/M05 Q1020 BLACKGRN"s,

	"METAR PABR 091953Z 28006KT 3/4SM R07/2000V4500FT BR OVC003 06/06 A3006"
	" RMK SLP179 VIS 1/2V1 T00560056 VIA AUTODIAL"s,

	"METAR CYHI 171900Z CCA 34019KT 10SM -SN DRSN VCSH VCFG"
	" FEW011 BKN030 BKN040 BKN060 BKN150 M05/M09 A2994"
	" RMK SC1ST4SC1SC1AC1 SC TR AC TR CIG RAG VIS SW 15 SLP144"s,

	"SPECI UKLL 202304Z 00000MPS 4800 1100SE R31/1900 BR NSC 11/10 Q1020 R31/CLRD//"
	" TEMPO 0400 FG VV002"s,

	"METAR SKBG 232200Z 31006KT 9999 FEW015 26/19 A2989 WS R35"s,

	"METAR YPKG 232300Z AUTO 26013KT 9999 // NCD 17/10 Q1010 RF00.0////./"s,

	"METAR ENSE 201620Z AUTO 24040KT 9999NDV FEW010/// SCT020///"
	" BKN027/// 12/10 Q0988 W///S7"s,

	"SPECI MMMM 171315Z 00000KT 5SM VCFG SCT008 BKN080 BKN230 15/15 A3023"
	" RMK 8/578 HZY"s,

	"METAR EGUL 211156Z 25014G22KT 9999 FEW046 SCT055 14/04 A2971"
	" RMK AO2A PK WND 24029/06 SLP064 70021 T01370043 10145 20103 52016 $"s,

	"METAR KABQ 092052Z 23004KT 10SM FEW080 FEW120 FEW180 SCT300 29/07 A3005"
	" RMK AO2 LTG DSNT SE SLP085 CB DSNT N-NE AND SE T02940067 58025"s
};


TEST(Visitor, visitorVoid) {

	visitorCounter v;

	for (const auto & report : testReports) {
		const auto result = metaf::Parser::parse(report);
			for (const auto & groupInfo : result.groups) {
				v.visit(groupInfo.group);
			}
	}

	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::FixedGroup>()]), 22);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::LocationGroup>()]), 11);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::ReportTimeGroup>()]), 11);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::TrendGroup>()]), 3);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::WindGroup>()]), 13);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::VisibilityGroup>()]), 16);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::CloudGroup>()]), 28);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::WeatherGroup>()]), 8);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::TemperatureGroup>()]), 13);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::TemperatureForecastGroup>()]), 2);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::PressureGroup>()]), 16);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::RunwayVisualRangeGroup>()]), 2);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::RunwayStateGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::SecondaryLocationGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::RainfallGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::SeaSurfaceGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::ColourCodeGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::MinMaxTemperatureGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::PrecipitationGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::LayerForecastGroup>()]), 2);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::PressureTendencyGroup>()]), 2);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::CloudTypesGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::CloudLayersGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::LightningGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::VicinityGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::MiscGroup>()]), 1);
	EXPECT_EQ((v.count[variant_index<metaf::Group, metaf::UnknownGroup>()]), 3);
}
