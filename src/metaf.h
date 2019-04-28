/*
* Copyright (C) 2018-2019 Nick Naumenko (https://gitlab.com/nnaumenko)
* All rights reserved.
* This software may be modified and distributed under the terms
* of the MIT license. See the LICENSE file for details.
*/

/// @file 
/// @brief METAR / TAF report parser, helpers and structs representing 
/// parsed METAR or TAF groups.

#ifndef METAF_H
#define METAF_H

#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <regex>
#include <cctype>
#include <cmath>

namespace metaf {

	// Metaf library version
	struct Version {
		static const int major = 2;
		static const int minor = 1;
		static const int patch = 0;
		inline static const char tag [] = "";
	};

	class PlainTextGroup;
	class FixedGroup;
	class LocationGroup;
	class ReportTimeGroup;
	class TrendGroup;
	class WindGroup;
	class VisibilityGroup;
	class CloudGroup;
	class WeatherGroup;
	class TemperatureGroup;
	class TemperatureForecastGroup;
	class PressureGroup;
	class RunwayVisualRangeGroup;
	class RunwayStateGroup;
	class WindShearLowLayerGroup;
	class RainfallGroup;
	class SeaSurfaceGroup;
	class ColourCodeGroup;

	// A variant type for all possible METAR and TAF groups.
	using Group = std::variant<
		PlainTextGroup,
		FixedGroup,
		LocationGroup,
		ReportTimeGroup,
		TrendGroup,
		WindGroup,
		VisibilityGroup,
		CloudGroup,
		WeatherGroup,
		TemperatureGroup,
		TemperatureForecastGroup,
		PressureGroup,
		RunwayVisualRangeGroup,
		RunwayStateGroup,
		WindShearLowLayerGroup,
		RainfallGroup,
		SeaSurfaceGroup,
		ColourCodeGroup
	>;

	enum class ReportPart {
		UNKNOWN,
		HEADER,
		METAR,
		TAF,
		RMK
	};

	inline std::optional<unsigned int> strToUint(const std::string & str,
		std::size_t startPos,
		std::size_t digits);

	///////////////////////////////////////////////////////////////////////////

	class Runway {
	public:
		enum class Designator {
			NONE,
			LEFT,
			CENTER,
			RIGHT,
		};
		unsigned int number() const { return(rNumber); }
		Designator designator() const { return(rDesignator); }
		bool isValid() const {
			return (rNumber <= maxRunwayNumber || 
				(rNumber == allRunwaysNumber && rDesignator == Designator::NONE) || 
				(rNumber == messageRepetitionNumber && rDesignator == Designator::NONE));
		}
		bool isAllRunways() const {
			return(rNumber == allRunwaysNumber && rDesignator == Designator::NONE);
		}
		bool isMessageRepetition() const {
			return(rNumber == messageRepetitionNumber && rDesignator == Designator::NONE);
		}

		Runway() = default;
		static inline std::optional<Runway> fromString(const std::string & s, bool enableRwy = false);
		static Runway makeAllRunways() {
			Runway rw;
			rw.rNumber = allRunwaysNumber;
			return(rw);
		}

	private:
		static inline std::optional<Designator> designatorFromChar(char c);

		unsigned int rNumber = 0;
		Designator rDesignator = Designator::NONE;
		static const unsigned int allRunwaysNumber = 88;
		static const unsigned int messageRepetitionNumber = 99;	
		static const auto maxRunwayNumber = 360 / 10;
	};

	class MetafTime {
	public:
		std::optional<unsigned int> day() const { return(dayValue); }
		unsigned int hour() const { return(hourValue); }
		unsigned int minute() const { return(minuteValue); }
		inline bool isValid() const;

		MetafTime() = default;
		static inline std::optional<MetafTime> fromStringDDHHMM(const std::string & s);
		static inline std::optional<MetafTime> fromStringDDHH(const std::string & s);

	private:
		std::optional<unsigned int> dayValue;
		unsigned int hourValue = 0;
		unsigned int minuteValue = 0;

		static const inline unsigned int dayNotReported = 0;
		static const inline unsigned int maxDay = 31;
		static const inline unsigned int maxHour = 24;
		static const inline unsigned int maxMinute = 59;
	};

	class Temperature {
	public:
		enum class Unit {
			C,
			F,
		};
		std::optional<int> temperature() const { return(tempValue); }
		Unit unit() const { return(tempUnit); }
		std::optional<float> inline toUnit(Unit unit) const;
		bool isFreezing() const { return(freezing); }

		Temperature () = default;
		static inline std::optional<Temperature> fromString(const std::string & s);
	private:
		std::optional<int> tempValue;
		bool freezing = false;
		static const Unit tempUnit = Unit::C;
	};

	class Speed {
	public:
		enum class Unit {
			KNOTS,
			METERS_PER_SECOND,
			KILOMETERS_PER_HOUR,
			MILES_PER_HOUR
		};
		std::optional<unsigned int> speed() const { return(speedValue); }
		Unit unit() const { return(speedUnit); }
		std::optional<float> inline toUnit(Unit unit) const;

		Speed() = default;
		static inline std::optional<Speed> fromString(const std::string & s, Unit unit);
		static inline std::optional<Unit> unitFromString(const std::string & s);

	private:
		std::optional<unsigned int> speedValue;
		Unit speedUnit = Unit::KNOTS;

		static inline std::optional<float> knotsToUnit(float valueKnots, Unit otherUnit);
		static inline std::optional<float> mpsToUnit(float valueMps, Unit otherUnit);
		static inline std::optional<float> kmhToUnit(float valueKmh, Unit otherUnit);
		static inline std::optional<float> mphToUnit(float valueMph, Unit otherUnit);
	};

	class Distance {
	public:
		enum class Unit {
			METERS,
			STATUTE_MILES,
			FEET
		};
		enum class Modifier {
			NONE,
			LESS_THAN,
			MORE_THAN,
		};
		inline std::optional<unsigned int> integer() const { return(distValueInt); }
		inline std::optional<unsigned int> numerator() const { return(distValueNum); }
		inline std::optional<unsigned int> denominator() const { return(distValueDen); }
		Modifier modifier() const { return(distModifier); }
		Unit unit() const { return(distUnit); }
		bool isInteger() const {
			return(integer().has_value() && 
				!numerator().has_value() && !denominator().has_value());
		}
		bool isFraction() const {
			return(!integer().has_value() && 
				numerator().has_value() && denominator().has_value());
		}
		bool isReported() const {
			return(integer().has_value() || 
					(numerator().has_value() && denominator().has_value()));
		}
		bool hasInteger() const { return(integer().has_value()); }
		bool hasFraction() const { return(numerator().has_value() && denominator().has_value()); }
		inline std::optional<float> toUnit(Unit unit) const;
		bool isValid() const {
			if (distValueDen.has_value() && !distValueDen.value()) return(false);
			if (distValueNum.has_value() && !distValueNum.value()) return(false);
			return(true);
		}

		Distance() = default;
		Distance(int d, Unit u) : distValueInt(d), distUnit(u) {} //Init distance without fraction
		Distance(Unit u) : distUnit(u) {} // Init non-reported distance
		static inline std::optional<Distance> fromIntegerAndFraction(const Distance & integer,
			const Distance & fraction);
		static inline std::optional<Distance> fromMeterString(const std::string & s);
		static inline std::optional<Distance> fromMileString(const std::string & s);
		static inline std::optional<Distance> fromHeightString(const std::string & s);
		static inline std::optional<Distance> fromRvrString(const std::string & s, bool unitFeet);

	private:
		Modifier distModifier = Modifier::NONE;
		std::optional<unsigned int> distValueInt;
		std::optional<unsigned int> distValueNum;
		std::optional<unsigned int> distValueDen;
		Unit distUnit = Unit::METERS;

		static const unsigned int heightFactor = 100; //height unit is 100s of feet

		static inline std::optional<Modifier> modifierFromChar(char c);
		static inline std::optional<float> metersToUnit(float value, Unit unit);
		static inline std::optional<float> milesToUnit(float value, Unit unit);
		static inline std::optional<float> feetToUnit(float value, Unit unit);
	};

	class Direction {
	public:
		enum class Cardinal {
			NONE,	// Not reported or no corresponding cardinal direction
			NDV,	// No Directional Variation
			N,
			S,
			W,
			E,
			NW,
			NE,
			SW,
			SE,
			TRUE_N,
			TRUE_W,
			TRUE_S,
			TRUE_E,
		};
		enum class Status {
			OMMITTED, 		  // Direction is ommitted (not specified at all)
			NOT_REPORTED,	  // Direction is specified as not reported
			VARIABLE,		  // Direction is reported as variable
			NDV,			  // Direction is reported as No Directional Variation
			VALUE_DEGREES, 	  // Direction is reported as value in degrees
			VALUE_CARDINAL,	  // Direction is reported as cardinal value
		};
		Status status() const { return (dirStatus); }
		inline Cardinal cardinal(bool trueDirections = false) const;
		std::optional<unsigned int> degrees() const {
			if (!isValue())	return(std::optional<unsigned int>());
			return(dirDegrees);
		}
		bool isValue() const {
			return(dirStatus == Status::VALUE_DEGREES || dirStatus == Status::VALUE_CARDINAL); 
		}
		bool isValid() const {
			if (isValue() && dirDegrees > maxDegrees) return(false);
			return(true);
		}

		Direction() = default;
		static inline std::optional<Direction> fromCardinalString(const std::string & s);
		static inline std::optional<Direction> fromDegreesString(const std::string & s);

	private:
		unsigned int dirDegrees = 0;
		Status dirStatus = Status::OMMITTED;
	private:
		static const inline unsigned int maxDegrees = 360;
		static const inline unsigned int degreesTrueNorth = 360;
		static const inline unsigned int degreesTrueWest = 270;
		static const inline unsigned int degreesTrueSouth = 180;
		static const inline unsigned int degreesTrueEast = 90;
		static const inline unsigned int degreesNorthWest = 315;
		static const inline unsigned int degreesNorthEast = 45;
		static const inline unsigned int degreesSouthWest = 225;
		static const inline unsigned int degreesSouthEast = 135;
	};

	class Pressure {
	public:
		enum class Unit {
			HECTOPASCAL,
			INCHES_HG,
			MM_HG
		};
		std::optional<float> pressure() const { return(pressureValue); }
		Unit unit() const { return(pressureUnit); }
		inline std::optional<float> toUnit(Unit unit) const;

		Pressure() = default;
		static inline std::optional<Pressure> fromString(const std::string & s);
		static inline std::optional<Pressure> fromForecastString(const std::string & s);
		static inline std::optional<Pressure> fromSlpString(const std::string & s);
		static inline std::optional<Pressure> fromQfeString(const std::string & s);

	private:
		std::optional<float> pressureValue;
		Unit pressureUnit = Unit::HECTOPASCAL;

		static inline const float inHgDecimalPointShift = 0.01;
	};

	class Precipitation {
	public:
		enum class Status {
			NOT_REPORTED,
			REPORTED,
			RUNWAY_NOT_OPERATIONAL,
		};
		enum class Unit {
			MM,
			INCHES,
		};
		std::optional<float> precipitation() const { 
			if (precipStatus != Status::REPORTED) return(std::optional<float>());
			return(precipValue);
		}
		Unit unit() const { return(precipUnit); }
		inline std::optional<float> toUnit(Unit unit) const;
		Status status() const { return (precipStatus); }

		Precipitation() = default;
		static inline std::optional<Precipitation> fromRainfallString(const std::string & s);
		static inline std::optional<Precipitation> fromRunwayDeposits(const std::string & s);

	private:
		Status precipStatus = Status::NOT_REPORTED;
		float precipValue = 0.0;
		static const Unit precipUnit = Unit::MM;
	private:
		// Special value for runway deposits depth, see Table 1079 in Manual on Codes (WMO No. 306).
		enum Reserved {
			RESERVED = 91,
			DEPTH_10CM = 92,
			DEPTH_15CM = 93,
			DEPTH_20CM = 94,
			DEPTH_25CM = 95,
			DEPTH_30CM = 96,
			DEPTH_35CM = 97,
			DEPTH_40CM = 98,
			RUNWAY_NOT_OPERATIONAL = 99
		};
	};

	class SurfaceFriction {
	public:
		enum class Status {
			NOT_REPORTED,
			SURFACE_FRICTION_REPORTED,
			BRAKING_ACTION_REPORTED,
			UNRELIABLE 		// Value unreliable or unmeasurable.
		};
		enum class BrakingAction {
			NONE,			// Not reported or unreliable			
			POOR,	 		// Friction coef <0.26
			MEDIUM_POOR,	// Friction coef 0.26 to 0.29
			MEDIUM,			// Friction coef 0.30 to 0.35
			MEDIUM_GOOD,	// Friction coef 0.36 to 0.39
			GOOD,			// Friction coef >0.39
		};
		Status status() const { return (sfStatus); }
		std::optional<float> coefficient() const {
			if (sfStatus == Status::NOT_REPORTED || 
				sfStatus == Status::UNRELIABLE) return(std::optional<float>());
			return(sfCoefficient * coefficientDecimalPointShift);
		}
		inline BrakingAction brakingAction() const;

		SurfaceFriction() = default;
		static inline std::optional<SurfaceFriction> fromString(const std::string & s);

	private:
		Status sfStatus = Status::NOT_REPORTED;	
		unsigned int sfCoefficient = 0; //0 to 100, multiply by 0.01 to get actual value
		static const inline auto coefficientDecimalPointShift = 0.01;
	private:
		// Special values for braking action, see Table 0366 in Manual on Codes (WMO No. 306).
		enum Reserved {
			BRAKING_ACTION_POOR = 91,
			BRAKING_ACTION_MEDIUM_POOR = 92,
			BRAKING_ACTION_MEDIUM = 93,
			BRAKING_ACTION_MEDIUM_GOOD = 94,
			BRAKING_ACTION_GOOD = 95,
			RESERVED_96 = 96,
			RESERVED_97 = 97,
			RESERVED_98 = 98,
			UNRELIABLE = 99,
		};
		static const inline auto baPoorLowLimit = 0u;
		static const inline auto baMediumPoorLowLimit = 26u;
		static const inline auto baMediumLowLimit = 30u;
		static const inline auto baMediumGoodLowLimit = 36u;
		static const inline auto baGoodLowLimit = 40u;
	};

	class WaveHeight {
	public:
		enum class Type {
			STATE_OF_SURFACE,	// Descriptive state of surface is specified
			WAVE_HEIGHT,		// Actual wave height is specified
		};
		enum class Unit {
			METERS,
			FEET,
		};
		// State of sea surface, see Table 3700 in Manual on Codes (WMO No. 306).
		enum class StateOfSurface {
			NOT_REPORTED,
			CALM_GLASSY,
			CALM_RIPPLED,
			SMOOTH,
			SLIGHT,
			MODERATE,
			ROUGH,
			VERY_ROUGH,
			HIGH,
			VERY_HIGH,
			PHENOMENAL,
		};
		Type type() const { return (whType); }
		inline StateOfSurface stateOfSurface() const;
		Unit unit() const { return(whUnit); }
		std::optional<float> waveHeight() const {
			if (!whValue.has_value()) return(std::optional<float>());
			return(whValue.value() * waveHeightDecimalPointShift);
		}
		bool isReported() const { return(whValue.has_value()); }
		inline std::optional<float> toUnit(Unit unit) const;

		WaveHeight() = default;
		static inline std::optional<WaveHeight> fromString(const std::string & s);

	private:
		Type whType = Type::STATE_OF_SURFACE;
		std::optional<unsigned int> whValue; //in decimeters, muliply by 0.1 to get value in meters
		static const inline auto waveHeightDecimalPointShift = 0.1;
		static const Unit whUnit = Unit::METERS;
	private:
		static inline std::optional<unsigned int> waveHeightFromStateOfSurfaceChar(char c);
	private:
		//Values below are in decimeters, muliply by 0.1 to get value in meters
		static const inline auto maxWaveHeightCalmGlassy = 0;
		static const inline auto maxWaveHeightCalmRippled = 1;
		static const inline auto maxWaveHeightSmooth = 5;
		static const inline auto maxWaveHeightSlight = 12;
		static const inline auto maxWaveHeightModerate = 25;
		static const inline auto maxWaveHeightRough = 40;
		static const inline auto maxWaveHeightVeryRough = 60;
		static const inline auto maxWaveHeightHigh = 90;
		static const inline auto maxWaveHeightVeryHigh = 140;
		static const inline auto minWaveHeightPhenomenal = 141;
	};

	///////////////////////////////////////////////////////////////////////////

	class PlainTextGroup {
	public:
		std::string toString() const { return(std::string(text)); }
		bool isValid() const { return(text[0]); }

		PlainTextGroup() = default;
		PlainTextGroup(const std::string & s) {
			strncpy(text, s.c_str(), textMaxLength); text[textMaxLength] = '\0';
		}
		static std::optional<PlainTextGroup> parse(const std::string & group, ReportPart reportPart) {
			(void)reportPart; return(PlainTextGroup(group));
		}
		inline std::optional<Group> combine(const Group & nextGroup) const;

