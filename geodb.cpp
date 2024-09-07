#include "geodb.h"
#include "geopoint.h"
#include "geotools.h"
#include "base_classes.h"
#include "hashmap.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

bool GeoDatabase::load(const std::string& map_data_file)
{
	//FOR EACH STREET SEGMENT, ADD THE STREET NAME AS A KEY
	//AND THE TWO POINTS AS VALUES

	std::ifstream file(map_data_file); //Replace later

	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file " << map_data_file << std::endl;
		return false;
	}

	//READ THE FILE INTO THE HASHMAP CLASS WITH VALUE TYPE
	//CONSISTING OF VECTOR OF GEOPOINTS AND POINTS OF INTEREST
	std::string line;
	while (std::getline(file, line))
	{
		//GET STREET NAME
		std::string street_name = line;	
		std::getline(file, line);

		//GET THE COORDINATES OF THE STREET
		std::istringstream iss(line);
		std::string lat1, lon1, lat2, lon2;
		iss >> lat1 >> lon1 >> lat2 >> lon2;

		std::string str1 = lat1 + " " + lon1;
		std::string str2 = lat2 + " " + lon2;
		std::string streetSegment = str1 + " " + str2;

		//MAKE OUR GEOPOINTS
		GeoPoint pt1(lat1, lon1);
		GeoPoint pt2(lat2, lon2);

		//UPDATE HASHMAPS
		m_street_segments.insert(streetSegment, street_name);
		m_street_compliments.insert(str1, pt2);
		m_street_compliments.insert(str2, pt1);

		//GET THE NUMBER OF POINTS OF INTEREST
		std::getline(file, line);
		int num_POIs = std::stoi(line);

		//IF THERE ARE POINTS OF INTEREST, ADD THEM TO POI HASHMAP
		if (num_POIs > 0)
		{
			int iter = num_POIs;
			bool first = true;

			while (iter > 0)
			{
				getline(file, line);
				std::stringstream yss(line);

				std::string str = yss.str();
				size_t pos = str.find('|');
				std::string name;

				if (pos != std::string::npos)
				{
					name = str.substr(0, pos);
					str.erase(0, pos + 1);
					yss.str("");
					yss << str;
				}

				//GET THE COORDS FOR POI
				std::string poiLat, poiLon;
				yss >> poiLat >> poiLon;

				//MAKE A STRING AND GEOPOINT FOR THE POINT OF INTEREST
				std::string poiString = poiLat + " " + poiLon;
				GeoPoint poiPoint(poiLat, poiLon);

				/////////////////////////////////////////////////
				//////////////UPDATE OUR HASMAPS///////////////////
				//////////////////////////////////////////////

				//MAKE A STRING AND GEOPOINT FOR THE MIDPOINT
				GeoPoint mid = midpoint(pt1, pt2);
				std::string midString = mid.sLatitude + " " + mid.sLongitude;

				if (first)
				{
					//START TO MIDPOINT
					std::string startToMidString = lat1 + " " + lon1 + " " + midString;
					m_street_segments.insert(startToMidString, street_name);
					m_street_compliments.insert(str1, mid);
					m_street_compliments.insert(midString, pt1);

					//MIDPOINT TO END
					std::string midToEndString = midString + " " + lat2 + " " + lon2;
					m_street_segments.insert(midToEndString, street_name);
					m_street_compliments.insert(midString, pt2);
					m_street_compliments.insert(str2, mid);
				}
				
				//MIDPOINT TO POI
				std::string poiToMidString = poiString + " " + midString;
				m_street_segments.insert(poiToMidString, "a path");
				m_street_compliments.insert(poiString, mid);
				m_street_compliments.insert(midString, poiPoint);

				//ADD POI TO 3RD HASHMAP
				GeoPoint Ppoint(poiLat, poiLon);
				m_POI.insert(name, Ppoint);

				iter--;
				first = false;
			}
		}
	}

	return true;
}


//Function must run in O(1) time
bool GeoDatabase::get_poi_location(const std::string& poi, GeoPoint& point) const
{
	//GET THE POINT OF INTEREST
	//IF IT EXISTS, RETURN TRUE AND THE POINT
	//OTHERWISE, RETURN FALSE
	if (m_POI.find(poi) != nullptr)
	{
		point = *m_POI.find(poi);
		return true;
	}

	return false;
}

std::vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
{
	std::string geoStr = pt.sLatitude + " " + pt.sLongitude;
	std::vector<GeoPoint> connectedPoints = m_street_compliments.find(geoStr);
	
	return connectedPoints;
}

std::string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
{
	std::string str1 = pt1.sLatitude + " " + pt1.sLongitude;
	std::string str2 = pt2.sLatitude + " " + pt2.sLongitude;
	std::string streetSegment1 = str1 + " " + str2;
	std::string streetSegment2 = str2 + " " + str1;

	if (m_street_segments.find(streetSegment1) != nullptr)
		return *m_street_segments.find(streetSegment1);
	else if (m_street_segments.find(streetSegment2) != nullptr)
		return *m_street_segments.find(streetSegment2);
	else
		return "";
}