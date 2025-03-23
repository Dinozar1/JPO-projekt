/**
* @file Utils.cpp
 * @brief Implementation of utility functions
 * @author Szymon
 */

#include "Utils.h"
#include <cmath>
#include <algorithm>

namespace Utils {

    size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *output) {
        size_t total_size = size * nmemb;
        output->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

    double CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
        // Earth's radius in kilometers
        const double R = 6371.0;

        // Convert degrees to radians
        double lat1_rad = lat1 * M_PI / 180.0;
        double lon1_rad = lon1 * M_PI / 180.0;
        double lat2_rad = lat2 * M_PI / 180.0;
        double lon2_rad = lon2 * M_PI / 180.0;

        // Differences in coordinates
        double dlat = lat2_rad - lat1_rad;
        double dlon = lon2_rad - lon1_rad;

        // Haversine formula
        double a = std::sin(dlat/2) * std::sin(dlat/2) +
                   std::cos(lat1_rad) * std::cos(lat2_rad) *
                   std::sin(dlon/2) * std::sin(dlon/2);
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));

        // Distance in kilometers
        return R * c;
    }

    std::string ToLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

} // namespace Utils