		static const size_t textMaxLength = 15;

	private:
		char text [textMaxLength + 1] = "\0";
	};

	class FixedGroup {
	public:
		enum class Type {
			METAR,
			SPECI,
			TAF,
			AMD,
			NIL,
			CNL,
			COR,
			AUTO,
			R_SNOCLO,
			CAVOK,
			NSW,
			RMK,
			WSCONDS,
			MAINTENANCE_INDICATOR,
			AO1,
			AO2,
			NOSPECI,
			PRESFR,
			PRESRR,
			RVRNO,
			PWINO,
			PNO,
			FZRANO,
			TSNO,
			SLPNO
		};
		Type type() const { return(t); }
		bool isValid() const { return(true); }

		FixedGroup() = default;
		static inline std::optional<FixedGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		FixedGroup(Type type) :t (type) {}
		Type t; 
	};

	class LocationGroup {
	public:
		std::string toString() const {return(std::string(location));}
		inline bool isValid() const { return(true); }

		LocationGroup() = default;
		static inline std::optional<LocationGroup> parse(const std::string & group, 
			ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		static const inline auto locationLength = 4; 
		char location [locationLength + 1] = "\0"; 
	};

	class ReportTimeGroup {
	public:
		MetafTime time() const { return(t); };
		bool isValid() const { return(t.isValid() && t.day().has_value()); }

		ReportTimeGroup() = default;
		static inline std::optional<ReportTimeGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		MetafTime t;	
	};

	class TrendGroup {
	public:
		enum class Type {
			NONE,		// Incomplete groups or their combination.
			NOSIG,
			BECMG,
			TEMPO,
			INTER,
			FROM,
			TIME_SPAN,
		};
		enum class Probability {
			NONE,		// Probability not specified.
			PROB_30,
			PROB_40,
		};
		Type type() const { return(t); }
		Probability probability() const { return(prob); }
		std::optional<MetafTime> timeFrom() const { return(tFrom); }
		std::optional<MetafTime> timeTill() const { return(tTill); }
		std::optional<MetafTime> timeAt() const { return(tAt); }
		bool isValid() const {
			if (tFrom.has_value() && !tFrom->isValid()) return(false);
			if (tTill.has_value() && !tTill->isValid()) return(false);
			if (tAt.has_value() && !tAt->isValid()) return(false);
			return (type() != Type::NONE); // Incomplete groups are considered invalid
		}
		inline bool isTimeSpanGroup() const;

		TrendGroup() = default;
		static inline std::optional<TrendGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		TrendGroup(Type type) : t(type) {}
		TrendGroup(Probability p) : t(Type::NONE), prob(p) {}

		static inline std::optional<TrendGroup> fromTimeSpan(const std::string & s);
		static inline std::optional<TrendGroup> fromFm(const std::string & s);
		static inline std::optional<TrendGroup> fromTrendTime(const std::string & s);

		inline bool combineProbAndTrendTypeGroups(const TrendGroup & nextTrendGroup);
		inline bool combineTrendTypeAndTimeGroup(const TrendGroup & nextTrendGroup);
		inline bool combineProbAndTimeSpanGroups(const TrendGroup & nextTrendGroup);
		inline bool combineIncompleteGroups(const TrendGroup & nextTrendGroup);

		static inline bool canCombineTime(const TrendGroup & g1, const TrendGroup & g2);
		inline void combineTime(const TrendGroup & nextTrendGroup);

		inline bool isProbabilityGroup() const;
		inline bool isTrendTypeGroup() const;
		inline bool isTrendTimeGroup() const;

		Type t = Type::NONE;
		Probability prob = Probability::NONE;
		std::optional<MetafTime> tFrom; // Time span beginning.
		std::optional<MetafTime> tTill;	// Time span end time.
		std::optional<MetafTime> tAt;	// Precise time.
	};

	class WindGroup {
	public:
		Direction direction() const { return(windDir); }
		Speed windSpeed() const { return(wSpeed); }
		Speed gustSpeed() const { return(gSpeed); }
		inline bool isCalm() const;
		Distance windShearHeight() const { return(wShHeight); }
		Direction varSectorBegin() const { return(vsecBegin); }
		Direction varSectorEnd() const { return(vsecEnd); }
		bool isWindShear() const { return(wShHeight.isReported()); }
		bool isSurfaceWind() const { return(!isWindShear()); }
		bool hasVariableSector() const { 
			return(isSurfaceWind() &&
				vsecBegin.status() == Direction::Status::VALUE_DEGREES && 
				vsecEnd.status() == Direction::Status::VALUE_DEGREES);
		}
		inline bool isValid() const;

		WindGroup() = default;
		static inline std::optional<WindGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;
	private:
		Direction windDir;
		Speed wSpeed;
		Speed gSpeed;
		Distance wShHeight;
		Direction vsecBegin;
		Direction vsecEnd;

		inline bool isSurfaceWindGroup() const;
		inline bool isWindSectorGroup() const;
	};

	class VisibilityGroup {
	public:
		Distance visibility() const { return(vis); }
		Direction direction() const { return(dir); }
		bool isPrevailing() const { 
			return(direction().status() == Direction::Status::OMMITTED ||
				direction().status() == Direction::Status::NDV); 
		}
		bool isDirectional() const { return(!isPrevailing()); }
		bool isValid() const { 
			return(!incompleteInteger && vis.isValid() && dir.isValid());
		}

		VisibilityGroup() = default;
		inline static std::optional<VisibilityGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;
	private:
		Distance vis;
		Direction dir;
		bool incompleteInteger = false; 
	};

	class CloudGroup {
	public:
		enum class Amount {
			NOT_REPORTED,
			NCD,
			NSC,
			NONE_CLR,
			NONE_SKC,
			FEW,
			SCATTERED,
			BROKEN,
			OVERCAST,
			OBSCURED
		};
		enum class Type {
			NOT_REPORTED,
			NONE,
			TOWERING_CUMULUS,
			CUMULONIMBUS
		};
		Amount amount() const { return(amnt); }
		Type type() const { return(tp); }
		inline Distance height() const;
		Distance verticalVisibility() const {
			if (amount() != Amount::OBSCURED) return(heightNotReported);
			return(heightOrVertVis);
		}
		bool isVerticalVisibility() const { return(amount() == Amount::OBSCURED); }
		bool isNoClouds() const {
			return(amount() == Amount::NONE_CLR || amount() == Amount::NONE_SKC ||
				amount() == Amount::NCD || amount() == Amount::NSC);
		}
		bool isCloudLayer() const {
			return(amount() == Amount::FEW || amount() == Amount::SCATTERED ||
				amount() == Amount::BROKEN || amount() == Amount::OVERCAST);		
		}
		bool isValid() const { return(heightOrVertVis.isValid()); }

		CloudGroup () = default;
		static inline std::optional<CloudGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;
	private:
		Amount amnt = Amount::NOT_REPORTED;
		Distance heightOrVertVis;
		Type tp = Type::NONE;
		static const inline auto heightNotReported = Distance(Distance::Unit::FEET);

		CloudGroup(Amount a) : amnt(a) {}
		static inline std::optional<Amount> amountFromString(const std::string & s);
		static inline std::optional<Type> typeFromString(const std::string & s);
	};

	class WeatherGroup {
	public:
		enum class Qualifier {
			NONE,	
			RECENT,
			VICINITY,	
			LIGHT,		
			MODERATE,	
			HEAVY		
		};
		enum class Descriptor {
			NONE,
			SHALLOW,
			PARTIAL,
			PATCHES,
			LOW_DRIFTING,
			BLOWING,
			SHOWERS,
			THUNDERSTORM,
			FREEZING
		};
		enum class Weather {
			NOT_REPORTED,
			DRIZZLE,
			RAIN,
			SNOW,
			SNOW_GRAINS,
			ICE_CRYSTALS,
			ICE_PELLETS,
			HAIL,
			SMALL_HAIL,
			UNDETERMINED,
			MIST,
			FOG,
			SMOKE,
			VOLCANIC_ASH,
			DUST,
			SAND,
			HAZE,
			SPRAY,
			DUST_WHIRLS,
			SQUALLS,
			FUNNEL_CLOUD,
			SANDSTORM,
			DUSTSTORM
		};

		Qualifier qualifier() const { return(q); }
		Descriptor descriptor() const { return(d); }
		std::vector<Weather> weather() const {
			std::vector<Weather> result;
			for (auto i = 0u; i < wSize; i++) result.push_back(w[i]);
			return(result);
		}
		bool isValid() const { return(true); } //TODO

		WeatherGroup() = default;
		static inline std::optional<WeatherGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Qualifier q = Qualifier::NONE;
		Descriptor d = Descriptor::NONE;
		inline static const size_t maxwSize = 8;
		Weather w[maxwSize];
		size_t wSize = 0;

		inline bool isModerateQualifier() const;

		static inline std::optional<Qualifier> qualifierFromString(const std::string & s);
		static inline std::optional<Descriptor> descriptorFromString(const std::string & s);
		static inline std::optional<Weather> weatherFromString(const std::string & s);

		static inline WeatherGroup notReported();
		static inline WeatherGroup notReportedRecent();
	};

	class TemperatureGroup {
	public:
		Temperature airTemperature() const { return(t); }
		Temperature dewPoint() const { return(dp); }
		inline std::optional<float> relativeHumidity() const;
		inline bool isValid() const;

		TemperatureGroup() = default;
		static inline std::optional<TemperatureGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Temperature t;
		Temperature dp;
	};

	class TemperatureForecastGroup {
	public:
		enum class Point {
			MINIMUM,
			MAXIMUM
		};
		Point point() const { return(p); }
		Temperature airTemperature() const { return(t); }
		MetafTime time() const { return(tm); }
		bool isValid() const { return(tm.isValid()); }

		TemperatureForecastGroup() = default;
		static inline std::optional<TemperatureForecastGroup> parse(const std::string & group, 
			ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Point p = Point::MINIMUM;
		Temperature t;
		MetafTime tm;

		static std::optional<Point> pointFromString (const std::string & s) {
			if (s == "TX") return(Point::MAXIMUM);
			if (s == "TN") return(Point::MINIMUM);
			return(std::optional<Point>());
		}
	};

	class PressureGroup {
	public:
		enum class Type {
			OBSERVED_QNH,			//Observed mean sea level pressure (METAR)
			FORECAST_LOWEST_QNH,	//Forecast lowest sea level pressure
			OBSERVED_QFE			//Observed actual (non-normalised) pressure
		};
		Type type() const { return(t); }
		Pressure atmosphericPressure() const { return(p); }
		bool isValid() const { return(true); }

		PressureGroup() = default;
		static inline std::optional<PressureGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Type t = Type::OBSERVED_QNH;
		Pressure p;
	};

	class RunwayVisualRangeGroup {
	public:
		enum class Trend {
			NONE,
			NOT_REPORTED,
			UPWARD,
			NEUTRAL,
			DOWNWARD
		};
		Runway runway() const { return(rw); }
		Distance visualRange() const { 
			if (!isVariableVisualRange()) return(visRange); 
			return(Distance());
		}
		Distance minVisualRange() const {
			if (isVariableVisualRange()) return(visRange);
			return(Distance());
		}
		Distance maxVisualRange() const { return(varVisRange); }
		Trend trend() const { return(trnd); }
		bool isVariableVisualRange() const { return(varVisRange.isReported()); }
		bool isValid() const { 
			return(rw.isValid() && visRange.isValid() && varVisRange.isValid());
		}

		RunwayVisualRangeGroup() = default;
		static inline std::optional<RunwayVisualRangeGroup> parse(const std::string & group, 
			ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Runway rw;
		Distance visRange;
		Distance varVisRange;
		Trend trnd;
	private:
		static inline std::optional<Trend> trendFromString(const std::string & s);
	};

	class RunwayStateGroup {
	public:
		enum class Status {
			NORMAL,
			CLRD,
			SNOCLO
		};	
		enum class Deposits {// Deposits type, see Table 0919 in Manual on Codes (WMO No. 306).
			CLEAR_AND_DRY,
			DAMP,
			WET_AND_WATER_PATCHES,
			RIME_AND_FROST_COVERED,
			DRY_SNOW,
			WET_SNOW,
			SLUSH,
			ICE,
			COMPACTED_OR_ROLLED_SNOW,
			FROZEN_RUTS_OR_RIDGES,
			NOT_REPORTED
		};
		
		enum class Extent {// Extent of runway contamination, see Table 0519 in Manual on Codes (WMO No. 306).
			NONE,
			LESS_THAN_10_PERCENT,
			FROM_11_TO_25_PERCENT,
			RESERVED_3,
			RESERVED_4,
			FROM_26_TO_50_PERCENT,
			RESERVED_6,
			RESERVED_7,
			RESERVED_8,
			MORE_THAN_51_PERCENT,
			NOT_REPORTED
		};
		Runway runway() const { return(rw); }
		Status status() const { return(st); }
		Deposits deposits() const { return(dp); }
		Extent contaminationExtent() const { return(ext); }
		Precipitation depositDepth() const { return(dDepth); }
		SurfaceFriction surfaceFriction() const { return(sf); }
		bool isValid() const { 
			return(rw.isValid() && 
				ext != Extent::RESERVED_3 && ext != Extent::RESERVED_4 && 
				ext != Extent::RESERVED_6 && ext != Extent::RESERVED_7 && ext != Extent::RESERVED_8);
		}

		RunwayStateGroup() = default;
		static inline std::optional<RunwayStateGroup> parse(const std::string & group, 
			ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Runway rw;
		Status st = Status::NORMAL;
		Deposits dp = Deposits::NOT_REPORTED;
		Extent ext = Extent::NOT_REPORTED;
		Precipitation dDepth;
		SurfaceFriction sf;

		static inline RunwayStateGroup runwaySnoclo(Runway runway);
		static inline RunwayStateGroup runwayClrd(Runway runway, 
			SurfaceFriction surfaceFriction);

		static inline std::optional<Deposits> depositsFromString(const std::string & s);
		static inline std::optional<Extent> extentFromString(const std::string & s);
	};

	class WindShearLowLayerGroup {
	public:
		Runway runway() const { return(rw); }
		bool isValid() const { return(rw.isValid() && status == Status::COMPLETE); }

		WindShearLowLayerGroup() = default; 
		static inline std::optional<WindShearLowLayerGroup> parse(
			const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;
	private:
		enum class Status {
			COMPLETE,
			INCOMPLETE_WS,
			INCOMPLETE_WS_ALL,
		};
		Runway rw;
		Status status = Status::INCOMPLETE_WS;
	};

	class RainfallGroup {
	public:
		Precipitation rainfallLast10Minutes() const { return(last10m); }
		Precipitation rainfallLast60Minutes() const { return(last60m); }
		Precipitation rainfallSince9AM() const { return(since0900); }
		bool isValid() const { return(true); }

		RainfallGroup() = default;
		static inline std::optional<RainfallGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Precipitation last10m;
		Precipitation since0900;
		Precipitation last60m;
	};

	class SeaSurfaceGroup {
	public:
		Temperature surfaceTemperature() const { return(t); }
		WaveHeight waves() const { return(wh); }
		bool isValid() const { return(true); }

		SeaSurfaceGroup() = default;
		static inline std::optional<SeaSurfaceGroup> parse(const std::string & group, ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;

	private:
		Temperature t;
		WaveHeight wh;
	};

	class ColourCodeGroup {
	public:
		enum class Code {
			BLUE ,	// Visibility >8000 m AND no cloud obscuring 3/8 or more below 2500 feet.	 
			WHITE ,	// Visibility >5000 m AND no cloud obscuring 3/8 or more below 1500 feet.			
			GREEN,	// Visibility >3700 m AND no cloud obscuring 3/8 or more below 700 feet.
			YELLOW1,// Visibility >2500 m AND no cloud obscuring 3/8 or more below 500 feet.
			YELLOW2,// Visibility >1600 m AND no cloud obscuring 3/8 or more below 300 feet.
			AMBER,	// Visibility >800 m AND no cloud obscuring 3/8 or more below 200 feet.
			RED		// Visibility <800 m OR clouds obscuring 3/8 or more below 200 feet.
		};
		Code code() const { return(c); }
		bool isCodeBlack() const { return(cBlack); }
		bool isValid() const { return(true); }

		ColourCodeGroup() = default;
		static inline std::optional<ColourCodeGroup> parse(const std::string & group, 
			ReportPart reportPart);
		inline std::optional<Group> combine(const Group & nextGroup) const;
	private:
		Code c = Code::BLUE;
		bool cBlack = false; //Is colour code BLACK reported along with main code
	};

	///////////////////////////////////////////////////////////////////////////////
	
	enum class SyntaxGroup {
		OTHER,
		METAR,
		SPECI,
		TAF,
		COR,
		AMD,
		LOCATION,
		REPORT_TIME,
		TIME_SPAN,
		CNL,
		NIL,
		RMK,
		MAINTENANCE_INDICATOR
	};

	inline SyntaxGroup getSyntaxGroup(const Group & group);

