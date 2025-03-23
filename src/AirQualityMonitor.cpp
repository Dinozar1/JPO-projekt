/**
 * @file AirQualityMonitor.cpp
 * @brief Implementation of the AirQualityMonitor class
 * @author Szymon
 */

#include "AirQualityMonitor.h"
#include "Utils.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

AirQualityMonitor::AirQualityMonitor() {
    // Initialize CURL global resources when application is created
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Fetch initial station data
    FetchAllStations();
}

AirQualityMonitor::~AirQualityMonitor() {
    // Clean up CURL global resources
    curl_global_cleanup();
}

void AirQualityMonitor::Update() {
    // Set initial window size
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("GIOŚ Air Quality Monitoring");

    // Search box by city name section
    if (ImGui::CollapsingHeader("Search by city name", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Search by city name:");
        ImGui::SameLine();
        ImGui::PushItemWidth(200);

        // When search text changes, filter stations
        if (ImGui::InputText("##SearchCity", search_query, IM_ARRAYSIZE(search_query))) {
            FilterStationsByCity();
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        if (ImGui::Button("Clear Search##City")) {
            memset(search_query, 0, IM_ARRAYSIZE(search_query));
            ResetFilters();
        }
    }

    // Search by radius section
    if (ImGui::CollapsingHeader("Search by radius", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Location address:");
        ImGui::InputText("##LocationAddress", location_address, IM_ARRAYSIZE(location_address));

        ImGui::Text("Radius (km):");
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputFloat("##Radius", &search_radius, 0.5f, 1.0f, "%.1f");

        // Ensure radius is positive
        search_radius = std::max(0.1f, search_radius);
        ImGui::PopItemWidth();

        if (ImGui::Button("Search Nearby Stations")) {
            GeocodeAddressAndFilterStations();
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear Search##Radius")) {
            memset(location_address, 0, IM_ARRAYSIZE(location_address));
            search_radius = 10.0f;  // Reset to default radius
            ResetFilters();
        }

        // Display reference point if set
        if (reference_lat != 0 && reference_lon != 0) {
            ImGui::Text("Reference point: %.6f, %.6f", reference_lat, reference_lon);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh Data")) {
        FetchAllStations();
    }

    // Display station count
    ImGui::Text("Found %zu stations", filtered_stations.size());

    // Station details window controls
    if (selected_station_index >= 0 && !filtered_stations.empty()) {
        ImGui::SameLine();
        if (ImGui::Button("Close Details")) {
            selected_station_index = -1;
            sensor_data.clear();
        }
    }

    // Table with stations
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    ImVec2 outer_size = ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 100);

    if (ImGui::BeginTable("StationsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable, outer_size)) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Station Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("City", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Province", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Distance (km)", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();

        // Populate table with filtered stations
        for (size_t i = 0; i < filtered_stations.size(); i++) {
            const auto& station = filtered_stations[i];
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%d", station.id);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", station.stationName.c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", station.cityName.c_str());

            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", station.province.c_str());

            ImGui::TableSetColumnIndex(4);
            if (station.distance > 0) {
                ImGui::Text("%.2f", station.distance);
            } else {
                ImGui::Text("-");
            }

            ImGui::TableSetColumnIndex(5);

            // Create a unique ID for the button
            std::string button_id = "Details##" + std::to_string(i);
            if (ImGui::Button(button_id.c_str())) {
                selected_station_index = i;
                FetchSensorData(station.id);
            }
        }
        ImGui::EndTable();
    }

    // Display selected station details
    if (selected_station_index >= 0 && selected_station_index < filtered_stations.size()) {
        ImGui::Separator();
        const auto& station = filtered_stations[selected_station_index];

        ImGui::Text("Station Details: %s", station.stationName.c_str());
        ImGui::Text("Address: %s", station.address.c_str());
        ImGui::Text("Coordinates: %.6f, %.6f", station.latitude, station.longitude);
        ImGui::Text("District: %s", station.district.c_str());

        if (reference_lat != 0 && reference_lon != 0) {
            ImGui::Text("Distance from search point: %.2f km", station.distance);
        }

        // Display sensor data if available
        if (!sensor_data.empty()) {
            ImGui::Separator();
            ImGui::Text("Sensor Data:");

            if (ImGui::BeginTable("SensorTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Sensor ID");
                ImGui::TableSetupColumn("Parameter");
                ImGui::TableSetupColumn("Parameter Code");
                ImGui::TableHeadersRow();

                try {
                    json sensors = json::parse(sensor_data);
                    for (const auto& sensor : sensors) {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%d", sensor["id"].get<int>());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", sensor["param"]["paramName"].get<std::string>().c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%s", sensor["param"]["paramCode"].get<std::string>().c_str());
                    }
                } catch (const json::exception& e) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("Error parsing sensor data: %s", e.what());
                }
                ImGui::EndTable();
            }
        }
    }

    ImGui::End();
}

void AirQualityMonitor::FetchAllStations() {
    CURL *curl;
    CURLcode res;
    std::string response_buffer;

    curl = curl_easy_init();
    if (curl) {
        // Set up the API endpoint URL
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.gios.gov.pl/pjp-api/rest/station/findAll");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        // Execute the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            // Parse JSON response
            try {
                json stations_json = json::parse(response_buffer);
                ParseStationsData(stations_json);
            } catch (const json::parse_error& e) {
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    // Initialize filtered_stations with all stations
    filtered_stations = all_stations;
}

void AirQualityMonitor::FetchSensorData(int station_id) {
    CURL *curl;
    CURLcode res;
    std::string response_buffer;

    curl = curl_easy_init();
    if (curl) {
        // Construct the URL with the station ID
        std::string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(station_id);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

        // Execute the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            sensor_data = response_buffer; // Store the raw JSON response
        }
        curl_easy_cleanup(curl);
    }
}

void AirQualityMonitor::GeocodeAddressAndFilterStations() {
    // Skip if address is empty
    if (strlen(location_address) == 0) {
        return;
    }

    CURL *curl;
    CURLcode res;
    std::string response_buffer;

    curl = curl_easy_init();
    if (curl) {
        // Encode the address for URL
        char *encoded_address = curl_easy_escape(curl, location_address, 0);
        if (encoded_address) {
            // Construct the URL for Nominatim geocoding API
            std::string url = "https://nominatim.openstreetmap.org/search?q=" +
                              std::string(encoded_address) +
                              "&format=json&limit=1";
            curl_free(encoded_address);

            // Set user agent (required by Nominatim's ToS)
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "User-Agent: AirQualityMonitor/1.0");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);

            // Execute the request
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                std::cerr << "curl_easy_perform() geocoding failed: " << curl_easy_strerror(res) << std::endl;
            } else {
                // Parse JSON response
                try {
                    json geocode_json = json::parse(response_buffer);
                    if (!geocode_json.empty()) {
                        // Extract coordinates from first result
                        reference_lat = std::stod(geocode_json[0]["lat"].get<std::string>());
                        reference_lon = std::stod(geocode_json[0]["lon"].get<std::string>());

                        // Filter stations by distance
                        FilterStationsByRadius();
                    } else {
                        std::cerr << "No geocoding results found for address: " << location_address << std::endl;
                    }
                } catch (const json::parse_error& e) {
                    std::cerr << "JSON parsing error in geocoding: " << e.what() << std::endl;
                }
            }
            curl_slist_free_all(headers);
        }
        curl_easy_cleanup(curl);
    }
}

void AirQualityMonitor::ParseStationsData(const json& stations_json) {
    all_stations.clear();

    for (const auto& station : stations_json) {
        Station s;

        // Extract basic station info
        s.id = station["id"].get<int>();
        s.stationName = station["stationName"].get<std::string>();
        s.distance = 0.0;  // Initialize distance

        // Handle city data (which may be null)
        if (station.contains("city") && !station["city"].is_null()) {
            const auto& city = station["city"];
            s.cityName = city["name"].get<std::string>();

            // Handle commune data (which may be null)
            if (city.contains("commune") && !city["commune"].is_null()) {
                const auto& commune = city["commune"];
                s.province = commune["provinceName"].get<std::string>();
                s.district = commune["districtName"].get<std::string>();
            }
        }

        // Handle address (which may be null)
        if (station.contains("addressStreet") && !station["addressStreet"].is_null()) {
            s.address = station["addressStreet"].get<std::string>();
        }

        // Handle coordinates (which may be null)
        if (station.contains("gegrLat") && !station["gegrLat"].is_null() &&
            station.contains("gegrLon") && !station["gegrLon"].is_null()) {
            s.latitude = std::stod(station["gegrLat"].get<std::string>());
            s.longitude = std::stod(station["gegrLon"].get<std::string>());
        }

        all_stations.push_back(s);
    }
}

void AirQualityMonitor::FilterStationsByCity() {
    // Convert search query to lowercase for case-insensitive comparison
    std::string query = Utils::ToLowerCase(search_query);

    if (query.empty()) {
        ResetFilters();
        return;
    }

    filtered_stations.clear();
    for (const auto& station : all_stations) {
        // Convert city name to lowercase
        std::string city_lower = Utils::ToLowerCase(station.cityName);

        // If city name contains the search query, add to filtered stations
        if (city_lower.find(query) != std::string::npos) {
            filtered_stations.push_back(station);
        }
    }

    // Reset selected station if it was filtered out
    selected_station_index = -1;
}

void AirQualityMonitor::FilterStationsByRadius() {
    if (reference_lat == 0 && reference_lon == 0) {
        return;
    }

    // Calculate distance for all stations from reference point
    for (auto& station : all_stations) {
        station.distance = Utils::CalculateDistance(
            reference_lat, reference_lon,
            station.latitude, station.longitude
        );
    }

    // Filter stations by radius
    filtered_stations.clear();
    for (const auto& station : all_stations) {
        if (station.distance <= search_radius) {
            filtered_stations.push_back(station);
        }
    }

    // Sort by distance (closest first)
    std::sort(filtered_stations.begin(), filtered_stations.end(),
              [](const Station& a, const Station& b) {
                  return a.distance < b.distance;
              });

    // Reset selected station if it was filtered out
    selected_station_index = -1;
}

void AirQualityMonitor::ResetFilters() {
    // Reset to showing all stations
    filtered_stations = all_stations;
    selected_station_index = -1;
}