/**
 * @file AirQualityMonitor.h
 * @brief Main application class for monitoring air quality
 * @author Szymon
 */

#pragma once

#include "UseImGui.h"
#include "Station.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>


/**
 * @brief Main application class for the air quality monitoring system
 *
 * This class handles the UI, API requests, and data processing for
 * showing information about air quality monitoring stations from GIOŚ
 * (Chief Inspectorate of Environmental Protection in Poland).
 */
class AirQualityMonitor : public UseImGui {
public:
    /**
     * @brief Constructor initializes CURL and fetches initial data
     */
    AirQualityMonitor();

    /**
     * @brief Destructor cleans up CURL resources
     */
    ~AirQualityMonitor();

    /**
     * @brief Update method for rendering the ImGui interface
     *
     * This method is called each frame to update and render the UI.
     */
    virtual void Update() override;

private:
    std::vector<Station> all_stations;        ///< All available stations
    std::vector<Station> filtered_stations;   ///< Filtered stations based on search criteria
    char search_query[64] = {};               ///< City name search query
    char location_address[256] = {};          ///< Address for radius search
    float search_radius = 10.0f;              ///< Search radius in kilometers
    double reference_lat = 0.0;               ///< Reference point latitude
    double reference_lon = 0.0;               ///< Reference point longitude
    int selected_station_index = -1;          ///< Index of the selected station for details
    std::string sensor_data;                  ///< Sensor data JSON for the selected station

    /**
     * @brief Fetch all stations from the GIOŚ API
     */
    void FetchAllStations();

    /**
     * @brief Fetch sensor data for a specific station
     *
     * @param station_id ID of the station to fetch sensor data for
     */
    void FetchSensorData(int station_id);

    /**
     * @brief Geocode an address using Nominatim API and filter stations
     *
     * This method converts an address to geographical coordinates
     * and then filters stations based on their distance from this point.
     */
    void GeocodeAddressAndFilterStations();

    /**
     * @brief Parse stations data from JSON
     *
     * @param stations_json JSON object containing stations data
     */
    void ParseStationsData(const nlohmann::json& stations_json);

    /**
     * @brief Filter stations based on city name search query
     */
    void FilterStationsByCity();

    /**
     * @brief Filter stations based on distance from reference point
     */
    void FilterStationsByRadius();

    /**
     * @brief Reset all filters and show all stations
     */
    void ResetFilters();
};