	template <class GroupVariant, class FallbackAlternative>
	class GenericGroupParser {
	public:
		using GroupType = GroupVariant;
		static GroupVariant parse(const std::string & group, ReportPart reportPart) {
			return(parseAlternative<0>(group, reportPart));
		}
	private:
		template <size_t I>
		static GroupVariant parseAlternative(const std::string & group, 
			ReportPart reportPart)
		{
			using Alternative = std::variant_alternative_t<I, GroupVariant>;
			if constexpr (!std::is_same<Alternative, FallbackAlternative>::value) {
				const auto parsed = Alternative::parse(group, reportPart);
				if (parsed.has_value()) return(parsed.value());
			}
			if constexpr (I < std::variant_size_v<GroupVariant> - 1) {
				return(parseAlternative<I+1>(group, reportPart));
			} 
			return(FallbackAlternative(group));
		} 
	};

	enum class ReportType {
		UNKNOWN,
		METAR,
		TAF
	};

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	class GenericParser {
	public:
		using GroupType = typename GenericGroupParser::GroupType;
		enum class Error {
			NONE,
			EMPTY_REPORT,
			EXPECTED_REPORT_TYPE_OR_LOCATION,
			EXPECTED_LOCATION,
			EXPECTED_REPORT_TIME,
			EXPECTED_TIME_SPAN,
			UNEXPECTED_REPORT_END,
			UNEXPECTED_GROUP_AFTER_NIL,
			UNEXPECTED_GROUP_AFTER_CNL,
			UNEXPECTED_GROUP_AFTER_MAINTENANCE_INDICATOR,
			UNEXPECTED_NIL_OR_CNL_IN_REPORT_BODY,
			AMD_ALLOWED_IN_TAF_ONLY,
			CNL_ALLOWED_IN_TAF_ONLY,
			MAINTENANCE_INDICATOR_ALLOWED_IN_METAR_ONLY,
			INTERNAL_PARSER_STATE
		};
		struct Result {
			ReportType reportType;
			Error error;
			std::vector<GroupType> groups;
		};

		struct ExtendedResult {
			ReportType reportType;
			Error error;
			std::vector< std::tuple<GroupType, ReportPart, std::string> > extgroups;
		};

		static Result parse (const std::string & report) {
			const auto result = parseInternal(report, false);
			return(std::get<Result>(result));
		}
		static ExtendedResult extendedParse(const std::string & report) {
			const auto result = parseInternal(report, true);
			return(std::get<ExtendedResult>(result));
		}
	private:
		using ParseInternalResult = std::variant<Result, ExtendedResult>;
		static inline ParseInternalResult parseInternal(
			const std::string & report, 
			bool extendedReport);
		static inline std::optional<GroupType> combineWithLastGroup(
			const std::optional<GroupType> & lastGroup,
			const GroupType & group);
		static inline std::optional<GroupType> getLastGroup(const Result & result);
		static inline std::optional<GroupType> getLastGroup(const ExtendedResult & result);

		static inline void saveToResult(Result & result, 
			GroupType group,
			const std::optional<GroupType> & combinedGroup);

		static inline void saveToResult(ExtendedResult & extresult, 
			GroupType group,
			const std::optional<GroupType> & combinedGroup, 
			ReportPart reportPart,
			const std::string & groupString);

		static inline void saveToResult(
			Result & result, 
			ReportType reportType, 
			Error error);

		static inline void saveToResult(
			ExtendedResult & extresult, 
			ReportType reportType, 
			Error error);

		class Status {
		public:
			Status() : 
				state(State::REPORT_TYPE_OR_LOCATION), 
				reportType(ReportType::UNKNOWN),
				error(Error::NONE) {}
			ReportType getReportType() { return(reportType); }
			Error getError() { return(error); }
			bool isError() { return(error != Error::NONE); }
			inline ReportPart getReportPart();
			inline void transition(SyntaxGroup group);
			inline void finalTransition();
			bool isReparseRequired() { 
				return(state == State::REPORT_BODY_BEGIN_METAR_REPEAT_PARSE);
			}
		private:
			enum class State {
				// States of state machine used to check syntax of METAR/TAF reports
				REPORT_TYPE_OR_LOCATION,
				CORRECTION,
				LOCATION,
				REPORT_TIME,
				TIME_SPAN,
				REPORT_BODY_BEGIN_METAR,
				REPORT_BODY_BEGIN_METAR_REPEAT_PARSE,
				REPORT_BODY_METAR,
				REPORT_BODY_BEGIN_TAF,
				REPORT_BODY_TAF,
				REMARK_METAR,
				REMARK_TAF,
				MAINTENANCE_INDICATOR,
				NIL,
				CNL,
				ERROR
			};

			void setState(State s) { state = s; }
			void setError(Error e) { state = State::ERROR; error = e; }
			void setReportType(ReportType rt) { reportType = rt; }

			inline void transitionFromReportTypeOrLocation(SyntaxGroup group);
			inline void transitionFromCorrecton(SyntaxGroup group);
			inline void transitionFromReportTime(SyntaxGroup group);
			inline void transitionFromTimeSpan(SyntaxGroup group);
			inline void transitionFromReportBodyBeginMetar(SyntaxGroup group);
			inline void transitionFromReportBodyMetar(SyntaxGroup group);
			inline void transitionFromReportBodyBeginTaf(SyntaxGroup group);
			inline void transitionFromReportBodyTaf(SyntaxGroup group);
			State state;
			ReportType reportType;
			Error error;
		};
	};

	using GroupParser = GenericGroupParser<Group, PlainTextGroup>;
	using Parser = GenericParser<GroupParser, getSyntaxGroup>;

	///////////////////////////////////////////////////////////////////////////////

	template <typename T>
	class GroupVisitor {
	public:
		inline T visit(const Group & group);
	protected:
		virtual T visitPlainTextGroup(const PlainTextGroup & group) = 0;
		virtual T visitFixedGroup(const FixedGroup & group) = 0;
		virtual T visitLocationGroup(const LocationGroup & group) = 0;
		virtual T visitReportTimeGroup(const ReportTimeGroup & group) = 0;
		virtual T visitTrendGroup(const TrendGroup & group) = 0;
		virtual T visitWindGroup(const WindGroup & group) = 0;
		virtual T visitVisibilityGroup(const VisibilityGroup & group) = 0;
		virtual T visitCloudGroup(const CloudGroup & group) = 0;
		virtual T visitWeatherGroup(const WeatherGroup & group) = 0;
		virtual T visitTemperatureGroup(const TemperatureGroup & group) = 0;
		virtual T visitTemperatureForecastGroup(const TemperatureForecastGroup & group) = 0;
		virtual T visitPressureGroup(const PressureGroup & group) = 0;
		virtual T visitRunwayVisualRangeGroup(const RunwayVisualRangeGroup & group) = 0;
		virtual T visitRunwayStateGroup(const RunwayStateGroup & group) = 0;
		virtual T visitWindShearLowLayerGroup(const WindShearLowLayerGroup & group) = 0;
		virtual T visitRainfallGroup(const RainfallGroup & group) = 0;
		virtual T visitSeaSurfaceGroup(const SeaSurfaceGroup & group) = 0;
		virtual T visitColourCodeGroup(const ColourCodeGroup & group) = 0;
		virtual T visitOther(const Group & group) = 0;
	};

	template <typename T>
	inline T GroupVisitor<T>::visit(const Group & group) {
		if (std::holds_alternative<PlainTextGroup>(group)) {
			return(this->visitPlainTextGroup(std::get<PlainTextGroup>(group)));
		}
		if (std::holds_alternative<FixedGroup>(group)) {
			return(this->visitFixedGroup(std::get<FixedGroup>(group)));
		}
		if (std::holds_alternative<LocationGroup>(group)) {
			return(this->visitLocationGroup(std::get<LocationGroup>(group)));
		}
		if (std::holds_alternative<ReportTimeGroup>(group)) {
			return(this->visitReportTimeGroup(std::get<ReportTimeGroup>(group)));
		}
		if (std::holds_alternative<TrendGroup>(group)) {
			return(this->visitTrendGroup(std::get<TrendGroup>(group)));
		}
		if (std::holds_alternative<WindGroup>(group)) {
			return(this->visitWindGroup(std::get<WindGroup>(group)));
		}
		if (std::holds_alternative<VisibilityGroup>(group)) {
			return(this->visitVisibilityGroup(std::get<VisibilityGroup>(group)));
		}
		if (std::holds_alternative<CloudGroup>(group)) {
			return(this->visitCloudGroup(std::get<CloudGroup>(group)));
		}
		if (std::holds_alternative<WeatherGroup>(group)) {
			return(this->visitWeatherGroup(std::get<WeatherGroup>(group)));
		}
		if (std::holds_alternative<TemperatureGroup>(group)) {
			return(this->visitTemperatureGroup(std::get<TemperatureGroup>(group)));
		}
		if (std::holds_alternative<TemperatureForecastGroup>(group)) {
			return(this->visitTemperatureForecastGroup(std::get<TemperatureForecastGroup>(group)));
		}
		if (std::holds_alternative<PressureGroup>(group)) {
			return(this->visitPressureGroup(std::get<PressureGroup>(group)));
		}
		if (std::holds_alternative<RunwayVisualRangeGroup>(group)) {
			return(this->visitRunwayVisualRangeGroup(std::get<RunwayVisualRangeGroup>(group)));
		}
		if (std::holds_alternative<RunwayStateGroup>(group)) {
			return(this->visitRunwayStateGroup(std::get<RunwayStateGroup>(group)));
		}
		if (std::holds_alternative<WindShearLowLayerGroup>(group)) {
			return(this->visitWindShearLowLayerGroup(std::get<WindShearLowLayerGroup>(group)));
		}
		if (std::holds_alternative<RainfallGroup>(group)) {
			return(this->visitRainfallGroup(std::get<RainfallGroup>(group)));
		}
		if (std::holds_alternative<SeaSurfaceGroup>(group)) {
			return(this->visitSeaSurfaceGroup(std::get<SeaSurfaceGroup>(group)));
		}
		if (std::holds_alternative<ColourCodeGroup>(group)) {
			return(this->visitColourCodeGroup(std::get<ColourCodeGroup>(group)));
		}
		return(this->visitOther(group));
	}

	template<>
	inline void GroupVisitor<void>::visit(const Group & group) {
		if (std::holds_alternative<PlainTextGroup>(group)) {
			this->visitPlainTextGroup(std::get<PlainTextGroup>(group));
			return;
		}
		if (std::holds_alternative<FixedGroup>(group)) {
			this->visitFixedGroup(std::get<FixedGroup>(group));
			return;
		}
		if (std::holds_alternative<LocationGroup>(group)) {
			this->visitLocationGroup(std::get<LocationGroup>(group));
			return;
		}
		if (std::holds_alternative<ReportTimeGroup>(group)) {
			this->visitReportTimeGroup(std::get<ReportTimeGroup>(group));
			return;
		}
		if (std::holds_alternative<TrendGroup>(group)) {
			this->visitTrendGroup(std::get<TrendGroup>(group));
			return;
		}
		if (std::holds_alternative<WindGroup>(group)) {
			this->visitWindGroup(std::get<WindGroup>(group));
			return;
		}
		if (std::holds_alternative<VisibilityGroup>(group)) {
			this->visitVisibilityGroup(std::get<VisibilityGroup>(group));
			return;
		}
		if (std::holds_alternative<CloudGroup>(group)) {
			this->visitCloudGroup(std::get<CloudGroup>(group));
			return;
		}
		if (std::holds_alternative<WeatherGroup>(group)) {
			this->visitWeatherGroup(std::get<WeatherGroup>(group));
			return;
		}
		if (std::holds_alternative<TemperatureGroup>(group)) {
			this->visitTemperatureGroup(std::get<TemperatureGroup>(group));
			return;
		}
		if (std::holds_alternative<TemperatureForecastGroup>(group)) {
			this->visitTemperatureForecastGroup(std::get<TemperatureForecastGroup>(group));
			return;
		}
		if (std::holds_alternative<PressureGroup>(group)) {
			this->visitPressureGroup(std::get<PressureGroup>(group));
			return;
		}
		if (std::holds_alternative<RunwayVisualRangeGroup>(group)) {
			this->visitRunwayVisualRangeGroup(std::get<RunwayVisualRangeGroup>(group));
			return;
		}
		if (std::holds_alternative<RunwayStateGroup>(group)) {
			this->visitRunwayStateGroup(std::get<RunwayStateGroup>(group));
			return;
		}
		if (std::holds_alternative<WindShearLowLayerGroup>(group)) {
			this->visitWindShearLowLayerGroup(std::get<WindShearLowLayerGroup>(group));
			return;
		}
		if (std::holds_alternative<RainfallGroup>(group)) {
			this->visitRainfallGroup(std::get<RainfallGroup>(group));
			return;
		}
		if (std::holds_alternative<SeaSurfaceGroup>(group)) {
			this->visitSeaSurfaceGroup(std::get<SeaSurfaceGroup>(group));
			return;
		}
		if (std::holds_alternative<ColourCodeGroup>(group)) {
			this->visitColourCodeGroup(std::get<ColourCodeGroup>(group));
			return;
		}
		this->visitOther(group);
	}

} //namespace metaf

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

namespace metaf {

	std::optional<unsigned int> strToUint(const std::string & str, 
		std::size_t startPos,
		std::size_t digits)
	{
		std::optional<unsigned int> error;
		if (str.empty() || !digits || startPos + digits > str.length()) return(error);
		unsigned int value = 0;
		for (auto [i,c] = std::pair(0u, str.c_str() + startPos); i < digits; i++, c++) {
			if (*c < '0' || *c > '9') return(error);
			static const auto decimalRadix = 10u;
			value = value * decimalRadix + (*c - '0');
		}
		return(value);
	}


	std::optional<Runway> Runway::fromString(const std::string & s, bool enableRwy) {
		//static const std::regex rgx("R(?:WY)?(\\d\\d)([RLC])?");
		static const std::optional<Runway> error;
		if (s.length() < 3) return(error);
		if (s[0] != 'R') return(error);
		auto numPos = 1u;
		if (enableRwy && s[1] == 'W' && s[2] == 'Y') numPos += 2;
		const auto rwyNum = strToUint(s, numPos, 2);
		if (!rwyNum.has_value()) return(error);
		const auto dsgPos = numPos + 2;
		if (s.length() > dsgPos + 1) return(error);
		Runway runway;
		runway.rNumber = rwyNum.value();
		if ( s.length() > dsgPos) {
			const auto designator = designatorFromChar(s[dsgPos]);
			if (!designator.has_value()) return(error);
			runway.rDesignator = designator.value();
		} 
		return(runway);
	}

