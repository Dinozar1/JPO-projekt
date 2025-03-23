/**
* @file Utils.h
 * @brief Utility functions for the air quality monitoring application
 * @author Szymon
 */

#pragma once

#include <string>
#include <functional>

/**
 * @brief Utility functions for the Air Quality Monitor application
 */
namespace Utils {
    /**
     * @brief Callback function for curl to write response data
     *
     * @param contents Pointer to the received data
     * @param size Size of each data element
     * @param nmemb Number of data elements
     * @param output String buffer to which data will be appended
     * @return Size of processed data
     */
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output);

    /**
     * @brief Calculate distance between two geographical coordinates using Haversine formula
     *
     * @param lat1 Latitude of the first point
     * @param lon1 Longitude of the first point
     * @param lat2 Latitude of the second point
     * @param lon2 Longitude of the second point
     * @return Distance in kilometers
     */
    double CalculateDistance(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Convert a string to lowercase
     *
     * @param str The input string
     * @return Lowercase version of the input string
     */
    std::string ToLowerCase(const std::string& str);
}