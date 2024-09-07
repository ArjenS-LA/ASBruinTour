#ifndef TOUR_GENERATOR_H
#define TOUR_GENERATOR_H

#include <vector>
#include <string>

#include "base_classes.h"
#include "geotools.h"

class TourGenerator : public TourGeneratorBase
{
public:
	TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router)
		: m_geodb(geodb), m_router(router) {}
	virtual ~TourGenerator() {}
	virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;
private:
	const GeoDatabaseBase& m_geodb;
	const RouterBase& m_router;

	std::string turn_direction(double diff) const
	{		
		if (diff < 0)
			diff += 360;
		else if (diff > 360)
			diff -= 360;

		if (diff < 180 && diff > 0)
			return "left";
		else if (diff > 180 && diff < 360)
			return "right";
		else
			return "";
	}

	std::string determine_direction(const double& angle) const
	{
		if (angle >= 0 && angle < 22.5)
			return "east";
		else if (angle >= 22.5 && angle < 67.5)
			return "northeast";
		else if (angle >= 67.5 && angle < 112.5)
			return "north";
		else if (angle >= 112.5 && angle < 157.5)
			return "northwest";
		else if (angle >= 157.5 && angle < 202.5)
			return "west";
		else if (angle >= 202.5 && angle < 247.5)
			return "southwest";
		else if (angle >= 247.5 && angle < 292.5)
			return "south";
		else if (angle >= 292.5 && angle < 337.5)
			return "southeast";
		else if (angle >= 337.5 && angle < 360)
			return "east";
		else
			return "";
	}


	void generate_tour_helper(const std::vector<GeoPoint>& route, 
		std::vector<TourCommand>& tour) const;
};

inline void TourGenerator::generate_tour_helper(const std::vector<GeoPoint>& route,
	std::vector<TourCommand>& tour) const
{
	for (unsigned int i = 0; i < route.size(); i++)
	{
		//ADD A PROCEED COMMAND
		if (i + 1 < route.size())
		{
			double currAngle = angle_of_line(route[i], route[i + 1]);
			std::string currStreetName = m_geodb.get_street_name(route[i], route[i + 1]);
			double currDistance = distance_earth_miles(route[i], route[i + 1]);

			//ADD A PROCEED COMMAND
			TourCommand tc;
			std::string proceedDirection = determine_direction(currAngle);
			tc.init_proceed(proceedDirection, currStreetName, currDistance, route[i], route[i + 1]);
			tour.push_back(tc);

			if (i + 2 < route.size())
			{
				std::string nextStreetName = m_geodb.get_street_name(route[i + 1], route[i + 2]);

				if (currStreetName != nextStreetName)
				{
					TourCommand tc;
					double turnAngle = angle_of_turn(route[i], route[i + 1], route[i + 2]);
					std::string nextAngleDirection = determine_direction(turnAngle);
					
					std::string turnDirection = turn_direction(turnAngle);
					std::string nextStreetName = m_geodb.get_street_name(route[i + 1], route[i + 2]);
					
					tc.init_turn(turnDirection, currStreetName);
					tour.push_back(tc);
				}
			}
		}
	}
}

#endif 