	std::optional<Runway::Designator> Runway::designatorFromChar(char c) {
		switch (c) {
			case 'R': return(Designator::RIGHT);
			case 'C': return(Designator::CENTER);
			case 'L': return(Designator::LEFT);
			default: return(std::optional<Designator>());
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<MetafTime> MetafTime::fromStringDDHHMM(const std::string & s) {
		//static const std::regex rgx ("(\\d\\d)?(\\d\\d)(\\d\\d)");
		static const std::optional<MetafTime> error;
		if (s.length() == 4) {
			const auto hour = strToUint(s, 0, 2);
			const auto minute = strToUint(s, 2, 2);
			if (!hour.has_value() || !minute.has_value()) return(error);
			MetafTime metafTime;
			metafTime.hourValue = hour.value();
			metafTime.minuteValue = minute.value();
			return(metafTime);
		}
		if (s.length() == 6) {
			const auto day = strToUint(s, 0, 2);
			const auto hour = strToUint(s, 2, 2);
			const auto minute = strToUint(s, 4, 2);
			if (!day.has_value() || !hour.has_value() || !minute.has_value()) return(error);
			MetafTime metafTime;
			metafTime.dayValue = day.value();
			metafTime.hourValue = hour.value();
			metafTime.minuteValue = minute.value();
			return(metafTime);
		}
		return(error);
	}

	std::optional<MetafTime> MetafTime::fromStringDDHH(const std::string & s) {
		//static const std::regex rgx ("(\\d\\d)(\\d\\d)");
		static const std::optional<MetafTime> error;
		if (s.length() != 4) return(error);
		const auto day = strToUint(s, 0, 2);
		const auto hour = strToUint(s, 2, 2);
		if (!day.has_value() || !hour.has_value()) return(error);
		MetafTime metafTime;
		metafTime.dayValue = day.value();
		metafTime.hourValue = hour.value();
		return(metafTime);
	}

	bool MetafTime::isValid() const {
		if (auto d = dayValue.value_or(maxDay); d > maxDay || !d) return(false);
		if (hourValue > maxHour) return(false);
		if (minuteValue > maxMinute) return(false);
		return(true);
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<Temperature> Temperature::fromString(const std::string & s) {
		//static const std::regex rgx ("(?:(M)?(\\d\\d))|//");
		std::optional<Temperature> error;
		if (s == "//") return(Temperature());
		if (s.length() == 3) {
			if (s[0] != 'M') return(error);
			const auto t = strToUint(s, 1, 2);
			if (!t.has_value()) return(error);
			Temperature temperature;
			temperature.tempValue = - t.value();
			temperature.freezing = true;
			return(temperature);
		}
		if (s.length() == 2) {
			const auto t = strToUint(s, 0, 2);
			if (!t.has_value()) return(error);
			Temperature temperature;
			temperature.tempValue = t.value();
			return(temperature);
		}
		return(error);
	}

	std::optional<float> Temperature::toUnit(Unit unit) const {
		std::optional<float> error;
		auto v = temperature();
		if (!v.has_value()) return(error);
		if (tempUnit == unit) return(v);
		if (tempUnit == Unit::C && unit == Unit::F) return(v.value() * 1.8 + 32);
		return(error);
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<Speed> Speed::fromString(const std::string & s, Unit unit) {
		//static const std::regex rgx ("([1-9]?\\d\\d)|//");
		static const std::optional<Speed> error;
		if (s.empty() || s == "//") return(Speed());
		if (s.length() != 2 && s.length() != 3) return(error);
		if (s.length() == 3 && s[0] == '0') return(error);
		const auto spd = strToUint(s, 0, s.length());
		if (!spd.has_value()) return(error);	
		Speed speed;
		speed.speedUnit = unit;
		speed.speedValue = spd.value();
		return(speed);
	}

	std::optional<float> Speed::toUnit(Unit unit) const {
		if (!speedValue.has_value()) return(std::optional<float>());
		switch (speedUnit) {
			case Unit::KNOTS:				return(knotsToUnit(speedValue.value(), unit));
			case Unit::METERS_PER_SECOND:	return(mpsToUnit(speedValue.value(), unit));
			case Unit::KILOMETERS_PER_HOUR:	return(kmhToUnit(speedValue.value(), unit));
			case Unit::MILES_PER_HOUR:		return(mphToUnit(speedValue.value(), unit));
			default:						return(std::optional<float>());
		}
	}

	std::optional<Speed::Unit> Speed::unitFromString(const std::string & s) {
		if (s == "KT") return(Speed::Unit::KNOTS);
		if (s == "MPS") return(Speed::Unit::METERS_PER_SECOND);
		if (s == "KMH") return(Speed::Unit::KILOMETERS_PER_HOUR);
		return(std::optional<Speed::Unit>());
	}

	std::optional<float> Speed::knotsToUnit(float valueKnots, Unit otherUnit) {
		switch(otherUnit) {
			case Unit::KNOTS: 				return(valueKnots);
			case Unit::METERS_PER_SECOND: 	return(valueKnots * 0.514444);
			case Unit::KILOMETERS_PER_HOUR:	return(valueKnots * 1.852);
			case Unit::MILES_PER_HOUR:		return(valueKnots * 1.150779);
			default:						return(std::optional<float>());
		}
	}

	std::optional<float> Speed::mpsToUnit(float valueMps, Unit otherUnit) {
		switch(otherUnit) {
			case Unit::KNOTS: 				return(valueMps * 1.943844);
			case Unit::METERS_PER_SECOND: 	return(valueMps);
			case Unit::KILOMETERS_PER_HOUR:	return(valueMps * 3.6);
			case Unit::MILES_PER_HOUR:		return(valueMps * 2.236936);
			default:						return(std::optional<float>());
		}
	}

	std::optional<float> Speed::kmhToUnit(float valueKmh, Unit otherUnit) {
		switch(otherUnit) {
			case Unit::KNOTS: 				return(valueKmh / 1.852);
			case Unit::METERS_PER_SECOND:	return(valueKmh / 3.6);
			case Unit::KILOMETERS_PER_HOUR:	return(valueKmh);
			case Unit::MILES_PER_HOUR:		return(valueKmh * 0.621371);
			default:						return(std::optional<float>());
		}
	}

	std::optional<float> Speed::mphToUnit(float valueMph, Unit otherUnit) {
		switch(otherUnit) {
			case Unit::KNOTS: 				return(valueMph * 0.868976);
			case Unit::METERS_PER_SECOND: 	return(valueMph * 0.44704);
			case Unit::KILOMETERS_PER_HOUR:	return(valueMph * 1.609344);
			case Unit::MILES_PER_HOUR:		return(valueMph);
			default:						return(std::optional<float>());
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	std::optional<Distance> Distance::fromMeterString(const std::string & s) {
		//static const std::regex rgx ("(\\d\\d\\d\\d)|////");
		static const std::optional<Distance> error;
		if (s.length() != 4) return(error);
		Distance distance;
		distance.distUnit = Unit::METERS;
		if (s == "////") return(distance);

		const auto dist = strToUint(s, 0, 4);
		if (!dist.has_value()) return(error);
		distance.distValueInt = dist.value();

		if (const auto valueMoreThan10km = 9999u; distance.distValueInt == valueMoreThan10km) {
			static const auto value10km = 10000u; 
			distance.distValueInt = value10km;
			distance.distModifier = Modifier::MORE_THAN;
		}
		return(distance);
	}

	std::optional<Distance> Distance::fromMileString(const std::string & s) {
		//static const std::regex rgx ("([PM])?(\\d?\\d)(?:/(\\d?\\d))?SM|////SM");
		static const std::optional<Distance> error;
		static const auto unitStr = std::string ("SM"); 
		static const auto unitLength = unitStr.length();
		if (s.length() < unitLength + 1) return(error); //1 digit minimum
		Distance distance;
		distance.distUnit = Unit::STATUTE_MILES;
		if (s == "////SM") return(distance);
		if (s.substr(s.length() - unitLength, unitLength) != unitStr) return(error); //s.endsWith(unitStr)
		const auto modifier = modifierFromChar(s[0]);
		if (const auto slashPos = s.find('/'); slashPos == std::string::npos) {
			//Integer value
			int intPos = 0, intLength = s.length() - unitLength;
			if (modifier.has_value()) {
				distance.distModifier = modifier.value();
				intPos++;
				intLength--;
			}
			if (intLength <= 0 || intLength > 2) return (error);
			const auto dist = strToUint(s, intPos, intLength);
			if (!dist.has_value()) return(error);
			distance.distValueInt = dist.value();
		} else {
			//Fraction value
			int numPos = 0, numLength = slashPos;
			if (modifier.has_value()) {
				distance.distModifier = modifier.value();
				numPos++;
				numLength--;
			}
			int denPos = slashPos + 1, denLength = s.length() - unitLength - denPos;
			if (numLength < 0 || numLength > 2 || denLength < 0 || denLength > 2 || denPos < 0) return(error);
			const auto distNum = strToUint(s, numPos, numLength);
			const auto distDen = strToUint(s, denPos, denLength);
			if (!distNum.has_value() || !distDen.has_value()) return(error);
			distance.distValueNum = distNum.value();
			distance.distValueDen = distDen.value();
			if (distNum.value() >= distDen.value()) {
				static const auto decimalRadix = 10u;
				distance.distValueInt = distNum.value() / decimalRadix;
				distance.distValueNum = distNum.value() % decimalRadix;
			}
		}
		return(distance);
	}

	std::optional<Distance> Distance::fromHeightString(const std::string & s) {
		//static const std::regex rgx ("(\\d\\d\\d)|///");
		static const std::optional<Distance> error;
		if (s.length() != 3) return(error);
		Distance distance;
		distance.distUnit = Unit::FEET;
		if (s == "///") return(distance);
		const auto h = strToUint(s, 0, 3);
		if (!h.has_value())	return(error);	
		distance.distValueInt = h.value() * heightFactor;
		return(distance);
	}

	std::optional<Distance> Distance::fromRvrString(const std::string & s, bool unitFeet)
	{
		//static const std::regex rgx ("([PM])?(\\d\\d\\d\\d)|////");
		static const std::optional<Distance> error;
		Distance distance;
		distance.distUnit = unitFeet ? Unit::FEET : Unit::METERS;
		if (s.length() == 4) {
			if (s == "////") return(distance);
			const auto dist = strToUint(s, 0, 4);
			if (!dist.has_value()) return(error);
			distance.distValueInt = dist.value();
			return(distance);
		}
		if (s.length() == 5) {
			auto modifier = modifierFromChar(s[0]);
			if (!modifier.has_value()) return(error);
			distance.distModifier = modifier.value();
			const auto dist = strToUint(s, 1, 4);
			if (!dist.has_value()) return(error);
			distance.distValueInt = dist.value();
			return(distance);
		}
		return(error);
	}

	std::optional<Distance> Distance::fromIntegerAndFraction(const Distance & integer, 
		const Distance & fraction)
	{
		if (!integer.isValid() || 
			!fraction.isValid() ||
			integer.modifier() != Modifier::NONE ||
			fraction.modifier() != Modifier::NONE ||
			integer.unit() != fraction.unit() ||
			!integer.isInteger() ||
			!fraction.isFraction()) return(std::optional<Distance>());
		Distance result = integer;
		result.distValueNum = fraction.distValueNum;
		result.distValueDen = fraction.distValueDen;
		return(result);
	}

	std::optional<float> Distance::toUnit(Unit unit) const {
		static const std::optional<float> error;
		if (!isReported()) return(error);
		if (!distValueDen.value_or(1)) return(error);
		const auto value = distValueInt.value_or(0) + 
			static_cast<float>(distValueNum.value_or(0)) / distValueDen.value_or(1);
		if (distUnit == unit) return(value);
		switch (distUnit) {
			case Unit::METERS: 			return(metersToUnit(value, unit));
			case Unit::STATUTE_MILES:	return(milesToUnit(value, unit));
			case Unit::FEET:			return(feetToUnit(value, unit));
			default:					return(error);
		}
	}

	std::optional<Distance::Modifier> Distance::modifierFromChar(char c) {
		if (c == 'M') return(Modifier::LESS_THAN);
		if (c == 'P') return(Modifier::MORE_THAN);
		return(std::optional<Modifier>());
	}

	std::optional<float> Distance::metersToUnit(float value, Unit unit) {
		switch(unit) {
			case Unit::METERS: 		return(value);
			case Unit::STATUTE_MILES:	return(value / 1609.347);
			case Unit::FEET:			return(value / 0.3048);
			default: return(std::optional<float>());
		}
	}

	std::optional<float> Distance::milesToUnit(float value, Unit unit) {
		switch(unit) {
			case Unit::METERS: 		return(value * 1609.347);
			case Unit::STATUTE_MILES:	return(value);
			case Unit::FEET:			return(value * 5280.0);
			default: return(std::optional<float>());
		}
	}

	std::optional<float> Distance::feetToUnit(float value, Unit unit) {
		switch(unit) {
			case Unit::METERS: 		return(value * 0.3048);
			case Unit::STATUTE_MILES:	return(value / 5280.0);
			case Unit::FEET:			return(value);
			default: return(std::optional<float>());
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	std::optional<Direction> Direction::fromCardinalString(const std::string & s) {
		if (s.empty()) {
			Direction dir;
			dir.dirStatus = Status::OMMITTED;
			return(dir);
		}
		if (s == "NDV") {
			Direction dir;
			dir.dirStatus = Status::NDV;
			return(dir);
		}
		int cardinalDegrees = 0;
		if (s == "N") cardinalDegrees = degreesTrueNorth;
		if (s == "W") cardinalDegrees = degreesTrueWest;
		if (s == "S") cardinalDegrees = degreesTrueSouth;
		if (s == "E") cardinalDegrees = degreesTrueEast;
		if (s == "NW") cardinalDegrees = degreesNorthWest;
		if (s == "NE") cardinalDegrees = degreesNorthEast;
		if (s == "SW") cardinalDegrees = degreesSouthWest;
		if (s == "SE") cardinalDegrees = degreesSouthEast;
		if (!cardinalDegrees) return(std::optional<Direction>());
		Direction dir;
		dir.dirStatus = Status::VALUE_CARDINAL;
		dir.dirDegrees = cardinalDegrees;
		return(dir);
	}

	std::optional<Direction> Direction::fromDegreesString(const std::string & s) {
		std::optional<Direction> error;
		Direction direction;
		if (s.empty()) {
			direction.dirStatus = Status::OMMITTED;
			return(direction);
		}
		if (s.length() != 3) return(error);
		if (s == "///") {
			direction.dirStatus = Status::NOT_REPORTED;
			return(direction);
		}
		if (s == "VRB") {
			direction.dirStatus = Status::VARIABLE;
			return(direction);
		}
		//static const std::regex rgx("\\d\\d0");
		if (s[2] != '0') return(error);
		const auto dir = strToUint(s, 0, 3);
		if (!dir.has_value()) return(error);
		direction.dirStatus = Status::VALUE_DEGREES;
		direction.dirDegrees = dir.value();
		return(direction);
	}

	Direction::Cardinal Direction::cardinal(bool trueDirections) const {
		if (status() == Status::OMMITTED || 
			status() == Status::NOT_REPORTED ||
			status() == Status::VARIABLE) return(metaf::Direction::Cardinal::NONE);
		if (status() == Status::NDV) return(metaf::Direction::Cardinal::NDV);
		if (trueDirections) {
			if (dirDegrees == degreesTrueNorth) return(Cardinal::TRUE_N);
			if (dirDegrees == degreesTrueSouth) return(Cardinal::TRUE_S);
			if (dirDegrees == degreesTrueWest) return(Cardinal::TRUE_W);
			if (dirDegrees == degreesTrueEast) return(Cardinal::TRUE_E);
		}
		//Degree values specifying cardinal direction sectors must be sorted.
		auto sectorSize = 45u; //cardinal direction sector size
		if (dirDegrees <= sectorSize/2) 						return(Cardinal::N);
		if (dirDegrees <= (degreesNorthEast + sectorSize/2)) 	return(Cardinal::NE);
		if (dirDegrees <= (degreesTrueEast + sectorSize/2))		return(Cardinal::E);
		if (dirDegrees <= (degreesSouthEast + sectorSize/2)) 	return(Cardinal::SE);
		if (dirDegrees <= (degreesTrueSouth + sectorSize/2))	return(Cardinal::S);
		if (dirDegrees <= (degreesSouthWest + sectorSize/2)) 	return(Cardinal::SW);
		if (dirDegrees <= (degreesTrueWest + sectorSize/2))		return(Cardinal::W);
		if (dirDegrees <= (degreesNorthWest + sectorSize/2)) 	return(Cardinal::NW);
		if (dirDegrees <= maxDegrees) 							return(Cardinal::N);
		return(Cardinal::NONE);
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<Pressure> Pressure::fromString(const std::string & s) {
		//static const std::regex rgx("([QA])(?:(\\d\\d\\d\\d)|////)");
		static const std::optional<Pressure> error;
		if (s.length() != 5) return(error);
		Pressure pressure;
		if (s == "A////") {
			pressure.pressureUnit = Unit::INCHES_HG;
			return(pressure);
		}
		if (s == "Q////") {
			pressure.pressureUnit = Unit::HECTOPASCAL;
			return(pressure);
		}
		const auto pr = strToUint(s, 1, 4);
		if (!pr.has_value()) return(error);
		switch (s[0]) {
			case 'A': 
				pressure.pressureUnit = Unit::INCHES_HG;
				pressure.pressureValue = pr.value() * inHgDecimalPointShift;
				break;
			case 'Q':
				pressure.pressureUnit = Unit::HECTOPASCAL;
				pressure.pressureValue = pr.value();
				break;
			default:  return(error);
		}
		return(pressure);
	}

	std::optional<Pressure> Pressure::fromForecastString(const std::string & s) {
		//static const std::regex rgx("QNH(\\d\\d\\d\\d)INS");
		static const std::optional<Pressure> error;
		if (s.length() != 10) return(error);
		if (s[0] != 'Q' || s[1] != 'N' || s[2] != 'H') return(error);
		if (s[7] != 'I' || s[8] != 'N' || s[9] != 'S') return(error);
		const auto pr = strToUint(s, 3, 4);
		if (!pr.has_value()) return(error);
		Pressure pressure;
		pressure.pressureUnit = Unit::INCHES_HG;
		pressure.pressureValue = pr.value() * inHgDecimalPointShift;
		return(pressure);
	}

	std::optional<Pressure> Pressure::fromSlpString(const std::string & s) {
		//SLP982 = 998.2 hPa, SLP015 = 1001.5 hPa, SLP221 = 1022.1 hPa
		//static const std::regex rgx("SLP(\\d\\d\\d)");
		static const std::optional<Pressure> error;
		if (s.length() != 6) return(error);
		if (s[0] != 'S' || s[1] != 'L' || s[2] != 'P') return(error);
		const auto pr = strToUint(s, 3, 3);
		if (!pr.has_value()) return(error);
		static const auto slpDecimalPointShift = 0.1;
		const auto base = (pr.value() < 500) ? 1000 : 900;
		Pressure pressure;
		pressure.pressureUnit = Unit::HECTOPASCAL;
		pressure.pressureValue = pr.value() * slpDecimalPointShift + base;
		return(pressure);
	}

	std::optional<Pressure> Pressure::fromQfeString(const std::string & s) {
		//static const std::regex rgx("QFE(\\d\\d\\d)(/\\d\\d\\d\\d)?");
		static const std::optional<Pressure> error;
		if (s.length() != 6 && s.length() != 11) return(error);
		if (s[0] != 'Q' || s[1] != 'F' || s[2] != 'E') return(error);
		const auto mmHg = strToUint(s, 3, 3);
		if (!mmHg.has_value()) return(error);
		if (s.length() == 11) {
			const auto hPa = strToUint(s, 7, 4);
			if (!hPa.has_value() || s[6] != '/') return(error);	
			//Value in hPa is ignored (parsed only for group syntax check)
		} 
		Pressure pressure;
		pressure.pressureUnit = Unit::MM_HG;
		pressure.pressureValue = mmHg.value();
		return(pressure);
	}

	std::optional<float> Pressure::toUnit(Unit unit) const {
		if (!pressureValue.has_value()) return(std::optional<float>());
		auto v = pressureValue.value();
		if (pressureUnit == unit) return(v);
		static const auto hpaPerInHg = 33.8639;
		static const auto hpaPerMmHg = 1.3332;
		static const auto mmPerInch = 25.4;
		if (pressureUnit == Unit::HECTOPASCAL && unit == Unit::INCHES_HG) {
			return(v / hpaPerInHg);
		}
		if (pressureUnit == Unit::HECTOPASCAL && unit == Unit::MM_HG) {
			return(v / hpaPerMmHg);
		}
		if (pressureUnit == Unit::INCHES_HG && unit == Unit::HECTOPASCAL) {
			return(v * hpaPerInHg);
		}
		if (pressureUnit == Unit::INCHES_HG && unit == Unit::MM_HG) {
			return(v * mmPerInch);
		}
		if (pressureUnit == Unit::MM_HG && unit == Unit::HECTOPASCAL) {
			return(v * hpaPerMmHg);
		}
		if (pressureUnit == Unit::MM_HG && unit == Unit::INCHES_HG) {
			return(v / mmPerInch);
		}
		return(std::optional<float>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<Precipitation> Precipitation::fromRainfallString(const std::string & s) {
		//static const std::regex rgx("\\d?\\d\\d\\.\\d");
		static const std::optional<Precipitation> error;
		if (s.empty() || s == "///./" || s == "//./") return(Precipitation());
		if (s.length() != 4 && s.length() != 5) return(error);
		if (s[s.length()-2] != '.') return(error);
		const auto fractPart = strToUint(s, s.length() - 1, 1);
		if (!fractPart.has_value()) return(error);
		const auto intPart = strToUint(s, 0, s.length() - 2);
		if (!intPart.has_value()) return(error);
		Precipitation precipitation;
		precipitation.precipStatus = Precipitation::Status::REPORTED;
		precipitation.precipValue = intPart.value() + 0.1 *fractPart.value();
		return(precipitation);
	}

	std::optional<Precipitation> Precipitation::fromRunwayDeposits(const std::string & s) {
		//static const std::regex rgx("\\d\\d");
		std::optional<Precipitation> error;
		if (s.length() != 2) return(error);
		if (s == "//") return(Precipitation());
		const auto dep = strToUint(s, 0, 2);
		if (!dep.has_value()) return(error);

		auto value = dep.value();
		Precipitation precipitation;
		precipitation.precipStatus = Status::REPORTED;
		switch (value) {
			case Reserved::RESERVED: return(error);

			case Reserved::DEPTH_10CM: value = 100; break;
			case Reserved::DEPTH_15CM: value = 150; break;
			case Reserved::DEPTH_20CM: value = 200; break;
			case Reserved::DEPTH_25CM: value = 250; break;
			case Reserved::DEPTH_30CM: value = 300; break;
			case Reserved::DEPTH_35CM: value = 350; break;
			case Reserved::DEPTH_40CM: value = 400; break;

			case Reserved::RUNWAY_NOT_OPERATIONAL: 
			value = 0;
			precipitation.precipStatus = Status::RUNWAY_NOT_OPERATIONAL;
			break;

			default: break;
		}
		precipitation.precipValue = value;
		return(precipitation);
	}

	std::optional<float> Precipitation::toUnit(Unit unit) const {
		if (precipStatus != Status::REPORTED) return(std::optional<float>());
		if (precipUnit == unit) return(precipValue);
		static const auto mmPerInch = 25.4;
		if (precipUnit == Unit::MM && unit == Unit::INCHES) return(precipValue / mmPerInch);
		return(std::optional<float>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<SurfaceFriction> SurfaceFriction::fromString(const std::string & s) {
		//static const std::regex rgx("\\d\\d");
		static const std::optional<SurfaceFriction> error;
		if (s.length() != 2) return(error);
		if (s == "//") return(SurfaceFriction());
		const auto sfVal = strToUint(s, 0, 2);
		if (!sfVal.has_value()) return(error);

		SurfaceFriction sf;
		auto coefficient = sfVal.value();
		switch (coefficient) {
			case Reserved::BRAKING_ACTION_POOR:
			sf.sfStatus = Status::BRAKING_ACTION_REPORTED;
			sf.sfCoefficient = baPoorLowLimit;
			break;

			case Reserved::BRAKING_ACTION_MEDIUM_POOR:
			sf.sfStatus = Status::BRAKING_ACTION_REPORTED;
			sf.sfCoefficient = baMediumPoorLowLimit;
			break;

			case Reserved::BRAKING_ACTION_MEDIUM:
			sf.sfStatus = Status::BRAKING_ACTION_REPORTED;
			sf.sfCoefficient = baMediumLowLimit;
			break;

			case Reserved::BRAKING_ACTION_MEDIUM_GOOD:
			sf.sfStatus = Status::BRAKING_ACTION_REPORTED;
			sf.sfCoefficient = baMediumGoodLowLimit;
			break;

			case Reserved::BRAKING_ACTION_GOOD:
			sf.sfStatus = Status::BRAKING_ACTION_REPORTED;
			sf.sfCoefficient = baGoodLowLimit;
			break;

			case Reserved::RESERVED_96:
			case Reserved::RESERVED_97:
			case Reserved::RESERVED_98:
			return(std::optional<SurfaceFriction>());

			case Reserved::UNRELIABLE:
			sf.sfStatus = Status::UNRELIABLE;
			break;

			default:
			sf.sfCoefficient = coefficient;
			sf.sfStatus = Status::SURFACE_FRICTION_REPORTED;
			break;
		}
		return(sf);
	}

	SurfaceFriction::BrakingAction SurfaceFriction::brakingAction() const {
		if (sfStatus == Status::NOT_REPORTED || 
			sfStatus == Status::UNRELIABLE) return(BrakingAction::NONE);
		if (sfCoefficient < baMediumPoorLowLimit) 	return(BrakingAction::POOR);
		if (sfCoefficient < baMediumLowLimit) 		return(BrakingAction::MEDIUM_POOR);
		if (sfCoefficient < baMediumGoodLowLimit) 	return(BrakingAction::MEDIUM);
		if (sfCoefficient < baGoodLowLimit)			return(BrakingAction::MEDIUM_GOOD);
		return(BrakingAction::GOOD);
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<WaveHeight> WaveHeight::fromString(const std::string & s) {
		//static const std::regex rgx("S(\\d)|H(\\d?\\d?\\d)");
		static const std::optional<WaveHeight> error;
		if (s.length() < 2 || s.length() > 4) return(error);
		WaveHeight wh;
		if (s == "H///") {
			wh.whType = Type::WAVE_HEIGHT;
			return(wh);
		}
		if (s == "S/") {
			wh.whType = Type::STATE_OF_SURFACE;
			return(wh);
		}
		if (s[0] == 'S') {
			if (s.length() != 2) return(error);
			auto h = waveHeightFromStateOfSurfaceChar(s[1]);
			if (!h.has_value()) return(error);
			wh.whType = Type::STATE_OF_SURFACE;
			wh.whValue = h.value();
			return(wh);
		}
		if (s[0] == 'H') {
			auto h = strToUint(s, 1, s.length() - 1);			
			if (!h.has_value()) return(error);
			wh.whType = Type::WAVE_HEIGHT;
			wh.whValue = h.value();
			return(wh);
		}
		return(error);
	}

	std::optional<float> WaveHeight::toUnit(Unit unit) const {
		const auto wh = waveHeight();
		if (!wh.has_value()) return(std::optional<float>());
		if (this->unit() == unit) return(wh.value());
		if (static const float metersPerFoot = 0.3048;
			this->unit() == Unit::METERS && unit == Unit::FEET)
		{
			return(wh.value() / metersPerFoot);
		} 
		return(std::optional<float>());

	}

	WaveHeight::StateOfSurface WaveHeight::stateOfSurface() const {
		if (!whValue.has_value()) return(StateOfSurface::NOT_REPORTED);
		const auto h = whValue.value();
		// Wave heights must be sorted
		if (!h) return(StateOfSurface::CALM_GLASSY);
		if (h <= maxWaveHeightCalmRippled) return(StateOfSurface::CALM_RIPPLED);
		if (h <= maxWaveHeightSmooth) return(StateOfSurface::SMOOTH);
		if (h <= maxWaveHeightSlight) return(StateOfSurface::SLIGHT);
		if (h <= maxWaveHeightModerate) return(StateOfSurface::MODERATE);
		if (h <= maxWaveHeightRough) return(StateOfSurface::ROUGH);
		if (h <= maxWaveHeightVeryRough) return(StateOfSurface::VERY_ROUGH);
		if (h <= maxWaveHeightHigh) return(StateOfSurface::HIGH);
		if (h <= maxWaveHeightVeryHigh) return(StateOfSurface::VERY_HIGH);
		return(StateOfSurface::PHENOMENAL);
	}

	std::optional<unsigned int> WaveHeight::waveHeightFromStateOfSurfaceChar(char c) {
		switch (c) {
			case '0': return(maxWaveHeightCalmGlassy);
			case '1': return(maxWaveHeightCalmRippled);
			case '2': return(maxWaveHeightSmooth);
			case '3': return(maxWaveHeightSlight);
			case '4': return(maxWaveHeightModerate);
			case '5': return(maxWaveHeightRough);
			case '6': return(maxWaveHeightVeryRough);
			case '7': return(maxWaveHeightHigh);
			case '8': return(maxWaveHeightVeryHigh);
			case '9': return(minWaveHeightPhenomenal);
			default:  return(std::optional<unsigned int>());
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<Group> PlainTextGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<FixedGroup> FixedGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		if (reportPart == ReportPart::HEADER) {
			if (group == "METAR") return(FixedGroup(Type::METAR));
			if (group == "SPECI") return(FixedGroup(Type::SPECI));
			if (group == "TAF") return(FixedGroup(Type::TAF));
			if (group == "AMD") return(FixedGroup(Type::AMD));
		}
		if (reportPart == ReportPart::HEADER || reportPart == ReportPart::METAR) {
			if (group == "COR") return(FixedGroup(Type::COR));
		}
		if (reportPart == ReportPart::HEADER || 
			reportPart == ReportPart::METAR || 
			reportPart == ReportPart::TAF) {
				if (group == "NIL") return(FixedGroup(Type::NIL));
				if (group == "CNL") return(FixedGroup(Type::CNL));
		}
		if (reportPart == ReportPart::METAR) {
			if (group == "AUTO") return(FixedGroup(Type::AUTO));
			if (group == "SNOCLO") return(FixedGroup(Type::R_SNOCLO));
			if (group == "R/SNOCLO") return(FixedGroup(Type::R_SNOCLO));
		}
		if (reportPart == ReportPart::TAF) {
			if (group == "WSCONDS") return(FixedGroup(Type::WSCONDS));
		}
		if (reportPart == ReportPart::METAR || reportPart == ReportPart::TAF) {
			if (group == "CAVOK") return(FixedGroup(Type::CAVOK));
			if (group == "NSW") return(FixedGroup(Type::NSW));
			if (group == "RMK") return(FixedGroup(Type::RMK));
		}
		if (reportPart == ReportPart::RMK) {
			if (group == "AO1") return(FixedGroup(Type::AO1));
			if (group == "AO2") return(FixedGroup(Type::AO2));
			if (group == "NOSPECI") return(FixedGroup(Type::NOSPECI));
			if (group == "PRESFR") return(FixedGroup(Type::PRESFR));
			if (group == "PRESRR") return(FixedGroup(Type::PRESRR));
			if (group == "RVRNO") return(FixedGroup(Type::RVRNO));
			if (group == "PWINO") return(FixedGroup(Type::PWINO));
			if (group == "PNO") return(FixedGroup(Type::PNO));
			if (group == "FZRANO") return(FixedGroup(Type::FZRANO));
			if (group == "TSNO") return(FixedGroup(Type::TSNO));
			if (group == "SLPNO") return(FixedGroup(Type::SLPNO));
		}
		if (group == "$") return(FixedGroup(Type::MAINTENANCE_INDICATOR));
		return(std::optional<FixedGroup>());
	}

	std::optional<Group> FixedGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<LocationGroup> LocationGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<LocationGroup> notRecognised;
		if (reportPart != ReportPart::HEADER) return(notRecognised);
		static const std::regex rgx = std::regex("[A-Z][A-Z0-9]{3}");		
		if (!regex_match(group, rgx)) return(notRecognised);
		LocationGroup result;
		strncpy(result.location, group.c_str(), locationLength);
		result.location[locationLength] = '\0';
		return(result);
	}

	std::optional<Group> LocationGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<ReportTimeGroup> ReportTimeGroup::parse(const std::string & group, 
		ReportPart reportPart) 
	{
		static const std::optional<ReportTimeGroup> notRecognised;
		static const std::regex rgx ("\\d\\d\\d\\d\\d\\dZ");
		static const auto posTime = 0, lenTime = 6;
		if (reportPart != ReportPart::HEADER) return(notRecognised);
		if (!regex_match(group, rgx)) return(notRecognised);
		const auto tm = MetafTime::fromStringDDHHMM(group.substr(posTime, lenTime));
		if (!tm.has_value()) return(notRecognised);
		if (!tm->day().has_value()) return(notRecognised);
		ReportTimeGroup g; 
		g.t = tm.value();
		return(g);
	}

	std::optional<Group> ReportTimeGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<TrendGroup> TrendGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		if (reportPart == ReportPart::METAR || reportPart == ReportPart::TAF) {
			// Detect trend type fixed groups
			if (group == "BECMG") return(TrendGroup(Type::BECMG));
			if (group == "TEMPO") return(TrendGroup(Type::TEMPO));
			if (group == "INTER") return(TrendGroup(Type::INTER));
		}
		if (reportPart == ReportPart::TAF) {
			// Detect probability groups
			if (group == "PROB30") return(TrendGroup(Probability::PROB_30));
			if (group == "PROB40") return(TrendGroup(Probability::PROB_40));
			// Detect time span groups
			if (auto timeSpan = fromTimeSpan(group); timeSpan.has_value()) return(timeSpan);
			// Detect FMxxxxxx groups
			if (auto longFm = fromFm(group); longFm.has_value()) return(longFm);
		}
		if (reportPart == ReportPart::METAR){
			// Detect NOSIG trend type
			if ( group == "NOSIG") return(TrendGroup(Type::NOSIG));
			// Detect FMxxxx /TLxxxx /ATxxxx
			if (auto trendTime = fromTrendTime(group); trendTime.has_value()) return(trendTime);
		}
		if (reportPart == ReportPart::HEADER || reportPart == ReportPart::TAF) {
			// Detect time span
			if (auto ts = fromTimeSpan(group); ts.has_value()) return(ts);
		}
		// Cannot detect group
		return(std::optional<TrendGroup>());
	}

	std::optional<Group> TrendGroup::combine(const Group & nextGroup) const {
		if (!std::holds_alternative<TrendGroup>(nextGroup)) return(std::optional<Group>());
		auto nextTrendGroup = std::get<TrendGroup>(nextGroup);
		TrendGroup combinedGroup = *this;
		if (combinedGroup.combineProbAndTrendTypeGroups(nextTrendGroup)) return(combinedGroup);
		if (combinedGroup.combineTrendTypeAndTimeGroup(nextTrendGroup)) return(combinedGroup);
		if (combinedGroup.combineProbAndTimeSpanGroups(nextTrendGroup)) return(combinedGroup);
		if (combinedGroup.combineIncompleteGroups(nextTrendGroup)) return(combinedGroup);
		return(std::optional<Group>());
	}

	std::optional<TrendGroup> TrendGroup::fromTimeSpan(const std::string & s) {
		static const std::optional<TrendGroup> notRecognised;
		static const std::regex rgx("(\\d\\d\\d\\d)/(\\d\\d\\d\\d)");
		static const auto matchFrom = 1, matchTill = 2;
		std::smatch match;
		if (!regex_match(s, match, rgx)) return(notRecognised);
		const auto from = MetafTime::fromStringDDHH(match.str(matchFrom));
		const auto till = MetafTime::fromStringDDHH(match.str(matchTill));
		if (!from.has_value() || !till.has_value()) return(notRecognised);

		TrendGroup result;
		result.t = Type::TIME_SPAN;
		result.tFrom = from;
		result.tTill = till;
		return(result);
	}

	std::optional<TrendGroup> TrendGroup::fromFm(const std::string & s) {
		static const std::optional<TrendGroup> notRecognised;
		static const std::regex rgx("FM\\d\\d\\d\\d\\d\\d");
		static const auto posTime = 2, lenTime = 6;
		if (!regex_match(s, rgx)) return(notRecognised);
		const auto time = MetafTime::fromStringDDHHMM(s.substr(posTime, lenTime));
		if (!time.has_value()) return(notRecognised);

		TrendGroup result;
		result.t = Type::FROM;
		result.tFrom = time;
		return(result);
	}

	std::optional<TrendGroup> TrendGroup::fromTrendTime(const std::string & s) {
		static const std::optional<TrendGroup> notRecognised;
		static const std::regex rgx("([FTA][MLT])(\\d\\d\\d\\d)");
		static const auto matchType = 1, matchTime = 2;
		std::smatch match;
		if (!regex_match(s, match, rgx)) return(notRecognised);
		const auto time = MetafTime::fromStringDDHHMM(match.str(matchTime));
		if (!time.has_value()) return(notRecognised);
		TrendGroup result;
		result.t = Type::NONE;
		if (match.str(matchType) == "FM") {
			result.tFrom = time;
			return(result);
		}
		if (match.str(matchType) == "TL") {
			result.tTill = time;
			return(result);
		}
		if (match.str(matchType) == "AT") {
			result.tAt = time;
			return(result);
		}
		return(notRecognised);
	}

	bool TrendGroup::combineProbAndTrendTypeGroups(const TrendGroup & nextTrendGroup) {
		if (!isProbabilityGroup()) return(false);
		if (!nextTrendGroup.isTrendTypeGroup()) return(false);
		if (nextTrendGroup.type() != Type::TEMPO && 
			nextTrendGroup.type() != Type::INTER) return(false);
		t = nextTrendGroup.type();
		return(true);
	}

	bool TrendGroup::combineTrendTypeAndTimeGroup(const TrendGroup & nextTrendGroup) {
		if (type() != Type::BECMG && 
			type() != Type::TEMPO && 
			type() != Type::INTER) return(false);
		if (!nextTrendGroup.isTimeSpanGroup() && 
			!nextTrendGroup.isTrendTimeGroup()) return(false);
		if (!canCombineTime(*this, nextTrendGroup)) return(false);
		combineTime(nextTrendGroup);
		return(true);
	}

	bool TrendGroup::combineProbAndTimeSpanGroups(const TrendGroup & nextTrendGroup) {
		if (!isProbabilityGroup()) return(false);
		if (!nextTrendGroup.isTimeSpanGroup()) return(false);
		combineTime(nextTrendGroup);
		t = Type::TIME_SPAN;
		return(true);
	}

	bool TrendGroup::combineIncompleteGroups(const TrendGroup & nextTrendGroup) {
		if (type() != Type::NONE) return(false);
		if (probability() != Probability::NONE) return(false);
		if (!nextTrendGroup.isTrendTimeGroup()) return(false);
		if (!canCombineTime(*this, nextTrendGroup)) return(false);
		combineTime(nextTrendGroup);
		return(true);
	}

	bool TrendGroup::isProbabilityGroup() const {
		// Probability group has format PROB30 or PROB40
		// Probability must be reported and no time allowed
		if (type() != Type::NONE) return(false);
		if (probability() == Probability::NONE) return(false);
		if (timeFrom().has_value() || timeTill().has_value()) return(false);
		if (timeAt().has_value()) return(false);
		return(true);
	}

	bool TrendGroup::isTrendTypeGroup() const {
		// Trend type group is a fixed group BECMG / TEMPO / INTER
		// No probability or time allowed
		if (type() != Type::BECMG && 
			type() != Type::TEMPO && 
			type() != Type::INTER) return(false);
		if (probability() != Probability::NONE) return(false);
		if (timeFrom().has_value() || timeTill().has_value()) return(false);
		if (timeAt().has_value()) return(false);
		return(true);
	}

	bool TrendGroup::isTrendTimeGroup() const {
		// Trend time group has format FMxxxx, TLxxxx, ATxxxx
		// Only one time from timeFrom, timeTill or timeAt can be reported
		if (type() != Type::NONE) return(false);
		if (probability() != Probability::NONE) return(false);
		if (!timeFrom() && !timeTill() && !timeAt()) return(false);
		if (timeFrom() && timeTill()) return(false);
		if (timeFrom() && timeAt()) return(false);
		if (timeTill() && timeAt()) return(false);
		return(true);
	}

	bool TrendGroup::isTimeSpanGroup() const {
		// Time span group has format xxxx/xxxx, 
		// only time 'from' and 'till' must be reported
		if (type() != Type::TIME_SPAN) return(false);
		if (probability() != Probability::NONE) return(false);
		if (!timeFrom().has_value() || !timeTill().has_value()) return(false);
		if (timeAt().has_value()) return(false);
		return(true);
	}

	bool TrendGroup::canCombineTime(const TrendGroup & g1, const TrendGroup & g2) {
		// Cannot combine time 'from' with 'from', 'till' with 'till', 'at' with 'at'
		if (g1.timeFrom().has_value() && g2.timeFrom().has_value()) return(false);
		if (g1.timeTill().has_value() && g2.timeTill().has_value()) return(false);
		if (g1.timeAt().has_value() && g2.timeAt().has_value()) return(false);
		// Cannot combine time 'from' or 'till' with 'at'
		if (g1.timeAt().has_value() &&
			(g2.timeFrom().has_value() || g2.timeTill().has_value())) return(false);
		if (g2.timeAt().has_value() &&
			(g1.timeFrom().has_value() || g1.timeTill().has_value())) return(false);
		return(true);
	}

	void TrendGroup::combineTime(const TrendGroup & nextTrendGroup) {
		if (!timeFrom().has_value()) tFrom = nextTrendGroup.timeFrom();
		if (!timeTill().has_value()) tTill = nextTrendGroup.timeTill();
		if (!timeAt().has_value()) tAt = nextTrendGroup.timeAt();
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<WindGroup> WindGroup::parse(const std::string & group, 
			ReportPart reportPart)
	{
		static const std::optional<WindGroup> notRecognised;
		static const std::regex windRgx("(?:WS(\\d\\d\\d)/)?"
			"(\\d\\d0|VRB|///)([1-9]?\\d\\d|//)(?:G([1-9]?\\d\\d))?([KM][TMP][HS]?)");
		static const auto matchWindShearHeight = 1, matchWindDir = 2;
		static const auto matchWindSpeed = 3, matchWindGust = 4, matchWindUnit = 5;
		static const std::regex varWindRgx("(\\d\\d0)V(\\d\\d0)");
		static const auto matchVarWindBegin = 1, matchVarWindEnd = 2;
		if (reportPart != ReportPart::METAR && 
			reportPart != ReportPart::TAF) return(notRecognised);
		if (std::smatch match; std::regex_match(group, match, windRgx)) {
			const auto speedUnit = Speed::unitFromString(match.str(matchWindUnit));
			if (!speedUnit.has_value()) return(notRecognised);
			WindGroup result;
			const auto dir = Direction::fromDegreesString(match.str(matchWindDir));
			if (!dir.has_value()) return(notRecognised);
			result.windDir = dir.value();
			const auto speed = Speed::fromString(match.str(matchWindSpeed), speedUnit.value());
			if (!speed.has_value()) return(notRecognised);
			result.wSpeed = speed.value();
			const auto gust = Speed::fromString(match.str(matchWindGust), speedUnit.value());
			if (gust.has_value()) result.gSpeed = gust.value();
			const auto wsHeight = Distance::fromHeightString(match.str(matchWindShearHeight));
			if (wsHeight.has_value()) result.wShHeight = wsHeight.value();
			return(result);
		}
		if (std::smatch match; std::regex_match(group, match, varWindRgx)) {
			WindGroup result;
			const auto begin = Direction::fromDegreesString(match.str(matchVarWindBegin));
			if (!begin.has_value()) return(notRecognised);
			result.vsecBegin = begin.value();
			const auto end = Direction::fromDegreesString(match.str(matchVarWindEnd));
			if (!end.has_value()) return(notRecognised);
			result.vsecEnd = end.value();
			return(result);
		}
		return(notRecognised);
	}

	std::optional<Group> WindGroup::combine(const Group & nextGroup) const {
		if (!std::holds_alternative<WindGroup>(nextGroup)) return(std::optional<Group>());
		auto nextWindGroup = std::get<WindGroup>(nextGroup);
		if (!isSurfaceWindGroup()) return(std::optional<Group>());
		if (!nextWindGroup.isWindSectorGroup()) return(std::optional<Group>());
		WindGroup combinedGroup = *this;
		combinedGroup.vsecBegin = nextWindGroup.vsecBegin;
		combinedGroup.vsecEnd = nextWindGroup.vsecEnd;
		return(combinedGroup);
	}

	bool WindGroup::isCalm() const {
		return (!direction().degrees().value_or(1) && 
			direction().status() == Direction::Status::VALUE_DEGREES &&
			!windSpeed().speed().value_or(1) && 
			!gustSpeed().speed().has_value() &&
			!windShearHeight().isReported() && 
			varSectorBegin().status() == Direction::Status::OMMITTED &&
			varSectorEnd().status() == Direction::Status::OMMITTED);
	}

	bool WindGroup::isValid() const {
		// If both wind and gust speed reported, wind speed cannot be greater than gust speed
		if (windSpeed().speed().value_or(0) >= gustSpeed().speed().value_or(999)) return(false);
		// Gust speed cannot be zero if reported
		if (!gustSpeed().speed().value_or(1)) return(false);
		// Wind shear height cannot be zero if reported
		if (!windShearHeight().integer().value_or(1)) return(false);
		// All data must be valid
		return(direction().isValid() &&
			windShearHeight().isValid() &&
			varSectorBegin().isValid() &&
			varSectorEnd().isValid());
	}

	bool WindGroup::isSurfaceWindGroup() const {
		return (direction().status() != Direction::Status::OMMITTED &&
			!windShearHeight().isReported() &&
			varSectorBegin().status() == Direction::Status::OMMITTED &&
			varSectorEnd().status() == Direction::Status::OMMITTED);
	}

	bool WindGroup::isWindSectorGroup() const {
		return (direction().status() == Direction::Status::OMMITTED &&
			!windSpeed().speed().has_value() &&
			!gustSpeed().speed().has_value() &&
			!windShearHeight().isReported() &&
			varSectorBegin().status() == Direction::Status::VALUE_DEGREES &&
			varSectorEnd().status() == Direction::Status::VALUE_DEGREES);
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<VisibilityGroup> VisibilityGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<VisibilityGroup> notRecognised;
		if (reportPart != ReportPart::METAR && reportPart != ReportPart::TAF) return(notRecognised);
		// Attempt to parse string as incomplete integer value
		if (auto incompleteLength = 1u; group.length() == incompleteLength && 
			std::isdigit(group.front()))
		{
			VisibilityGroup result;
			auto v = group.front() - '0';
			result.vis = Distance(v, Distance::Unit::STATUTE_MILES);
			result.incompleteInteger = true;
			return(result);
		}
		// Attempt to parse string as visibility in meters
		static const std::regex rgx("(\\d\\d\\d\\d|////)([NSWE][WED]?[V]?)?");
		static const auto matchVis = 1, matchDir = 2;
		std::smatch match;
		if (std::regex_match(group, match, rgx)) {
			const auto v = Distance::fromMeterString(match.str(matchVis));
			if (!v.has_value()) return(notRecognised);
			const auto d = Direction::fromCardinalString(match.str(matchDir));
			if (!d.has_value()) return(notRecognised);
			VisibilityGroup result;
			result.vis = v.value();
			result.dir = d.value();
			return(result);
		}
		// Attempt to parse as visibility in miles
		const auto v = Distance::fromMileString(group);
		if (!v.has_value()) return(notRecognised);
		VisibilityGroup result;
		result.vis = v.value();
		return(result);
	}

	std::optional<Group> VisibilityGroup::combine(const Group & nextGroup) const {
		if (!std::holds_alternative<VisibilityGroup>(nextGroup)) return(std::optional<Group>());
		auto nextVisGroup = std::get<VisibilityGroup>(nextGroup);
		if (visibility().unit() != Distance::Unit::STATUTE_MILES || 
			nextVisGroup.visibility().unit() != Distance::Unit::STATUTE_MILES) {
				return(std::optional<Group>());
		}
		if (incompleteInteger && nextVisGroup.visibility().isFraction()) {
			auto v = Distance::fromIntegerAndFraction(visibility(), 
				nextVisGroup.visibility());
			if (!v.has_value()) return(std::optional<Group>());
			VisibilityGroup combinedGroup = *this;
			combinedGroup.vis = v.value();
			combinedGroup.incompleteInteger = false;
			return(combinedGroup);
		}
		return (std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<CloudGroup> CloudGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<CloudGroup> notRecognised;
		if (reportPart != ReportPart::METAR && reportPart != ReportPart::TAF) return(notRecognised);
		// Attempt to parse fixed groups
		if (group == "NCD") return(CloudGroup(Amount::NCD));
		if (group == "NSC") return(CloudGroup(Amount::NSC));
		if (group == "CLR") return(CloudGroup(Amount::NONE_CLR));
		if (group == "SKC") return(CloudGroup(Amount::NONE_SKC));
		if (group == "NCD") return(CloudGroup(Amount::NCD));
		// Attempt to parse vertical visibility (format VVxxx)
		std::smatch match;
		// Attempt to parse 
		static const std::regex rgx(
			"([BFOSV][CEKV][CNTW]?|///)(\\d\\d\\d|///)([CT][BC][U]?|///)?");
		static const auto matchAmount = 1, matchHeight = 2, matchType = 3;
		if (!std::regex_match(group, match, rgx)) return(notRecognised);

		const auto amount = amountFromString(match.str(matchAmount));
		if (!amount.has_value()) return(notRecognised);
		const auto height = Distance::fromHeightString(match.str(matchHeight));
		if (!height.has_value()) return(notRecognised);
		const auto type = typeFromString(match.str(matchType));
		if (!type.has_value()) return(notRecognised);

		// If vertical visibility is given, convective cloud type must not be specified
		if (amount.value() == Amount::OBSCURED && type.value() != Type::NONE) return(notRecognised);

		CloudGroup result;
		result.amnt = amount.value();
		result.heightOrVertVis = height.value();
		result.tp = type.value();
		return(result);
	}

	std::optional<Group> CloudGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	std::optional<CloudGroup::Amount> CloudGroup::amountFromString(const std::string & s) {
		if (s == "FEW") return(CloudGroup::Amount::FEW);
		if (s == "SCT") return(CloudGroup::Amount::SCATTERED);
		if (s == "BKN") return(CloudGroup::Amount::BROKEN);
		if (s == "OVC") return(CloudGroup::Amount::OVERCAST);
		if (s == "VV") return(CloudGroup::Amount::OBSCURED);
		if (s == "///") return(CloudGroup::Amount::NOT_REPORTED);
		return(std::optional<Amount>());
	}

	Distance CloudGroup::height() const {
		switch(amount()) {
			case Amount::NOT_REPORTED:
			case Amount::FEW:
			case Amount::SCATTERED:
			case Amount::BROKEN:
			case Amount::OVERCAST:
			return(heightOrVertVis);

			default: 
			return(heightNotReported);
		}
	}

	std::optional<CloudGroup::Type> CloudGroup::typeFromString(const std::string & s){
		if (s.empty()) return(Type::NONE);
		if (s == "TCU") return(Type::TOWERING_CUMULUS);
		if (s == "CB") return(Type::CUMULONIMBUS);
		if (s == "///") return(Type::NOT_REPORTED);
		return(std::optional<Type>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<WeatherGroup> WeatherGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		std::optional<WeatherGroup> notRecognised;
		if (reportPart != ReportPart::METAR && reportPart != ReportPart::TAF) return(notRecognised);

		if (reportPart == ReportPart::METAR) {
			if (group == "RE//") return(notReportedRecent());
			if (group == "//") return(notReported());
		}

		if (group.empty()) return(notRecognised);
		static const std::regex rgx("(RE|[\\+-]|VC)?(MI|PR|BC|DR|BL|SH|TS|FZ)?((?:[A-Z][A-Z])*)");
		static const auto matchQualifier = 1, matchDescriptor = 2, matchWeather = 3;
		std::smatch match;
		if (!regex_match(group, match, rgx)) return(notRecognised);
		
		const auto qualifier = qualifierFromString(match.str(matchQualifier));
		if (!qualifier.has_value()) return(notRecognised);
		const auto descriptor = descriptorFromString(match.str(matchDescriptor));
		if (!descriptor.has_value()) return(notRecognised);
		
		WeatherGroup result;
		static const auto wthrTokenSize = 2;
		const auto weatherStr = match.str(matchWeather);
		for (auto i = 0; i < match.length(matchWeather); i += wthrTokenSize) {
			auto weather = weatherFromString(weatherStr.substr(i, wthrTokenSize));
			if (!weather.has_value()) return(notRecognised);
			if (result.wSize >= maxwSize) return(notRecognised);
			result.w[result.wSize++] = weather.value();
		}
		result.d = descriptor.value();
		result.q = qualifier.value();
		if (result.q == Qualifier::NONE && result.isModerateQualifier()) result.q = Qualifier::MODERATE;
		return(result);
	}

	std::optional<Group> WeatherGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	bool WeatherGroup::isModerateQualifier() const {
		for (auto i=0u; i<wSize; i++) {
			switch (w[i]) {
				case Weather::DRIZZLE:
				case Weather::RAIN:
				case Weather::SNOW_GRAINS:
				case Weather::ICE_PELLETS:
				case Weather::UNDETERMINED:
				return(true);

				case Weather::SNOW:
				if (descriptor() != Descriptor::LOW_DRIFTING && 
					descriptor() != Descriptor::BLOWING) return(true);
				break;

				default:
				break;
			}
		}
		return(false);
	}

	std::optional<WeatherGroup::Qualifier> WeatherGroup::qualifierFromString(
		const std::string & s)
	{
		if (s.empty()) return(Qualifier::NONE);
		if (s == "RE") return(Qualifier::RECENT);
		if (s == "-") return(Qualifier::LIGHT);
		if (s == "+") return(Qualifier::HEAVY);
		if (s == "VC") return(Qualifier::VICINITY);
		return(std::optional<Qualifier>());
	}

	std::optional<WeatherGroup::Descriptor> WeatherGroup::descriptorFromString(
		const std::string & s)
	{
		if (s.empty()) return(Descriptor::NONE);
		if (s == "MI") return(Descriptor::SHALLOW);
		if (s == "PR") return(Descriptor::PARTIAL);
		if (s == "BC") return(Descriptor::PATCHES);
		if (s == "DR") return(Descriptor::LOW_DRIFTING);
		if (s == "BL") return(Descriptor::BLOWING);
		if (s == "SH") return(Descriptor::SHOWERS);
		if (s == "TS") return(Descriptor::THUNDERSTORM);
		if (s == "FZ") return(Descriptor::FREEZING);
		return(std::optional<Descriptor>());
	}

	std::optional<WeatherGroup::Weather> WeatherGroup::weatherFromString(
		const std::string & s)
	{
		if (s == "DZ") return(Weather::DRIZZLE);
		if (s == "RA") return(Weather::RAIN);
		if (s == "SN") return(Weather::SNOW);
		if (s == "SG") return(Weather::SNOW_GRAINS);
		if (s == "IC") return(Weather::ICE_CRYSTALS);
		if (s == "PL") return(Weather::ICE_PELLETS);
		if (s == "GR") return(Weather::HAIL);
		if (s == "GS") return(Weather::SMALL_HAIL);
		if (s == "UP") return(Weather::UNDETERMINED);
		if (s == "BR") return(Weather::MIST);
		if (s == "FG") return(Weather::FOG);
		if (s == "FU") return(Weather::SMOKE);
		if (s == "VA") return(Weather::VOLCANIC_ASH);
		if (s == "DU") return(Weather::DUST);
		if (s == "SA") return(Weather::SAND);
		if (s == "HZ") return(Weather::HAZE);
		if (s == "PY") return(Weather::SPRAY);
		if (s == "PO") return(Weather::DUST_WHIRLS);
		if (s == "SQ") return(Weather::SQUALLS);
		if (s == "FC") return(Weather::FUNNEL_CLOUD);
		if (s == "SS") return(Weather::SANDSTORM);
		if (s == "DS") return(Weather::DUSTSTORM);
		return(std::optional<Weather>());
	}

	WeatherGroup WeatherGroup::notReported() {
		WeatherGroup result;
		result.w[0] = Weather::NOT_REPORTED; result.wSize = 1;
		return(result);
	}
	
	WeatherGroup WeatherGroup::notReportedRecent() {
		WeatherGroup result = notReported();
		result.q = Qualifier::RECENT;
		return(result);
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<float> TemperatureGroup::relativeHumidity() const {
		const auto temperatureC = airTemperature().toUnit(Temperature::Unit::C);
		const auto dewPointC = dewPoint().toUnit(Temperature::Unit::C);
		if (!temperatureC.has_value() || !dewPointC.has_value()) {
			return(std::optional<float>());
		}
		if (*temperatureC < *dewPointC) return(100.0);
		const auto saturationVapourPressure = 
			6.11 * powf(10, 7.5 * *temperatureC / (237.7 + *temperatureC));
		const auto actualVapourPressure = 
			6.11 * powf(10, 7.5 * *dewPointC / (237.7 + *dewPointC));
		return (100.0 * actualVapourPressure / saturationVapourPressure);
	}

	bool TemperatureGroup::isValid() const {
		// Either temperature or dew point not reported: always valid
		if (!airTemperature().temperature().has_value() || 
			!dewPoint().temperature().has_value()) return(true);
		// If temperature reported M00 then dew point cannot be 00
		if (!airTemperature().temperature().value() &&	
			!dewPoint().temperature().value() &&
			airTemperature().isFreezing() &&
			!dewPoint().isFreezing()) return (false);
		// Generally dew point must be less or equal to temperature
		return (airTemperature().temperature().value() >= dewPoint().temperature().value());
	}

	std::optional<TemperatureGroup> TemperatureGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<TemperatureGroup> notRecognised;
		if (reportPart != ReportPart::METAR) return(notRecognised);
		static const std::regex rgx("(M?\\d\\d|//)/(M?\\d\\d|//)?");
		static const auto matchTemperature = 1, matchDewPoint = 2;
		std::smatch match;
		if (!regex_match(group, match, rgx)) return(notRecognised);
		const auto t = Temperature::fromString(match.str(matchTemperature));
		if (!t.has_value()) return(notRecognised);
		TemperatureGroup result;
		result.t = t.value();
		if (match.length(matchDewPoint)) {
			const auto dp = Temperature::fromString(match.str(matchDewPoint));
			if (!dp.has_value()) return(notRecognised);
			result.dp = dp.value();
		}
		return(result);
	}

	std::optional<Group> TemperatureGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<TemperatureForecastGroup> TemperatureForecastGroup::parse(
		const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<TemperatureForecastGroup> notRecognised;
		if (reportPart != ReportPart::TAF) return (notRecognised);
		static const std::regex rgx ("(TX|TN)(M?\\d\\d)/(\\d\\d\\d\\d)Z");
		static const auto matchPoint = 1, matchTemperature = 2, matchTime = 3;
		std::smatch match;
		if (!std::regex_match(group, match, rgx)) return(notRecognised);
		auto point = pointFromString(match.str(matchPoint));
		if (!point.has_value()) return(notRecognised);
		auto temp = metaf::Temperature::fromString(match.str(matchTemperature));
		if (!temp.has_value()) return(notRecognised);
		auto time = metaf::MetafTime::fromStringDDHH(match.str(matchTime));
		if (!time.has_value()) return(notRecognised);
		TemperatureForecastGroup result;
		result.p = point.value();
		result.t = temp.value();
		result.tm = time.value();
		return(result);
	}

	std::optional<Group> TemperatureForecastGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<PressureGroup> PressureGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<PressureGroup> notRecognised;
		if (reportPart == metaf::ReportPart::METAR) {
			const auto pressure = metaf::Pressure::fromString(group);
			if (!pressure.has_value()) return(notRecognised);
			PressureGroup result;
			result.p = pressure.value();
			result.t = Type::OBSERVED_QNH;
			return(result);
		} 
		if (reportPart == metaf::ReportPart::TAF) {
			const auto pressure = metaf::Pressure::fromForecastString(group);
			if (!pressure.has_value()) return(notRecognised);
			PressureGroup result;
			result.p = pressure.value();
			result.t = Type::FORECAST_LOWEST_QNH;
			return(result);
		}
		if (reportPart == metaf::ReportPart::RMK) {
			if (const auto pr = metaf::Pressure::fromSlpString(group); pr.has_value()) {
				PressureGroup result;
				result.p = pr.value();
				result.t = Type::OBSERVED_QNH;
				return(result);
			}
			if (const auto pr = metaf::Pressure::fromQfeString(group); pr.has_value()) {
				PressureGroup result;
				result.p = pr.value();
				result.t = Type::OBSERVED_QFE;
				return(result);
			}
			return(notRecognised);
		}
		return(notRecognised);
	}

	std::optional<Group> PressureGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<RunwayVisualRangeGroup> RunwayVisualRangeGroup::parse(
		const std::string & group, ReportPart reportPart)
	{
		static const std::optional<RunwayVisualRangeGroup> notRecognised;
		if (reportPart != ReportPart::METAR) return(notRecognised);
		static const std::regex rgx("(R\\d\\d[RCL]?)/(////|[PM]?\\d\\d\\d\\d)"
			"(?:V([PM]?\\d\\d\\d\\d))?(FT/?)?([UND/])?");
		static const auto matchRunway = 1, matchRvr = 2, matchVarRvr = 3, matchUnit = 4;
		static const auto matchTrend = 5;
		std::smatch match;
		if (!regex_match(group, match, rgx)) return(notRecognised);
		const auto tr = trendFromString(match.str(matchTrend));
		if (!tr.has_value()) return(notRecognised);
		const bool unitFeet = match.length(matchUnit);
		const auto runway = Runway::fromString(match.str(matchRunway));
		if (!runway.has_value()) return(notRecognised);
		const auto rvr = Distance::fromRvrString(match.str(matchRvr), unitFeet);
		if (!rvr.has_value()) return(notRecognised);
		RunwayVisualRangeGroup result;
		if (match.length(matchVarRvr)) {
			const auto varRvr = Distance::fromRvrString(match.str(matchVarRvr), unitFeet);
			if (!varRvr.has_value()) return(notRecognised);
			result.varVisRange = varRvr.value();
		}
		result.rw = runway.value();
		result.visRange = rvr.value();
		result.trnd = tr.value();
		return(result);
	}

	std::optional<Group> RunwayVisualRangeGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	std::optional<RunwayVisualRangeGroup::Trend> RunwayVisualRangeGroup::trendFromString(
		const std::string & s)
	{
		if (s.empty()) return(Trend::NONE);
		if (s == "/") return(Trend::NOT_REPORTED);
		if (s == "U") return(Trend::UPWARD);
		if (s == "N") return(Trend::NEUTRAL);
		if (s == "D") return(Trend::DOWNWARD);
		return(std::optional<Trend>());
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<RunwayStateGroup> RunwayStateGroup::parse(const std::string & group, 
			ReportPart reportPart)
	{
		static const std::optional<RunwayStateGroup> notRecognised;
		if (reportPart != ReportPart::METAR) return(notRecognised);
		static const std::regex rgx("(R\\d\\d[RCL]?)/"
			"(?:(SNOCLO)|(?:(\\d|/)(\\d|/)(\\d\\d|//)|(CLRD))(\\d\\d|//))");
		static const auto matchRunway = 1, matchSnoclo = 2, matchDeposits = 3;
		static const auto matchExtent = 4, matchDepth = 5, matchClrd = 6, matchFriction = 7;
		std::smatch match;
		if (!regex_match(group, match, rgx)) return(notRecognised);
		const auto runway = Runway::fromString(match.str(matchRunway));
		if (!runway.has_value()) return(notRecognised);
		if (match.length(matchSnoclo)) return(runwaySnoclo(runway.value()));
		const auto friction = SurfaceFriction::fromString(match.str(matchFriction));
		if (!friction.has_value()) return(notRecognised);
		if (match.length(matchClrd)) return(runwayClrd(runway.value(), friction.value()));
		const auto deposits = depositsFromString(match.str(matchDeposits));
		if (!deposits.has_value()) return(notRecognised);
		const auto extent = extentFromString(match.str(matchExtent));
		if (!extent.has_value()) return(notRecognised);
		const auto depth = Precipitation::fromRunwayDeposits(match.str(matchDepth));
		if (!depth.has_value()) return(notRecognised);
		RunwayStateGroup result;
		result.st = Status::NORMAL;
		result.rw = runway.value();
		result.dp = deposits.value();
		result.ext = extent.value();
		result.dDepth = depth.value();
		result.sf = friction.value();
		return(result);
	}

	std::optional<Group> RunwayStateGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	RunwayStateGroup RunwayStateGroup::runwaySnoclo(Runway runway) {
		RunwayStateGroup result; 
		result.rw = std::move(runway); result.st = Status::SNOCLO;
		return(result);
	}

	RunwayStateGroup RunwayStateGroup::runwayClrd(Runway runway, 
		SurfaceFriction surfaceFriction)
	{
		RunwayStateGroup result; 
		result.rw = std::move(runway); result.st = Status::CLRD;
		result.sf = std::move(surfaceFriction);
		return(result);
	}

	std::optional<RunwayStateGroup::Deposits> RunwayStateGroup::depositsFromString(
		const std::string & s)
	{
		std::optional<Deposits> error;
		if (s.length() != 1) return(error);
		switch (s[0]) {
			case '0': return(Deposits::CLEAR_AND_DRY);
			case '1': return(Deposits::DAMP);
			case '2': return(Deposits::WET_AND_WATER_PATCHES);
			case '3': return(Deposits::RIME_AND_FROST_COVERED);
			case '4': return(Deposits::DRY_SNOW);
			case '5': return(Deposits::WET_SNOW);
			case '6': return(Deposits::SLUSH);
			case '7': return(Deposits::ICE);
			case '8': return(Deposits::COMPACTED_OR_ROLLED_SNOW);
			case '9': return(Deposits::FROZEN_RUTS_OR_RIDGES);
			case '/': return(Deposits::NOT_REPORTED);
			default:  return(error);
		}
	}

	std::optional<RunwayStateGroup::Extent> RunwayStateGroup::extentFromString(
		const std::string & s)
	{
		std::optional<Extent> error;
		if (s.length() != 1) return(error);
		switch (s[0]) {
			case '0': return(Extent::NONE);
			case '1': return(Extent::LESS_THAN_10_PERCENT);
			case '2': return(Extent::FROM_11_TO_25_PERCENT);
			case '3': return(Extent::RESERVED_3);
			case '4': return(Extent::RESERVED_4);
			case '5': return(Extent::FROM_26_TO_50_PERCENT);
			case '6': return(Extent::RESERVED_6);
			case '7': return(Extent::RESERVED_7);
			case '8': return(Extent::RESERVED_8);
			case '9': return(Extent::MORE_THAN_51_PERCENT);
			case '/': return(Extent::NOT_REPORTED);
			default:  return(error);
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<WindShearLowLayerGroup> WindShearLowLayerGroup::parse(
			const std::string & group,
			ReportPart reportPart)
	{
		static const std::optional<WindShearLowLayerGroup> notRecognised;
		if (reportPart != ReportPart::METAR) return(notRecognised);
		if (group == "WS") return(WindShearLowLayerGroup());
		return(notRecognised);
	}

	std::optional<Group> WindShearLowLayerGroup::combine(const Group & nextGroup) const { 
		static const std::optional<WindShearLowLayerGroup> notCombined;
		if (!std::holds_alternative<PlainTextGroup>(nextGroup)) return(notCombined);
		const auto nextGroupStr = std::get<PlainTextGroup>(nextGroup).toString();
		WindShearLowLayerGroup combinedGroup = *this;

		switch (status) {
			case Status::COMPLETE:
			return(notCombined);

			case Status::INCOMPLETE_WS:
			if (nextGroupStr == "ALL") {
				combinedGroup.status = Status::INCOMPLETE_WS_ALL;
				return(combinedGroup);
			}
			if (const auto runway = Runway::fromString(nextGroupStr, true); runway.has_value()) {
				combinedGroup.status = Status::COMPLETE;
				combinedGroup.rw = *runway;
				return(combinedGroup);
			} 
			return(notCombined);

			case Status::INCOMPLETE_WS_ALL:
			if (nextGroupStr == "RWY") {
				combinedGroup.status = Status::COMPLETE;
				combinedGroup.rw = Runway::makeAllRunways();
				return(combinedGroup);
			}
			return(notCombined);

			default:
			return(notCombined);
		}
	}

	///////////////////////////////////////////////////////////////////////////

	std::optional<RainfallGroup> RainfallGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		static const std::optional<RainfallGroup> notRecognised;
		if (reportPart != ReportPart::METAR) return(notRecognised);
		static const std::regex rgx (
			"RF(\\d\\d\\.\\d|//\\./)/(\\d\\d\\d\\.\\d|///\\./)(?:/(\\d\\d\\d\\.\\d))?");
		static const auto matchLast10Minutes = 1, matchSince9AM = 2, matchLast60Minutes = 3;
		std::smatch match;
		if (!std::regex_match(group, match, rgx)) return(notRecognised);
		const auto last10min = metaf::Precipitation::fromRainfallString(match.str(matchLast10Minutes));
		if (!last10min.has_value()) return(notRecognised);
		const auto since9AM = metaf::Precipitation::fromRainfallString(match.str(matchSince9AM));
		if (!since9AM.has_value()) return(notRecognised);
		const auto last60min = metaf::Precipitation::fromRainfallString(match.str(matchLast60Minutes));
		if (!last60min.has_value()) return(notRecognised);
		RainfallGroup result;
		result.last10m = last10min.value();
		result.since0900 = since9AM.value();
		result.last60m = last60min.value();
		return(result);
	}

	std::optional<Group> RainfallGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}
	
	///////////////////////////////////////////////////////////////////////////////

	std::optional<SeaSurfaceGroup> SeaSurfaceGroup::parse(const std::string & group, 
			ReportPart reportPart)
	{
		static const std::optional<SeaSurfaceGroup> notRecognised;
		if (reportPart != metaf::ReportPart::METAR) return(notRecognised);
		static const std::regex rgx ("W(\\d\\d|//)/([HS](?:\\d\\d?\\d?|///|/))");
		static const auto matchTemp = 1, matchWaveHeight = 2;
		std::smatch match;
		if (!std::regex_match(group, match, rgx)) return(notRecognised);
		const auto temp = metaf::Temperature::fromString(match.str(matchTemp));
		if (!temp.has_value()) return(notRecognised);
		const auto waveHeight = metaf::WaveHeight::fromString(match.str(matchWaveHeight));
		if (!waveHeight.has_value()) return(notRecognised);
		SeaSurfaceGroup result;
		result.t = temp.value();
		result.wh = waveHeight.value();
		return(result);
	}

	std::optional<Group> SeaSurfaceGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	std::optional<ColourCodeGroup> ColourCodeGroup::parse(const std::string & group, 
		ReportPart reportPart)
	{
		std::optional<ColourCodeGroup> notRecognised;
		if (reportPart!=ReportPart::METAR) return(notRecognised);
		ColourCodeGroup result;
		std::string colourCodeStr(group);
		if (std::string codeBlack("BLACK"); !group.find(codeBlack)) {
			result.cBlack = true;
			colourCodeStr = group.substr(codeBlack.length());
		}
		if (colourCodeStr == "BLU") { result.c = Code::BLUE; return(result); }
		if (colourCodeStr == "WHT") { result.c = Code::WHITE; return(result); }
		if (colourCodeStr == "GRN") { result.c = Code::GREEN; return(result); }
		if (colourCodeStr == "YLO1") { result.c = Code::YELLOW1; return(result); }
		if (colourCodeStr == "YLO2") { result.c = Code::YELLOW2; return(result); }
		if (colourCodeStr == "AMB") { result.c = Code::AMBER; return(result); }
		if (colourCodeStr == "RED") { result.c = Code::RED; return(result); }
		return(notRecognised);
	}

	std::optional<Group> ColourCodeGroup::combine(const Group & nextGroup) const { 
		(void)nextGroup; return(std::optional<Group>());
	}

	///////////////////////////////////////////////////////////////////////////////

	SyntaxGroup getSyntaxGroup(const Group & group) {
		if (auto fixedGroup = std::get_if<FixedGroup>(&group)) {
			switch (fixedGroup->type()) {
				case FixedGroup::Type::METAR:	return(SyntaxGroup::METAR);
				case FixedGroup::Type::SPECI:	return(SyntaxGroup::SPECI);
				case FixedGroup::Type::TAF:		return(SyntaxGroup::TAF);
				case FixedGroup::Type::COR:		return(SyntaxGroup::COR);
				case FixedGroup::Type::AMD:		return(SyntaxGroup::AMD);
				case FixedGroup::Type::NIL:		return(SyntaxGroup::NIL);
				case FixedGroup::Type::CNL:		return(SyntaxGroup::CNL);
				case FixedGroup::Type::RMK:		return(SyntaxGroup::RMK);
				
				case FixedGroup::Type::MAINTENANCE_INDICATOR:
				return(SyntaxGroup::MAINTENANCE_INDICATOR);
				
				default:						return(SyntaxGroup::OTHER);
			}
		}
		if (std::get_if<LocationGroup>(&group)) return(SyntaxGroup::LOCATION);
		if (std::get_if<ReportTimeGroup>(&group)) return(SyntaxGroup::REPORT_TIME);
		if (auto trendGroup = std::get_if<TrendGroup>(&group)) {
			if (trendGroup->isTimeSpanGroup()) return(SyntaxGroup::TIME_SPAN);
			return(SyntaxGroup::OTHER);
		}
		return(SyntaxGroup::OTHER);
	}

	///////////////////////////////////////////////////////////////////////////////

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	typename GenericParser<GenericGroupParser, SyntaxGroupGetter>::ParseInternalResult 
	GenericParser<GenericGroupParser, SyntaxGroupGetter>::parseInternal(
		const std::string & report, 
		bool extendedResult)
	{
		static const std::regex delimiterRegex("\\s+");
		std::sregex_token_iterator iter(report.begin(), report.end(), 
			delimiterRegex, 
			-1);
		bool reportEnd = false;
		Status status;
	
		Result result;
		ExtendedResult extresult;

		while (iter != std::sregex_token_iterator() && !reportEnd && !status.isError()) {
			std::string groupString = *iter;
			if (const char endChar = '='; groupString.back() == endChar) {
				reportEnd = true;
				groupString.pop_back();
			}
			if (groupString.length()) {
				GroupType group;
				const auto reportPart = status.getReportPart();
				do {
					group = GenericGroupParser::parse(groupString, status.getReportPart());
					status.transition(SyntaxGroupGetter(group));
				} while(status.isReparseRequired());
				const auto lastGroup = 
					(!extendedResult) ? getLastGroup(result) : getLastGroup(extresult);
				const auto combinedGroup = combineWithLastGroup(lastGroup, group);
				if (!extendedResult) {
					saveToResult(result, group, combinedGroup);	
				} else {
					saveToResult(extresult, group, combinedGroup, reportPart, groupString);
				}
				
			}
			iter++;
		}
		status.finalTransition();
		if (!extendedResult) {
			saveToResult(result, status.getReportType(), status.getError());
			return(result);
		} else {
			saveToResult(extresult, status.getReportType(), status.getError());
			return(extresult);
		}
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	std::optional<typename GenericParser<GenericGroupParser, SyntaxGroupGetter>::GroupType>
	GenericParser<GenericGroupParser, SyntaxGroupGetter>::combineWithLastGroup(
		const std::optional<GroupType> & lastGroup,
		const GroupType & group)
	{
		if (!lastGroup.has_value()) return(std::optional<GroupType>());
		return(std::visit([](auto&& previousGroup, auto && currentGroup) -> 
			std::optional<GroupType> { return (previousGroup.combine(currentGroup)); }, 
				*lastGroup, group));
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	std::optional<typename GenericParser<GenericGroupParser, SyntaxGroupGetter>::GroupType>
	GenericParser<GenericGroupParser, SyntaxGroupGetter>::getLastGroup(
		const Result & result)
	{
		if (result.groups.empty()) return(std::optional<GroupType>());
		return(result.groups.back());
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	std::optional<typename GenericParser<GenericGroupParser, SyntaxGroupGetter>::GroupType>
	GenericParser<GenericGroupParser, SyntaxGroupGetter>::getLastGroup(
		const ExtendedResult & extresult)
	{
		if (extresult.extgroups.empty()) return(std::optional<GroupType>());
		return(std::get<GroupType>(extresult.extgroups.back()));
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::saveToResult(
		Result & result, 
		GroupType group, 
		const std::optional<GroupType> & combinedGroup)
	{
		if (combinedGroup.has_value()) {
			result.groups.back() = combinedGroup.value();
		} else {
			result.groups.push_back(std::move(group));
		}
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::saveToResult(
		ExtendedResult & extresult, 
		GroupType group, 
		const std::optional<GroupType> & combinedGroup, 
		ReportPart reportPart, 
		const std::string & groupString)
	{
		if (combinedGroup.has_value()) {
			const auto newGroupStr = 
				std::get<std::string>(extresult.extgroups.back()) + " " + groupString;
			extresult.extgroups.back() = 
				std::tuple(combinedGroup.value(), reportPart, newGroupStr);
		} else {
			extresult.extgroups.push_back(std::tuple(std::move(group), reportPart, groupString));
		}
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::saveToResult(
		Result & result, 
		ReportType reportType, 
		Error error)
	{
		result.reportType = reportType;
		result.error = error;
	}

	template <class GenericGroupParser, 
	SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::saveToResult(
		ExtendedResult & extresult, 
		ReportType reportType, 
		Error error)
	{
		extresult.reportType = reportType;
		extresult.error = error;
	}


	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	ReportPart
	GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::getReportPart() {
		using StateReportPart = std::pair<State, ReportPart>;
		static const std::vector<StateReportPart> stateReportParts = {
			std::make_pair(State::REPORT_TYPE_OR_LOCATION, ReportPart::HEADER),
			std::make_pair(State::CORRECTION, ReportPart::HEADER),
			std::make_pair(State::LOCATION, ReportPart::HEADER),
			std::make_pair(State::REPORT_TIME, ReportPart::HEADER),
			std::make_pair(State::TIME_SPAN, ReportPart::HEADER),
			std::make_pair(State::REPORT_BODY_BEGIN_METAR, ReportPart::METAR),
			std::make_pair(State::REPORT_BODY_BEGIN_METAR_REPEAT_PARSE, ReportPart::METAR),
			std::make_pair(State::REPORT_BODY_METAR, ReportPart::METAR),
			std::make_pair(State::REPORT_BODY_BEGIN_TAF, ReportPart::TAF),
			std::make_pair(State::REPORT_BODY_TAF, ReportPart::TAF),
			std::make_pair(State::REMARK_METAR, ReportPart::RMK),
			std::make_pair(State::REMARK_TAF, ReportPart::RMK),
			std::make_pair(State::MAINTENANCE_INDICATOR, ReportPart::UNKNOWN),
			std::make_pair(State::NIL, ReportPart::UNKNOWN),
			std::make_pair(State::CNL, ReportPart::UNKNOWN),
			std::make_pair(State::ERROR, ReportPart::UNKNOWN)
		};
		for (const auto & srp : stateReportParts) {
			if (std::get<State>(srp) == state) return(std::get<ReportPart>(srp));
		}
		return(ReportPart::UNKNOWN);
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transition(
		SyntaxGroup group)
	{
		switch (state) {
			case State::REPORT_TYPE_OR_LOCATION:
			transitionFromReportTypeOrLocation(group);
			break;

			case State::CORRECTION:
			transitionFromCorrecton(group);
			break;

			case State::LOCATION:
			if (group == SyntaxGroup::LOCATION) {setState(State::REPORT_TIME); break;} 
			setError(Error::EXPECTED_LOCATION);
			break;

			case State::REPORT_TIME:
			transitionFromReportTime(group);
			break;

			case State::TIME_SPAN:
			transitionFromTimeSpan(group);
			break;

			case State::REPORT_BODY_BEGIN_METAR:
			case State::REPORT_BODY_BEGIN_METAR_REPEAT_PARSE:
			transitionFromReportBodyBeginMetar(group);
			break;

			case State::REPORT_BODY_METAR:
			transitionFromReportBodyMetar(group);
			break;

			case State::REPORT_BODY_BEGIN_TAF:
			transitionFromReportBodyBeginTaf(group);
			break;

			case State::REPORT_BODY_TAF:
			transitionFromReportBodyTaf(group);
			break;

			case State::REMARK_METAR:
			if (group == SyntaxGroup::MAINTENANCE_INDICATOR) {
				setState(State::MAINTENANCE_INDICATOR);
			}
			break;

			case State::REMARK_TAF:
			if (group == SyntaxGroup::MAINTENANCE_INDICATOR) {
				setError(Error::MAINTENANCE_INDICATOR_ALLOWED_IN_METAR_ONLY);
			}
			break;

			case State::MAINTENANCE_INDICATOR:
			setError(Error::UNEXPECTED_GROUP_AFTER_MAINTENANCE_INDICATOR);
			break;

			case State::NIL:
			setError(Error::UNEXPECTED_GROUP_AFTER_NIL);
			break;

			case State::CNL:
			setError(Error::UNEXPECTED_GROUP_AFTER_CNL);
			break;

			case State::ERROR:
			break;

			default:
			setError(Error::INTERNAL_PARSER_STATE);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportTypeOrLocation(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::METAR:
			case SyntaxGroup::SPECI:
			setReportType(ReportType::METAR);
			setState(State::CORRECTION);
			break;

			case SyntaxGroup::TAF:
			setReportType(ReportType::TAF);
			setState(State::CORRECTION);
			break;

			case SyntaxGroup::LOCATION:
			setState(State::REPORT_TIME);
			break;

			default:
			setError(Error::EXPECTED_REPORT_TYPE_OR_LOCATION);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromCorrecton(
		SyntaxGroup group)
	{
		switch (group) {
			case SyntaxGroup::AMD:
			setState(State::LOCATION);
			if (getReportType() != ReportType::TAF) setError(Error::AMD_ALLOWED_IN_TAF_ONLY);
			break;

			case SyntaxGroup::COR:
			setState(State::LOCATION);
			break;

			case SyntaxGroup::LOCATION:
			setState(State::REPORT_TIME);
			break;

			default:
			setError(Error::EXPECTED_LOCATION);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportTime(
		SyntaxGroup group)
	{
		switch (group) {
			case SyntaxGroup::REPORT_TIME:
			if (reportType == ReportType::METAR) {
				setState(State::REPORT_BODY_BEGIN_METAR);
				break;
			}
			setState(State::TIME_SPAN);
			break;

			case SyntaxGroup::TIME_SPAN:
			if (getReportType() == ReportType::TAF) {
				setState(State::REPORT_BODY_BEGIN_TAF);
				break;	
			}
			setError(Error::EXPECTED_REPORT_TIME);
			break;

			case SyntaxGroup::NIL:
			setState(State::NIL);
			break;

			default:
			setError(Error::EXPECTED_REPORT_TIME);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromTimeSpan(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::TIME_SPAN:
			setReportType(ReportType::TAF);
			setState(State::REPORT_BODY_BEGIN_TAF);
			break;

			case SyntaxGroup::NIL:
			setState(State::NIL);
			break;

			default:
			if (getReportType() == ReportType::UNKNOWN) {
				setReportType(ReportType::METAR);
				setState(State::REPORT_BODY_BEGIN_METAR_REPEAT_PARSE);
				break;
			}
			setError(Error::EXPECTED_TIME_SPAN);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportBodyBeginMetar(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::NIL: 
			setState(State::NIL);
			break;

			case SyntaxGroup::CNL:
			setError(Error::CNL_ALLOWED_IN_TAF_ONLY);
			break;

			case SyntaxGroup::RMK:
			setState(State::REMARK_METAR);
			break;

			case SyntaxGroup::MAINTENANCE_INDICATOR:
			setState(State::MAINTENANCE_INDICATOR);
			break;

			default:
			setState(State::REPORT_BODY_METAR);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportBodyMetar(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::RMK:
			setState(State::REMARK_METAR);
			break;

			case SyntaxGroup::MAINTENANCE_INDICATOR:
			setState(State::MAINTENANCE_INDICATOR);
			break;

			case SyntaxGroup::NIL:
			case SyntaxGroup::CNL:
			setError(Error::UNEXPECTED_NIL_OR_CNL_IN_REPORT_BODY);
			break;

			default:
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportBodyBeginTaf(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::NIL: 
			setState(State::NIL);
			break;

			case SyntaxGroup::CNL: 
			setState(State::CNL);
			break;

			case SyntaxGroup::RMK: 
			setState(State::REMARK_TAF);
			break;

			case SyntaxGroup::MAINTENANCE_INDICATOR:
			setError(Error::MAINTENANCE_INDICATOR_ALLOWED_IN_METAR_ONLY);
			break;

			default:
			setState(State::REPORT_BODY_TAF);
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::transitionFromReportBodyTaf(
		SyntaxGroup group)
	{
		switch(group) {
			case SyntaxGroup::RMK:
			setState(State::REMARK_TAF);
			break;

			case SyntaxGroup::NIL:
			case SyntaxGroup::CNL:
			setError(Error::UNEXPECTED_NIL_OR_CNL_IN_REPORT_BODY);
			break;

			case SyntaxGroup::MAINTENANCE_INDICATOR:
			setError(Error::MAINTENANCE_INDICATOR_ALLOWED_IN_METAR_ONLY);
			break;

			default:
			break;
		}
	}

	template <class GenericGroupParser, 
		SyntaxGroup (*SyntaxGroupGetter)(const typename GenericGroupParser::GroupType &)>
	void GenericParser<GenericGroupParser, SyntaxGroupGetter>::Status::finalTransition() {
		switch (state) {
			case State::REPORT_BODY_METAR:
			case State::REPORT_BODY_TAF:
			case State::REMARK_METAR:
			case State::REMARK_TAF:
			case State::MAINTENANCE_INDICATOR:
			case State::NIL:
			case State::CNL:
			case State::ERROR:
			break;

			case State::REPORT_TYPE_OR_LOCATION:
			setError(Error::EMPTY_REPORT);
			break;
			
			case State::CORRECTION:
			case State::LOCATION:
			case State::REPORT_TIME:
			case State::TIME_SPAN:
			case State::REPORT_BODY_BEGIN_METAR:
			case State::REPORT_BODY_BEGIN_METAR_REPEAT_PARSE:
			case State::REPORT_BODY_BEGIN_TAF:
			setError(Error::UNEXPECTED_REPORT_END);
			break;

			default:
			setError(Error::INTERNAL_PARSER_STATE);
			break;
		}
	}


} //namespace metaf

#endif //#ifndef METAF_H