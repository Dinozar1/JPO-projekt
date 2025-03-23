/**
* @file Station.h
 * @brief Defines the Station structure to represent air quality monitoring stations
 * @author Szymon
 */

#pragma once

#include <string>

/**
 * @brief Structure to hold air quality monitoring station data
 *
 * This structure represents a single air quality monitoring station
 * with all its related information including location, address and
 * calculated distance from a reference point.
 */
struct Station {
    int id;                 ///< Unique identifier for the station
    std::string stationName; ///< Name of the station
    std::string cityName;    ///< City where the station is located
    std::string province;    ///< Province (województwo) where station is located
    std::string district;    ///< District (powiat) where station is located
    std::string address;     ///< Street address of the station
    double latitude;         ///< Geographical latitude
    double longitude;        ///< Geographical longitude
    double distance;         ///< Distance from reference point in km
};