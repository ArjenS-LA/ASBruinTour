#include <vector>
#include <string>
#include <iostream>

#include "base_classes.h"
#include "tour_generator.h"
#include "stops.h"

std::vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const
{
	std::vector<TourCommand> tour_commands;

	for (size_t i = 0; i < stops.size(); ++i)
	{
		std::string currentPoi;
		std::string currentCommentary;
		if (stops.get_poi_data(i, currentPoi, currentCommentary))
		{
			TourCommand tc;
			tc.init_commentary(currentPoi, currentCommentary);
			tour_commands.push_back(tc);
		}

		//IF THERE IS ANOTHER POINT OF INTEREST FOLLOWING P THEN USE YOUR ROUTER CLASS
		//GENERATE A POINT-TO-POINT ROUTE BETWEEN THE GEOPOINT ASSOCIATED WITH P AND THE GEOPOINT OF THE NEXT POINT OF INTEREST
		if (i + 1 < stops.size())
		{
			//FIND GEOPOINT OF CURRENT POI
			GeoPoint currentPoiGeo;
			m_geodb.get_poi_location(currentPoi, currentPoiGeo);
			
			//FIND GEOPOINT OF NEXT POI
			std::string nextPoi;
			std::string nextCommentary;
			stops.get_poi_data(i + 1, nextPoi, nextCommentary);
			GeoPoint nextPoiGeo;
			m_geodb.get_poi_location(nextPoi, nextPoiGeo);

			//GENERATE ROUTE BETWEEN CURRENT POI AND NEXT POI
			std::vector<GeoPoint> route = m_router.route(currentPoiGeo, nextPoiGeo);

			//IF ROUTE IS EMPTY THEN THERE IS NO ROUTE BETWEEN THE TWO POINTS
			if (route.empty())
			{
				std::vector<TourCommand> empty;
				return empty;
			}

			//GENERATE A TOUR COMMAND PATH USING ROUTE
			generate_tour_helper(route, tour_commands);
		}
	}

	return tour_commands;
}