/*
* Copyright (C) 2018-2019 Nick Naumenko (https://gitlab.com/nnaumenko)
* All rights reserved.
* This software may be modified and distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/

/// @file 
/// @brief This example parses METAR or TAF reports displays them in human-readable format.

#include "metaf.h"
#include <vector>
#include <sstream>
#include <cmath>
#include <tuple>
#include <emscripten/emscripten.h>

using namespace std::literals;

class GroupVisitorExplain : public metaf::GroupVisitor<std::string> {
public:
	static std::string_view reportTypeToString(metaf::ReportType reportType);
	static std::string_view reportErrorToString(metaf::Parser::Error error);
private:
	virtual std::string visitPlainTextGroup(const metaf::PlainTextGroup & group);
	virtual std::string visitFixedGroup(const metaf::FixedGroup & group);
	virtual std::string visitLocationGroup(const metaf::LocationGroup & group);
	virtual std::string visitReportTimeGroup(const metaf::ReportTimeGroup & group);
	virtual std::string visitTrendGroup(const metaf::TrendGroup & group);
	virtual std::string visitWindGroup(const metaf::WindGroup & group);
	virtual std::string visitVisibilityGroup(const metaf::VisibilityGroup & group);
	virtual std::string visitCloudGroup(const metaf::CloudGroup & group);
	virtual std::string visitWeatherGroup(const metaf::WeatherGroup & group);
	virtual std::string visitTemperatureGroup(const metaf::TemperatureGroup & group);
	virtual std::string visitTemperatureForecastGroup(
		const metaf::TemperatureForecastGroup & group);
	virtual std::string visitPressureGroup(const metaf::PressureGroup & group);
	virtual std::string visitRunwayVisualRangeGroup(const metaf::RunwayVisualRangeGroup & group);
	virtual std::string visitRunwayStateGroup(const metaf::RunwayStateGroup & group);
	virtual std::string visitWindShearLowLayerGroup(const metaf::WindShearLowLayerGroup & group);
	virtual std::string visitRainfallGroup(const metaf::RainfallGroup & group);
	virtual std::string visitSeaSurfaceGroup(const metaf::SeaSurfaceGroup & group);
	virtual std::string visitColourCodeGroup(const metaf::ColourCodeGroup & group);
	virtual std::string visitOther(const metaf::Group & group);

	static std::string explainRunway(const metaf::Runway & runway);
	static std::string explainMetafTime(const metaf::MetafTime & metafTime);
	static std::string explainTemperature(const metaf::Temperature & temperature);
	static std::string explainSpeed(const metaf::Speed & speed);
	static std::string explainDistance(const metaf::Distance & distance);
	static std::string explainDirection(const metaf::Direction & direction,
		bool trueCardinalDirections = false);
	static std::string explainPressure(const metaf::Pressure & pressure);
	static std::string explainPrecipitation(const metaf::Precipitation & precipitation);
	static std::string explainSurfaceFriction(const metaf::SurfaceFriction & surfaceFriction);
	static std::string explainWaveHeight(const metaf::WaveHeight & waveHeight);

	static std::string_view speedUnitToString(metaf::Speed::Unit unit);
	static std::string_view distanceUnitToString(metaf::Distance::Unit unit);
	static std::string_view cardinalDirectionToString(metaf::Direction::Cardinal cardinal);
	static std::string_view brakingActionToString(
		metaf::SurfaceFriction::BrakingAction brakingAction);
	static std::string_view stateOfSeaSurfaceToString(
		metaf::WaveHeight::StateOfSurface stateOfSurface);
	static std::string_view trendTypeToString(metaf::TrendGroup::Type type);
	static std::string_view cloudAmountToString(metaf::CloudGroup::Amount amount);
	static std::string_view cloudTypeToString(metaf::CloudGroup::Type type);
	static std::string_view weatherQualifierToString(metaf::WeatherGroup::Qualifier qualifier);
	static std::string_view weatherDescriptorToString(metaf::WeatherGroup::Descriptor descriptor);
	static std::string_view weatherPhenomenaToString(metaf::WeatherGroup::Weather weather);
	static std::string_view specialWeatherPhenomenaToString(const metaf::WeatherGroup & group);
	static std::string_view rvrTrendToString(metaf::RunwayVisualRangeGroup::Trend trend);
	static std::string_view runwayStateDepositsToString(metaf::RunwayStateGroup::Deposits deposits);
	static std::string_view runwayStateExtentToString(metaf::RunwayStateGroup::Extent extent);
	static std::string_view colourCodeToString(metaf::ColourCodeGroup::Code code);
	static unsigned int colourCodeVisibility(metaf::ColourCodeGroup::Code code);
	static unsigned int colourCodeCeiling(metaf::ColourCodeGroup::Code code);

	static std::string roundTo(float number, size_t digitsAfterDecimalPoint);

	static const inline std::string lineBreak = std::string("<br>");
	static const inline std::string groupNotValidMessage = 
		std::string("Data in this group may be errorneous, incomplete or inconsistent");
};

std::string GroupVisitorExplain::visitPlainTextGroup(const metaf::PlainTextGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "This group is not recognised by parser: " << group.toString();
	return(result.str());
}

std::string GroupVisitorExplain::visitFixedGroup(const metaf::FixedGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	switch (group.type()) {
		case metaf::FixedGroup::Type::METAR:
		result << "Report type: METAR (weather observation report)";
		break;

		case metaf::FixedGroup::Type::SPECI:
		result << "Report type: unscheduled METAR (weather observation report)" << lineBreak;
		result << "Unscheduled report is issued dut to sudden changes in weather ";
		result << "conditions: wind shift, visibility decrease, severe weather, ";
		result << "clouds formed or dissipated, etc.";
		break;

		case metaf::FixedGroup::Type::TAF:
		result << "Report type: TAF (terminal aerodrome forecast)";
		break;

		case metaf::FixedGroup::Type::AMD:
		result << "Amended report";
		break;

		case metaf::FixedGroup::Type::NIL:
		result << "Missing report";
		break;

		case metaf::FixedGroup::Type::CNL:
		result << "Cancelled report";
		break;

		case metaf::FixedGroup::Type::COR:
		result << "Correctional report";
		break;

		case metaf::FixedGroup::Type::AUTO:
		result << "Fully automated report with no human intervention or oversight";
		break;

		case metaf::FixedGroup::Type::NSW:
		result << "No significant weather" << lineBreak;
		result << "Indicates end of significant weather phenomena";
		break;

		case metaf::FixedGroup::Type::R_SNOCLO:
		result << "Aerodrome is closed due to snow accumulation";
		break;

		case metaf::FixedGroup::Type::CAVOK:
		result << "Ceiling and visibility OK" << lineBreak;
		result << "Visibility 10 km or more in all directions, ";
		result << "no cloud below 5000 feet (1500 meters), ";
		result << "no cumulonimbus or towering cumulus clouds, ";
		result << "no significant weather phenomena";
		break;

		case metaf::FixedGroup::Type::RMK:
		result << "The remarks are as follows" << lineBreak;
		result << "Note: this version does not recognise or decode remarks";
		break;

		case metaf::FixedGroup::Type::WSCONDS:
		result << "Potential wind shear conditions are present ";
		result << "but there's not enough information to reliably forecast ";
		result << "height, direction and speed of wind shear";
		break;

		default:
		result << "Unknown fixed group";
		break;
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitLocationGroup(const metaf::LocationGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "ICAO code for location: " << group.toString();
	return(result.str());
}

std::string GroupVisitorExplain::visitReportTimeGroup(
	const metaf::ReportTimeGroup & group)
{
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Day and time of report issue: " << explainMetafTime(group.time());
	return(result.str());
}

std::string GroupVisitorExplain::visitTrendGroup(const metaf::TrendGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << trendTypeToString(group.type()) << lineBreak;

	switch (group.probability()) {
		case metaf::TrendGroup::Probability::PROB_30:
		result << "Trend probability is 30 percent" << lineBreak;;
		break;

		case metaf::TrendGroup::Probability::PROB_40:
		result << "Trend probability is 40 percent" << lineBreak;;
		break;

		case metaf::TrendGroup::Probability::NONE:
		if (group.type() == metaf::TrendGroup::Type::BECMG ||
			group.type() == metaf::TrendGroup::Type::TEMPO ||
			group.type() == metaf::TrendGroup::Type::INTER)
		{
			result << "Trend probability is 50 percent or more" << lineBreak;;
		}
		break;

		default:
		result << "[Unknown probability value]" << lineBreak;;
		break;
	}
	if (const auto timeFrom = group.timeFrom(); timeFrom) {
		result << "From " << explainMetafTime(*timeFrom) << lineBreak;
	}
	if (const auto timeTill = group.timeTill(); timeTill) {
		result << "Until " << explainMetafTime(*timeTill) << lineBreak;
	}
	if (const auto timeAt = group.timeAt(); timeAt) {
		result << "At " << explainMetafTime(*timeAt) << lineBreak;
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitWindGroup(const metaf::WindGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	if (group.isSurfaceWind()) result << "Surface wind:";
	if (group.isWindShear()) {
		result << "Wind shear at height ";
		result << explainDistance(group.windShearHeight()) << ':';
	}
	result << lineBreak;

	if (group.isCalm()) {
		// Calm wind
		result << "No wind / Calm";
		return(result.str());
	}

	if (group.direction().status() != metaf::Direction::Status::OMMITTED || 
		group.windSpeed().speed().has_value())
	{
		// General wind information present in this group
		result << "Wind direction: " << explainDirection(group.direction(), true) << lineBreak;
		result << "Wind speed: " << explainSpeed(group.windSpeed()) << lineBreak;
		if (group.gustSpeed().speed().has_value()) {
			result  << "Gust speed: " << explainSpeed(group.gustSpeed()) << lineBreak;
		}
	}
	if (group.hasVariableSector()) {
		result << "Variable wind direction sector from ";
		result << explainDirection(group.varSectorBegin()) << " clockwise to ";
		result << explainDirection(group.varSectorEnd());
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitVisibilityGroup(const metaf::VisibilityGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Visibility ("; 
	if (group.isPrevailing()) result << "prevailing";
	if (group.isDirectional())result << explainDirection(group.direction());
	result << ") ";
	result << explainDistance(group.visibility());
	return(result.str());
}

std::string GroupVisitorExplain::visitCloudGroup(const metaf::CloudGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << cloudAmountToString(group.amount());
	if (const auto typeStr = cloudTypeToString(group.type()); !typeStr.empty()) {
		result << ", type: " << typeStr;
	}
	result << lineBreak;
	if (group.height().isReported()) {
		result << "Base height " << explainDistance(group.height()) << lineBreak;
	}
	if (group.verticalVisibility().isReported()) {
		result << "Vertical visibility " << explainDistance(group.verticalVisibility()) << lineBreak;
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitWeatherGroup(const metaf::WeatherGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	if (group.qualifier() != metaf::WeatherGroup::Qualifier::RECENT) {
		result << "Weather phenomena: ";
	} else {
		result << "Recent weather: ";
	}

	if (const auto weatherStr = specialWeatherPhenomenaToString(group); !weatherStr.empty()) {
		result << weatherStr;
		return(result.str());
	}

	const bool vicinity = (group.qualifier() == metaf::WeatherGroup::Qualifier::VICINITY);
	const bool showers = (group.descriptor() == metaf::WeatherGroup::Descriptor::SHOWERS);
	const bool thunderstorm = (group.descriptor() == metaf::WeatherGroup::Descriptor::THUNDERSTORM);
	
	if (const auto q = weatherQualifierToString(group.qualifier()); !q.empty()) result << q << " ";
	if (const auto d = weatherDescriptorToString(group.descriptor()); !d.empty()) result << d << " ";

	metaf::WeatherGroup::Weather previous = metaf::WeatherGroup::Weather::NOT_REPORTED;
	const auto weatherPhenomena = group.weather();
	for (const auto w : weatherPhenomena) {
		std::string weatherStr(weatherPhenomenaToString(w));
		if (previous == metaf::WeatherGroup::Weather::RAIN && 
			w == metaf::WeatherGroup::Weather::SNOW) {
				weatherStr = "and snow mix";
		}
		if (previous == metaf::WeatherGroup::Weather::SNOW && 
			w == metaf::WeatherGroup::Weather::RAIN) {
				weatherStr = "and rain mix";
		}
		result << weatherStr << " ";
		previous = w;
	}
	if (showers) result << "showers ";
	if (thunderstorm) {
		if (weatherPhenomena.size()) result << "with ";
		result << "thunderstorm ";
	}
	if (vicinity) result << "in vicinity (5 to 10 miles away)";
	return(result.str());
}

std::string GroupVisitorExplain::visitTemperatureGroup(const metaf::TemperatureGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Air temperature: " << explainTemperature(group.airTemperature()) << lineBreak;
	result << "Dew point: " << explainTemperature(group.dewPoint()) << lineBreak;
	if (const auto rh = group.relativeHumidity(); rh.has_value()) {
		result << "Relative humidity: " << static_cast<int>(*rh) << " percent"; 
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitTemperatureForecastGroup(
	const metaf::TemperatureForecastGroup & group)
{
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	switch(group.point()) {
		case metaf::TemperatureForecastGroup::Point::MINIMUM: 
		result << "Minimum temperature";
		break;

		case metaf::TemperatureForecastGroup::Point::MAXIMUM:
		result << "Maximum temperature";
		break;

		default:
		result << "[unknown temperature point]";
		break;
	}
	result << " " << explainTemperature(group.airTemperature()) << ", ";
	result << "expected on " << explainMetafTime(group.time());
	return(result.str());
}

std::string GroupVisitorExplain::visitPressureGroup(const metaf::PressureGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Atmospheric pressure: " << explainPressure(group.atmosphericPressure());
	return(result.str());
}

std::string GroupVisitorExplain::visitRunwayVisualRangeGroup(
	const metaf::RunwayVisualRangeGroup & group)
{
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Visual range of " << explainRunway(group.runway()) << " is ";
	if (group.isVariableVisualRange()) {
		result << explainDistance(group.maxVisualRange());
		result << " to " << explainDistance(group.maxVisualRange());
	} else {
		result << explainDistance(group.visualRange());
	}
	if (group.trend() != metaf::RunwayVisualRangeGroup::Trend::NONE) {
		result << ", and the trend is " << rvrTrendToString(group.trend());
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitRunwayStateGroup(const metaf::RunwayStateGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "State of " << explainRunway(group.runway()) << ": ";
	switch (group.status()) {
		case metaf::RunwayStateGroup::Status::NORMAL:
		result << runwayStateDepositsToString(group.deposits()) << lineBreak;
		if (group.deposits() != metaf::RunwayStateGroup::Deposits::CLEAR_AND_DRY) {
		    result << "Depth of deposits on runway: ";
		    result << explainPrecipitation(group.depositDepth()) << lineBreak;
			result << "Runway contamination extent: ";
			result << runwayStateExtentToString(group.contaminationExtent()) << lineBreak;
		}
		result << "Surface friction: ";
		result << explainSurfaceFriction(group.surfaceFriction());
		break;

		case metaf::RunwayStateGroup::Status::CLRD:
		result << "deposits on runway were cleared or ceased to exist" << lineBreak;
		result << "Surface friction: ";
		result << explainSurfaceFriction(group.surfaceFriction());
		break;

		case metaf::RunwayStateGroup::Status::SNOCLO:
		result << "closed due to snow accumulation";
		break;

		default:
		result << "[unknown runway status]";
		break;
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitWindShearLowLayerGroup(
	const metaf::WindShearLowLayerGroup & group)
{
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Wind shear significant to aircraft operations is present along ";
	result << "the take-off path or approach path ";
	result << "between runway level and 500 metres (1 600 ft)";
	if (group.isValid()) result << " at " << explainRunway(group.runway());
	return(result.str());
}


std::string GroupVisitorExplain::visitRainfallGroup(const metaf::RainfallGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Rainfall for last 10 minutes ";
	result << explainPrecipitation(group.rainfallLast10Minutes()) << ", ";
	if (group.rainfallLast60Minutes().status() == metaf::Precipitation::Status::REPORTED) {
		result << "for last 60 minutes ";
		result << explainPrecipitation(group.rainfallLast60Minutes()) << ", ";
	}
	result << "total rainfall since 9:00 AM ";
	result << explainPrecipitation(group.rainfallLast60Minutes());
	return(result.str());
}

std::string GroupVisitorExplain::visitSeaSurfaceGroup(const metaf::SeaSurfaceGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	result << "Sea surface temperature " << explainTemperature(group.surfaceTemperature()) << ", ";
	result << explainWaveHeight(group.waves());
	return(result.str());
}

std::string GroupVisitorExplain::visitColourCodeGroup(const metaf::ColourCodeGroup & group) {
	std::ostringstream result;
	if (!group.isValid()) result << groupNotValidMessage << lineBreak;
	if (group.isCodeBlack()) {
		result << "Colour code BLACK: ";
		result << "aerodrome closed due to snow accumulation or non-weather reasons" << lineBreak;
	}
	result << "Colour code " << colourCodeToString(group.code()) << ": ";
	if (const auto [vis, ceiling] = 
			std::pair(colourCodeVisibility(group.code()), colourCodeCeiling(group.code()));
		vis && ceiling)
	{
		if (group.code() == metaf::ColourCodeGroup::Code::RED) {
			result << "either visibility &lt; " << vis;
			result << " or lowest cloud base height &lt; " << ceiling;
		} else {
			result << "visibility &gt;" << vis;
			result << " and lowest cloud base height &gt;" << ceiling;
		}
	}
	return(result.str());
}

std::string GroupVisitorExplain::visitOther(const metaf::Group & group) {
	(void)group;
	return("This group is recognised by parser but not listed");
}

std::string_view GroupVisitorExplain::reportTypeToString(metaf::ReportType reportType) {
	switch (reportType) {
		case metaf::ReportType::UNKNOWN:	return("unable to detect");
		case metaf::ReportType::METAR:		return("METAR (weather observation)");
		case metaf::ReportType::TAF:		return("TAF (weather forecast)");
		default: return("unknown report type");
	}
}

std::string_view GroupVisitorExplain::reportErrorToString(metaf::Parser::Error error) {
	switch (error) {
		case metaf::Parser::Error::NONE:
		return("no error");

		case metaf::Parser::Error::EMPTY_REPORT:
		return("report is empty");
				
		case metaf::Parser::Error::EXPECTED_REPORT_TYPE_OR_LOCATION:
		return("expected report type or ICAO location at report start");
				
		case metaf::Parser::Error::EXPECTED_LOCATION:
		return("expected ICAO location");
				
		case metaf::Parser::Error::EXPECTED_REPORT_TIME:
		return("expected report time");
				
		case metaf::Parser::Error::EXPECTED_TIME_SPAN:
		return("expected time span");
				
		case metaf::Parser::Error::UNEXPECTED_REPORT_END:
		return("unexpected report end");
				
		case metaf::Parser::Error::UNEXPECTED_GROUP_AFTER_NIL:
		return("unexpected group after NIL");
				
		case metaf::Parser::Error::UNEXPECTED_GROUP_AFTER_CNL:
		return("unexpected group after CNL");
				
		case metaf::Parser::Error::UNEXPECTED_NIL_OR_CNL_IN_REPORT_BODY:
		return("unexpected NIL or CNL in report body");
				
		case metaf::Parser::Error::AMD_ALLOWED_IN_TAF_ONLY:
		return("AMD is allowed only in TAF reports");
				
		case metaf::Parser::Error::CNL_ALLOWED_IN_TAF_ONLY:
		return("CNL is allowed only in TAF reports");
				
		case metaf::Parser::Error::INTERNAL_PARSER_STATE:
		return("internal error, unknown parser state");
				
		default: 
		return("unknown error");
	}
}

std::string GroupVisitorExplain::explainRunway(const metaf::Runway & runway) {
	if (runway.isAllRunways()) return("all runways");
	if (runway.isMessageRepetition()) return("same runway (repetition of last message)"); 
	std::ostringstream result;
	result << "runway ";
	result << (runway.number() < 10 ? "0"s : ""s) << std::to_string(runway.number());
	std::string designatorStr;
	switch(runway.designator()) {
		case metaf::Runway::Designator::NONE: 	break;
		case metaf::Runway::Designator::LEFT: 	designatorStr = "LEFT"; break;
		case metaf::Runway::Designator::CENTER:	designatorStr = "CENTER"; break;
		case metaf::Runway::Designator::RIGHT:  designatorStr = "RIGHT"; break;
		default: designatorStr = "(unknown runway designator)"; break;
	}
	if (!designatorStr.empty()) result << " " << designatorStr;
	return(result.str());
}

std::string GroupVisitorExplain::explainMetafTime(const metaf::MetafTime & metafTime) {
	static const std::string hourMinuteSeparator(":");
	std::ostringstream result;
	if (const auto day = metafTime.day(); day.has_value()) {
		result << "day " << *day << ", ";
	}
	result << (metafTime.hour() < 10 ? "0"s : ""s) << metafTime.hour();
	result << hourMinuteSeparator;
	result << (metafTime.minute() < 10 ? "0"s : ""s) << metafTime.minute();
	result << " GMT";
	return(result.str());
}

std::string GroupVisitorExplain::explainTemperature(const metaf::Temperature & temperature) {
	if (!temperature.temperature().has_value()) return("not reported");
	std::ostringstream result;
	if (!temperature.temperature().value() && temperature.isFreezing()) result << "slightly less than ";
	if (!temperature.temperature().value() && !temperature.isFreezing()) result << "slightly more than ";
	if (const auto t = temperature.toUnit(metaf::Temperature::Unit::C); t.has_value()) {
		result << static_cast<int>(*t) << " &deg;C";
	} else {
		result << "[unable to convert temperature to &deg;C]";
	}
	result << " / ";
	if (const auto t = temperature.toUnit(metaf::Temperature::Unit::F); t.has_value()) {
		result << static_cast<int>(*t) << " &deg;F";
	} else {
		result << "[unable to convert temperature to &deg;F]";
	}
	return(result.str());
}

std::string GroupVisitorExplain::explainSpeed(const metaf::Speed & speed) {
	std::ostringstream result;
	if (const auto s = speed.speed(); s.has_value()) {
		result << *speed.speed() << " " << speedUnitToString(speed.unit());
	} else {
		return("not reported");
	}
	result << " (";
	if (speed.unit() != metaf::Speed::Unit::KNOTS) {
		if (const auto s = speed.toUnit(metaf::Speed::Unit::KNOTS); s.has_value()) {
			result << roundTo(*s, 1);
			result << " " << speedUnitToString(metaf::Speed::Unit::KNOTS);
		} else {
			result << "[unable to convert speed to knots]";
		}
		result << " / ";
	}
	if (speed.unit() != metaf::Speed::Unit::METERS_PER_SECOND) {
		if (const auto s = speed.toUnit(metaf::Speed::Unit::METERS_PER_SECOND); s.has_value()) {
			result << roundTo(*s, 1);
			result << " " << speedUnitToString(metaf::Speed::Unit::METERS_PER_SECOND);
		} else {
			result << "[unable to convert speed to m/s]";
		}
		result << " / ";
	}
	if (speed.unit() != metaf::Speed::Unit::KILOMETERS_PER_HOUR) {
		if (const auto s = speed.toUnit(metaf::Speed::Unit::KILOMETERS_PER_HOUR); s.has_value()) {
			result << roundTo(*s, 1);
			result << " " << speedUnitToString(metaf::Speed::Unit::KILOMETERS_PER_HOUR);
		} else {
			result << "[unable to convert speed to km/h]";
		}
		if (speed.unit() != metaf::Speed::Unit::MILES_PER_HOUR) result << " / ";
	}
	if (speed.unit() != metaf::Speed::Unit::MILES_PER_HOUR) {
		if (const auto s = speed.toUnit(metaf::Speed::Unit::MILES_PER_HOUR); s.has_value()) {
			result << roundTo(*s, 1);
			result << " " << speedUnitToString(metaf::Speed::Unit::MILES_PER_HOUR);
		} else {
			result << "[unable to convert speed to mph]";
		}
	}
	result << ")";
	return(result.str());
}

std::string GroupVisitorExplain::explainDistance(const metaf::Distance & distance) {
	if (!distance.isReported()) return("not reported");
	std::ostringstream result;
	switch (distance.modifier()) {
		case metaf::Distance::Modifier::NONE:			break;
		case metaf::Distance::Modifier::LESS_THAN:		result << "&lt;"; break;
		case metaf::Distance::Modifier::MORE_THAN:		result << "&gt;"; break;
		default: result << "unknown modifier, "; break;
	}
	const auto d = distance.toUnit(distance.unit());
	if (!d.has_value()) return("[unable to get distance's floating-point value]");
	if (distance.unit() == metaf::Distance::Unit::STATUTE_MILES) {
		result << roundTo(*d, 3);
	} else {
		result << static_cast<int>(*d);
	}
	result << " " << distanceUnitToString(distance.unit());
	result << " (";
	if (distance.unit() != metaf::Distance::Unit::METERS) {
		if (const auto d = distance.toUnit(metaf::Distance::Unit::METERS); d.has_value()) {
			result << static_cast<int>(*d);
			result << " " << distanceUnitToString(metaf::Distance::Unit::METERS);
		} else {
			result << "[unable to convert distance to meters]";
		}
		result << " / ";
	}
	if (distance.unit() != metaf::Distance::Unit::STATUTE_MILES) {
		if (const auto d = distance.toUnit(metaf::Distance::Unit::STATUTE_MILES); d.has_value()) {
			result << roundTo(*d, 3);
			result << " " << distanceUnitToString(metaf::Distance::Unit::STATUTE_MILES);
		} else {
			result << "[unable to convert distance to statute miles]";
		}
		if (distance.unit() != metaf::Distance::Unit::FEET) result << " / ";
	}
	if (distance.unit() != metaf::Distance::Unit::FEET) {
		if (const auto d = distance.toUnit(metaf::Distance::Unit::FEET); d.has_value()) {
			result << static_cast<int>(*d);
			result << " " << distanceUnitToString(metaf::Distance::Unit::FEET);
		} else {
			result << "[unable to convert distance to feet]";
		}
	}
	result << ")";
	return(result.str());
}

std::string GroupVisitorExplain::explainDirection(const metaf::Direction & direction,
	bool trueCardinalDirections)
{
	std::ostringstream result;
	switch (direction.status()) {
		case metaf::Direction::Status::OMMITTED:
		return(std::string());

		case metaf::Direction::Status::NOT_REPORTED:
		return("not reported");

		case metaf::Direction::Status::VARIABLE:
		return("variable");

		case metaf::Direction::Status::NDV:
		return("no directional variation");

		case metaf::Direction::Status::VALUE_DEGREES:
		if (const auto d = direction.degrees(); d.has_value()) {
			result << *d << " degrees";
		} else {
			result << "[unable to produce value in &deg;]";
		}
		
		case metaf::Direction::Status::VALUE_CARDINAL:
		if (const auto c = cardinalDirectionToString(direction.cardinal(trueCardinalDirections)); 
			!c.empty()) {
				if (direction.status() == metaf::Direction::Status::VALUE_DEGREES) result << "(";
				result << c; 
				if (direction.status() == metaf::Direction::Status::VALUE_DEGREES) result << ")";
		}
		break;

		default:
		return("unknown direction status");
	}
	return(result.str());
}

std::string GroupVisitorExplain::explainPressure(const metaf::Pressure & pressure) {
	if (!pressure.pressure().has_value()) return("not reported");
	std::ostringstream result;
	if (const auto phpa = pressure.toUnit(metaf::Pressure::Unit::HECTOPASCAL); phpa.has_value()) {
		result << static_cast<int>(*phpa) << " hPa";
	} else {
		result << "[unable to convert pressure to hPa]";
	}
	result << " / ";
	if (const auto pinhg = pressure.toUnit(metaf::Pressure::Unit::INCHES_HG); pinhg.has_value()) {
		result << roundTo(*pinhg, 2) << " inHg";
	} else {
		result << "[unable to convert pressure to inHg]";
	}
	return(result.str());
}

std::string GroupVisitorExplain::explainPrecipitation(const metaf::Precipitation & precipitation) {
	std::ostringstream result;
	switch (precipitation.status()) {
		case metaf::Precipitation::Status::NOT_REPORTED:
		return("not reported");

		case metaf::Precipitation::Status::RUNWAY_NOT_OPERATIONAL:
		return("not measured because runway is not operational");

		case metaf::Precipitation::Status::REPORTED:
		if (const auto p = precipitation.toUnit(metaf::Precipitation::Unit::MM); p.has_value()) {
			result << static_cast<int>(*p) << " mm";
		} else {
			result << "[unable to convert precipitation to mm]";
		}
		result << " / ";
		if (const auto p = precipitation.toUnit(metaf::Precipitation::Unit::INCHES); p.has_value()) {
			result << roundTo(*p, 2) << " inches";
		} else {
			result << "[unable to convert precipitation to inches]";
		}
		break;

		default:
		return("unknown precipitation status");
	}
	return(result.str());
}

std::string GroupVisitorExplain::explainSurfaceFriction(
	const metaf::SurfaceFriction & surfaceFriction)
{
	switch (surfaceFriction.status()) {
		case metaf::SurfaceFriction::Status::NOT_REPORTED:
		return("not reported");

		case metaf::SurfaceFriction::Status::SURFACE_FRICTION_REPORTED:
		if (const auto c = surfaceFriction.coefficient(); c.has_value()) {
			return ("friction coefficient "s + roundTo(*c, 2));
		}
		return("[unable to produce a friction coefficient]");
		
		case metaf::SurfaceFriction::Status::BRAKING_ACTION_REPORTED:
		return ("braking action "s + 
			std::string(brakingActionToString(surfaceFriction.brakingAction())));

		case metaf::SurfaceFriction::Status::UNRELIABLE:
		return("unreliable or unmeasurable");

		default:
		return("[unknown surface friction status]");
	}
}

std::string GroupVisitorExplain::explainWaveHeight(const metaf::WaveHeight & waveHeight) {
	switch (waveHeight.type()) {
		case metaf::WaveHeight::Type::STATE_OF_SURFACE:
		return("state of sea surface: "s + 
			std::string(stateOfSeaSurfaceToString(waveHeight.stateOfSurface())));

		case metaf::WaveHeight::Type::WAVE_HEIGHT:
		if (waveHeight.isReported()) {
			std::ostringstream result;
			result << "wave height: ";
			if (const auto h = waveHeight.toUnit(metaf::WaveHeight::Unit::METERS); h.has_value()) {
				result << roundTo(*h, 1) << " meters";
			} else {
				result << "[unable to convert wave height to meters]";
			}
			result << " / ";
			if (const auto h = waveHeight.toUnit(metaf::WaveHeight::Unit::FEET); h.has_value()) {
				result << roundTo(*h, 1) << " feet";
			} else {
				result << "[unable to convert wave height to feet]";
			}
			return(result.str());
		}
		return("wave height not reported");

		default:
		return("[unknown wave height status]");
	}
}

std::string_view GroupVisitorExplain::speedUnitToString(metaf::Speed::Unit unit) {
	switch (unit) {	
		case metaf::Speed::Unit::KNOTS: 				return("knots");
		case metaf::Speed::Unit::METERS_PER_SECOND:		return("m/s");
		case metaf::Speed::Unit::KILOMETERS_PER_HOUR:	return("km/h");
		case metaf::Speed::Unit::MILES_PER_HOUR:		return("mph");
		default: 										return("[unknown speed unit]");
	}
}

std::string_view GroupVisitorExplain::distanceUnitToString(metaf::Distance::Unit unit) {
	switch (unit) {
		case metaf::Distance::Unit::METERS:			return("meters");
		case metaf::Distance::Unit::STATUTE_MILES:	return("statute miles");
		case metaf::Distance::Unit::FEET:			return("feet");
		default: 									return("[unknown distance unit]");
	}
}

std::string_view GroupVisitorExplain::cardinalDirectionToString(metaf::Direction::Cardinal cardinal) {
	switch(cardinal) {
		case metaf::Direction::Cardinal::NONE:		return(std::string());
		case metaf::Direction::Cardinal::N:			return("north");
		case metaf::Direction::Cardinal::S: 		return("south");
		case metaf::Direction::Cardinal::W: 		return("west");
		case metaf::Direction::Cardinal::E: 		return("east");
		case metaf::Direction::Cardinal::NW: 		return("northwest");
		case metaf::Direction::Cardinal::NE: 		return("northeast");
		case metaf::Direction::Cardinal::SW: 		return("southwest");
		case metaf::Direction::Cardinal::SE: 		return("southeast");
		case metaf::Direction::Cardinal::TRUE_N: 	return("true north");
		case metaf::Direction::Cardinal::TRUE_W: 	return("true west");
		case metaf::Direction::Cardinal::TRUE_S: 	return("true south");
		case metaf::Direction::Cardinal::TRUE_E: 	return("true east");
		default: 									return("[unknown cardinal direction]");
	}
}

std::string_view GroupVisitorExplain::brakingActionToString(
		metaf::SurfaceFriction::BrakingAction brakingAction)
{
	switch(brakingAction) {
		case metaf::SurfaceFriction::BrakingAction::NONE:
		return("not reported");

		case metaf::SurfaceFriction::BrakingAction::POOR:
		return("poor (friction coefficient 0.0 to 0.25)");

		case metaf::SurfaceFriction::BrakingAction::MEDIUM_POOR:
		return("medium/poor (friction coefficient 0.26 to 0.29)");

		case metaf::SurfaceFriction::BrakingAction::MEDIUM:
		return("medium (friction coefficient 0.30 to 0.35)");

		case metaf::SurfaceFriction::BrakingAction::MEDIUM_GOOD:
		return("medium/good (friction coefficient 0.36 to 0.40)");

		case metaf::SurfaceFriction::BrakingAction::GOOD:
		return("good (friction coefficient 0.40 to 1.00)");

		default:
		return("unknown braking action");
	}
}

std::string_view GroupVisitorExplain::stateOfSeaSurfaceToString(
	metaf::WaveHeight::StateOfSurface stateOfSurface)
{
	switch(stateOfSurface) {
		case metaf::WaveHeight::StateOfSurface::NOT_REPORTED:
		return("not reported");

		case metaf::WaveHeight::StateOfSurface::CALM_GLASSY:
		return("calm (glassy), no waves");

		case metaf::WaveHeight::StateOfSurface::CALM_RIPPLED:
		return("calm (rippled), wave height &lt;0.1 meters / &lt;1/3 feet");

		case metaf::WaveHeight::StateOfSurface::SMOOTH:
		return("smooth, wave height 0.1 to 0.5 meters / 1/3 to 1 1/2 feet");

		case metaf::WaveHeight::StateOfSurface::SLIGHT:
		return("slight, wave height 0.5 to 1.25 meters / 1 1/2 to 4 feet");

		case metaf::WaveHeight::StateOfSurface::MODERATE:
		return("moderate, wave height 1.25 to 2.5 meters / 4 to 8 feet");

		case metaf::WaveHeight::StateOfSurface::ROUGH:
		return("rough, wave height 2.5 to 4 meters / 8 to 13 feet");

		case metaf::WaveHeight::StateOfSurface::VERY_ROUGH:
		return("very rough, wave height 4 to 6 meters / 13 to 20 feet");

		case metaf::WaveHeight::StateOfSurface::HIGH:
		return("high, wave height 6 to 9 meters / 20 to 30 feet");

		case metaf::WaveHeight::StateOfSurface::VERY_HIGH:
		return("very high, wave height 9 to 14 meters / 30 to 46 feet");

		case metaf::WaveHeight::StateOfSurface::PHENOMENAL:
		return("phenomenal, wave height >14 meters / &gt;46 feet");

		default:
		return("unknown state of sea surface");
	}
}

std::string_view GroupVisitorExplain::trendTypeToString(metaf::TrendGroup::Type type) {
	switch (type) {
		case metaf::TrendGroup::Type::NONE:
		return("This group contains incomplete information");

		case metaf::TrendGroup::Type::NOSIG:
		return("No significant weather changes expected");

		case metaf::TrendGroup::Type::BECMG:
		return("Weather conditions are expected to gradually change as follows");

		case metaf::TrendGroup::Type::TEMPO:
		return("The following temporary weather conditions may arise for less than 60 minutes");

		case metaf::TrendGroup::Type::INTER:
		return("The following temporary weather conditions may arise for less than 30 minutes");

		case metaf::TrendGroup::Type::FROM:
		return("All previous weather conditions are superseded by the following conditions");

		case metaf::TrendGroup::Type::TIME_SPAN:
		return("The following weather condition are expected within time span");

		default:
		return("Unknown trend type");
	}
}

std::string_view GroupVisitorExplain::cloudAmountToString(metaf::CloudGroup::Amount amount) {
	switch (amount) {
		case metaf::CloudGroup::Amount::NOT_REPORTED:
		return("Cloud amount not reported");

		case metaf::CloudGroup::Amount::NSC:
		return("No significant cloud: "
			"no cloud below 5000 feet (1500 meters), no cumulonimbus or towering "
			"cumulus clouds, no vertical visibility restriction");

		case metaf::CloudGroup::Amount::NCD:
		return("No cloud detected: automated weather station did not detect any clouds; "
			"this can happen due to either no clouds present or sensor error");

		case metaf::CloudGroup::Amount::NONE_CLR:
		return("Clear sky: "
			"no cloud layers are detected at or below 12000 feet (3700 meters) (US) "
			"or 25000 feet (7600 meters) (Canada); "
			"indicates that station is at least partly automated");

		case metaf::CloudGroup::Amount::NONE_SKC:
		return("Clear sky: "
			"In North America indicates report producted by human rather than "
			"automatic weather station");

		case metaf::CloudGroup::Amount::FEW:
		return("Few clouds (1/8 to 2/8 sky covered)");

		case metaf::CloudGroup::Amount::SCATTERED:
		return("Scattered clouds (3/8 to 4/8 sky covered)");

		case metaf::CloudGroup::Amount::BROKEN:
		return("Broken clouds (5/8 to 7/8 sky covered)");

		case metaf::CloudGroup::Amount::OVERCAST:
		return("Overcast (8/8 sky covered)");

		case metaf::CloudGroup::Amount::OBSCURED:
		return("Sky obscured");

		default: 
		return("Unknown cloud amound");
	}
}

std::string_view GroupVisitorExplain::cloudTypeToString(metaf::CloudGroup::Type type) {
	switch (type) {
		case metaf::CloudGroup::Type::NONE:
		return(std::string_view());

		case metaf::CloudGroup::Type::NOT_REPORTED:
		return("not reported");

		case metaf::CloudGroup::Type::TOWERING_CUMULUS:
		return("towering cumulus");

		case metaf::CloudGroup::Type::CUMULONIMBUS:
		return("cumulonimbus");

		default: 
		return("undefined");
	}
}

std::string_view GroupVisitorExplain::weatherQualifierToString(
	metaf::WeatherGroup::Qualifier qualifier)
{
	switch (qualifier) {
		case metaf::WeatherGroup::Qualifier::NONE:	  	return(std::string_view());
		case metaf::WeatherGroup::Qualifier::RECENT: 	return(std::string_view());
		case metaf::WeatherGroup::Qualifier::VICINITY: 	return(std::string_view());
		case metaf::WeatherGroup::Qualifier::LIGHT: 	return("light");
		case metaf::WeatherGroup::Qualifier::MODERATE:	return("moderate");
		case metaf::WeatherGroup::Qualifier::HEAVY:		return("heavy");
		default: return("[unknown intensity/proximity qualifier];");
	}
}

std::string_view GroupVisitorExplain::weatherDescriptorToString(
	metaf::WeatherGroup::Descriptor descriptor)
{
	switch(descriptor) {
		case metaf::WeatherGroup::Descriptor::NONE:			return(std::string());
		case metaf::WeatherGroup::Descriptor::SHALLOW:		return("shallow");
		case metaf::WeatherGroup::Descriptor::PARTIAL:		return("partial");
		case metaf::WeatherGroup::Descriptor::PATCHES:		return("patches of ");
		case metaf::WeatherGroup::Descriptor::LOW_DRIFTING: return("low drifting");
		case metaf::WeatherGroup::Descriptor::BLOWING:		return("blowing");
		case metaf::WeatherGroup::Descriptor::SHOWERS:		return(std::string());
		case metaf::WeatherGroup::Descriptor::THUNDERSTORM:	return(std::string());
		case metaf::WeatherGroup::Descriptor::FREEZING:		return("freezing");
		default: return("[uknown weather descriptor];");
	}
}

std::string_view GroupVisitorExplain::weatherPhenomenaToString(
	metaf::WeatherGroup::Weather weather)
{
	switch (weather) {
			case metaf::WeatherGroup::Weather::NOT_REPORTED:	return("not reported");
			case metaf::WeatherGroup::Weather::DRIZZLE:			return("drizzle");
			case metaf::WeatherGroup::Weather::RAIN:			return("rain");
			case metaf::WeatherGroup::Weather::SNOW:			return("snow");
			case metaf::WeatherGroup::Weather::SNOW_GRAINS:		return("snow grains");
			case metaf::WeatherGroup::Weather::ICE_CRYSTALS:	return("ice crystals");
			case metaf::WeatherGroup::Weather::ICE_PELLETS:		return("ice pellets");
			case metaf::WeatherGroup::Weather::HAIL:			return("hail");
			case metaf::WeatherGroup::Weather::SMALL_HAIL:		return("small hail (graupel)");
			case metaf::WeatherGroup::Weather::UNDETERMINED:	return("undetermined precipitation");
			case metaf::WeatherGroup::Weather::MIST:			return("mist");
			case metaf::WeatherGroup::Weather::FOG:				return("fog");
			case metaf::WeatherGroup::Weather::SMOKE:			return("smoke");
			case metaf::WeatherGroup::Weather::VOLCANIC_ASH:	return("volcanic ash");
			case metaf::WeatherGroup::Weather::DUST:			return("dust");
			case metaf::WeatherGroup::Weather::SAND:			return("sand");
			case metaf::WeatherGroup::Weather::HAZE:			return("haze");
			case metaf::WeatherGroup::Weather::SPRAY:			return("spray");
			case metaf::WeatherGroup::Weather::DUST_WHIRLS:		return("dust or sand whirls");
			case metaf::WeatherGroup::Weather::SQUALLS:			return("squalls");
			case metaf::WeatherGroup::Weather::FUNNEL_CLOUD:	return("funnel cloud");
			case metaf::WeatherGroup::Weather::SANDSTORM:		return("sand storm");
			case metaf::WeatherGroup::Weather::DUSTSTORM:		return("dust storm");
			default: return("[unknown weather phenomena]");
	}

}

std::string_view GroupVisitorExplain::specialWeatherPhenomenaToString(
	const metaf::WeatherGroup & group)
{
	using WeatherVector = std::vector<metaf::WeatherGroup::Weather>;
	using SpecialWeatherPhenomena = std::tuple<	
		metaf::WeatherGroup::Qualifier, 
		metaf::WeatherGroup::Descriptor, 
		WeatherVector,
		std::string_view
		>;

	static const std::vector <SpecialWeatherPhenomena> 
		specialWeatherPhenomena = {
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::VICINITY,
				metaf::WeatherGroup::Descriptor::SHOWERS,
				{},
				"precipitation in vicinity (up to 10 miles away)"),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::NONE,
				{ metaf::WeatherGroup::Weather::ICE_CRYSTALS },
				"ice crystals (diamond dust)"),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::NONE,
				{ metaf::WeatherGroup::Weather::DUST },
				"widespread dust"),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::NONE,
				{ metaf::WeatherGroup::Weather::UNDETERMINED },
				"undetermined precipitation (automated system cannot identify precipitation)"
			),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::SHALLOW,
				{ metaf::WeatherGroup::Weather::FOG },
				"shallow fog (ground fog)"
			),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::PARTIAL,
				{ metaf::WeatherGroup::Weather::FOG },
				"fog covering part of the location"
			),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::PATCHES,
				{ metaf::WeatherGroup::Weather::FOG },
				"patches of fog randomly covering the location"
			),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::NONE,
				metaf::WeatherGroup::Descriptor::FREEZING,
				{ metaf::WeatherGroup::Weather::FOG },
				"fog at freezing temperture (may or may not deposit rime)"
			),
			SpecialWeatherPhenomena(
				metaf::WeatherGroup::Qualifier::HEAVY,
				metaf::WeatherGroup::Descriptor::NONE,
				{ metaf::WeatherGroup::Weather::FUNNEL_CLOUD },
				"tornado or waterspout"
			)
	};

	for (const auto w : specialWeatherPhenomena) {
		if (group.qualifier() == std::get<metaf::WeatherGroup::Qualifier>(w) &&
			group.descriptor() == std::get<metaf::WeatherGroup::Descriptor>(w) &&
			group.weather() == std::get<WeatherVector>(w))
		{
				return(std::get<std::string_view>(w));
		}
	}
	return(std::string());
}

std::string_view GroupVisitorExplain::rvrTrendToString(metaf::RunwayVisualRangeGroup::Trend trend) {
	switch(trend) {
		case metaf::RunwayVisualRangeGroup::Trend::NONE:		return(std::string_view());
		case metaf::RunwayVisualRangeGroup::Trend::NOT_REPORTED:return("not reported");
		case metaf::RunwayVisualRangeGroup::Trend::UPWARD:		return("upward");
		case metaf::RunwayVisualRangeGroup::Trend::NEUTRAL:		return("neutral");
		case metaf::RunwayVisualRangeGroup::Trend::DOWNWARD:	return("downward");
		default: 												return("[unknown trend]");
	}
}

std::string_view GroupVisitorExplain::runwayStateDepositsToString(
	metaf::RunwayStateGroup::Deposits deposits)
{
	switch(deposits) {
		case metaf::RunwayStateGroup::Deposits::NOT_REPORTED:
		return("not reported");

		case metaf::RunwayStateGroup::Deposits::CLEAR_AND_DRY:
		return("clear and dry");

		case metaf::RunwayStateGroup::Deposits::DAMP:
		return("damp");

		case metaf::RunwayStateGroup::Deposits::WET_AND_WATER_PATCHES:
		return("wet and water patches");

		case metaf::RunwayStateGroup::Deposits::RIME_AND_FROST_COVERED:
		return("rime and frost covered");

		case metaf::RunwayStateGroup::Deposits::DRY_SNOW:
		return("dry snow");

		case metaf::RunwayStateGroup::Deposits::WET_SNOW:
		return("wet snow");

		case metaf::RunwayStateGroup::Deposits::SLUSH:
		return("slush");

		case metaf::RunwayStateGroup::Deposits::ICE:
		return("ice");

		case metaf::RunwayStateGroup::Deposits::COMPACTED_OR_ROLLED_SNOW:
		return("compacted or rolled snow");

		case metaf::RunwayStateGroup::Deposits::FROZEN_RUTS_OR_RIDGES:
		return("frozen ruts or ridges");

		default:
		return("[unknown runway deposits]");
	}
}

std::string_view GroupVisitorExplain::runwayStateExtentToString(
	metaf::RunwayStateGroup::Extent extent)
{
	switch(extent) {
		case metaf::RunwayStateGroup::Extent::NOT_REPORTED:
		return("not reported");

		case metaf::RunwayStateGroup::Extent::NONE:
		return("none");

		case metaf::RunwayStateGroup::Extent::LESS_THAN_10_PERCENT:
		return("&lt;10 percent");

		case metaf::RunwayStateGroup::Extent::FROM_11_TO_25_PERCENT:
		return("11 to 25 percent");

		case metaf::RunwayStateGroup::Extent::FROM_26_TO_50_PERCENT:
		return("26 to 50 percent");

		case metaf::RunwayStateGroup::Extent::MORE_THAN_51_PERCENT:
		return("&gt;51 percent");

		case metaf::RunwayStateGroup::Extent::RESERVED_3:
		return("[reserved_extent_value 3]");

		case metaf::RunwayStateGroup::Extent::RESERVED_4:
		return("[reserved_extent_value 4]");

		case metaf::RunwayStateGroup::Extent::RESERVED_6:
		return("[reserved_extent_value 6]");

		case metaf::RunwayStateGroup::Extent::RESERVED_7:
		return("[reserved_extent_value 7]");

		case metaf::RunwayStateGroup::Extent::RESERVED_8:
		return("[reserved_extent_value 8]");

		default:
		return("[unknown extent value]");
	}
}

std::string_view GroupVisitorExplain::colourCodeToString(metaf::ColourCodeGroup::Code code) {
	switch(code) {
		case metaf::ColourCodeGroup::Code::BLUE:		return("BLUE");
		case metaf::ColourCodeGroup::Code::WHITE:		return("WHITE");
		case metaf::ColourCodeGroup::Code::GREEN:		return("GREEN");
		case metaf::ColourCodeGroup::Code::YELLOW1:		return("YELLOW1");
		case metaf::ColourCodeGroup::Code::YELLOW2:		return("YELLOW2");
		case metaf::ColourCodeGroup::Code::AMBER:		return("AMBER");
		case metaf::ColourCodeGroup::Code::RED:			return("RED");
		default:										return("[unknown colour code]");
	}
}

unsigned int GroupVisitorExplain::colourCodeVisibility(metaf::ColourCodeGroup::Code code) {
	switch(code) {
		case metaf::ColourCodeGroup::Code::BLUE:	return(8000);
		case metaf::ColourCodeGroup::Code::WHITE:	return(5000);
		case metaf::ColourCodeGroup::Code::GREEN:	return(3700);
		case metaf::ColourCodeGroup::Code::YELLOW1: return(2500);
		case metaf::ColourCodeGroup::Code::YELLOW2:	return(1600);
		case metaf::ColourCodeGroup::Code::AMBER:	return(800);
		case metaf::ColourCodeGroup::Code::RED:		return(800);
		default:									return(0);
	}
}

unsigned int GroupVisitorExplain::colourCodeCeiling(metaf::ColourCodeGroup::Code code) {
	switch(code) {
		case metaf::ColourCodeGroup::Code::BLUE:	return(2500);
		case metaf::ColourCodeGroup::Code::WHITE:	return(1500);
		case metaf::ColourCodeGroup::Code::GREEN:	return(700);
		case metaf::ColourCodeGroup::Code::YELLOW1:	return(500);
		case metaf::ColourCodeGroup::Code::YELLOW2:	return(300);
		case metaf::ColourCodeGroup::Code::AMBER:	return(200);
		case metaf::ColourCodeGroup::Code::RED:		return(200);
		default:									return(0);
	}
}

std::string GroupVisitorExplain::roundTo(float number, size_t digitsAfterDecimalPoint) {
	static const char decimalPoint = '.';
	std::string numStr = std::to_string(number);
	const auto decimalPointPos = numStr.find(decimalPoint);
	if (decimalPointPos == std::string::npos) return(numStr);
	const auto decimalsAfterPoint = numStr.length() - decimalPointPos;
	if (decimalsAfterPoint > digitsAfterDecimalPoint) {
		return(numStr.substr(0, decimalPointPos + digitsAfterDecimalPoint + 1));
	}
	return(numStr);
}

///////////////////////////////////////////////////////////////////////////////

static std::string result;

void addResult(const std::string & group, const std::string & explanation) {
	static const char delimiter = '|';
	result += group;
	result.push_back(delimiter);
	result += explanation;
	result.push_back(delimiter);
}

extern "C" const char * EMSCRIPTEN_KEEPALIVE explain(const char * input) {
	metaf::Parser parser;
	parser.parse(std::string(input), true);
	addResult("", "Detected report type: "s + 
		std::string(GroupVisitorExplain::reportTypeToString(parser.getReportType())));
	if (parser.getError() != metaf::Parser::Error::NONE) {
		addResult("", "Parsing error: "s + 
			std::string(GroupVisitorExplain::reportErrorToString(parser.getError())));
	}
	for (auto i=0u; i<parser.getResult().size(); i++) {
		std::string source = std::string("");
		if (i < parser.getSourceGroups().size()) {
			source = parser.getSourceGroups().at(i);
		}
		GroupVisitorExplain visitor;
		addResult(source, visitor.visit(parser.getResult().at(i)));
	}
	//remove last delimiter which is always added at the end 
	//of the last explanation string
	result.pop_back(); 
	return(result.c_str());
}

extern "C" void EMSCRIPTEN_KEEPALIVE freeMemory(){
	std::string().swap(result);
}

int main(int argc, char ** argv) {
	(void) argc; (void) argv;
	//Using EM_ASM_ because EM_ASM(explain()); gives a warning
	EM_ASM_(explain(), 0); 
}
