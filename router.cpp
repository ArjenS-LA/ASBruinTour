#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <string>

#include "router.h"

//IMPLMENTATION OF A* ALGORITHM
std::vector<GeoPoint> Router::route
	(const GeoPoint& pt1, 
	 const GeoPoint& pt2) const
{
	//CONVERT GEOPOINTS TO STRINGS
	//USING THE STRINGS AS KEYS TO THE UNORDERED MAP

	std::string start = pt1.sLatitude + " " + pt1.sLongitude;
	std::string end = pt2.sLatitude + " " + pt2.sLongitude;

	//CREATE A PRIORITY QUEUE
	PriorityQueue<std::string, double> frontier;
	std::unordered_map<std::string, std::string> came_from;
	std::unordered_map<std::string, double> cost_so_far;

	frontier.put(start, 0);
	came_from[start] = start;
	cost_so_far[start] = 0;

	//WHILE THE PRIORITY QUEUE IS NOT EMPTY
	while (!frontier.empty())
	{
		std::string current = frontier.get();

		size_t pos = current.find(" ");
		std::string lat = current.substr(0, pos);
		std::string lon = current.substr(pos + 1);
		GeoPoint current_point(lat, lon);

		//IF THE CURRENT ITEM IS THE END, THEN BREAK
		if (current == end)
		{
			break;
		}

		//GET THE NEIGHBORS OF THE CURRENT ITEM
		std::vector<GeoPoint> neighbors = m_geo_db.get_connected_points(current_point);

		for (const auto& neighbor : neighbors)
		{
			//GET THE STRING REPRESENTATION OF THE NEIGHBOR
			std::string next_str = neighbor.sLatitude + " " + neighbor.sLongitude;
			size_t pos = next_str.find(" ");
			std::string nextlat = next_str.substr(0, pos);
			std::string nextlon = next_str.substr(pos + 1);
			GeoPoint next_point(nextlat, nextlon);

			double new_cost = cost_so_far[current] + distance(current_point, next_point);

			//IF THE NEIGHBOR IS NOT IN THE COST MAP OR THE NEW COST IS LESS THAN THE COST IN THE COST MAP
			if (cost_so_far.find(next_str) == cost_so_far.end() || new_cost < cost_so_far[next_str])
			{
				//UPDATE THE COST MAP
				cost_so_far[next_str] = new_cost;

				//CALCULATE THE PRIORITY OF THE NEIGHBOR
				double priority = new_cost + distance(next_point, pt2);

				//PUT THE NEIGHBOR IN THE PRIORITY QUEUE
				frontier.put(next_str, priority);

				//UPDATE THE CAME FROM MAP
				came_from[next_str] = current;
			}
		}
	}

	//RECONSTRUCT THE PATH
	std::vector<GeoPoint> path;

	//PATH NOT FOUND RETURN EMPTY VECTOR
	if (came_from.find(end) == came_from.end())
	{
		std::vector<GeoPoint> empty;
		return empty;
	}

	std::string current = end;
	while (current != start)
	{
		size_t pos = current.find(" ");
		std::string lat = current.substr(0, pos);
		std::string lon = current.substr(pos + 1);
		GeoPoint current_point(lat, lon);
		path.push_back(current_point);
		current = came_from[current];
	}
	std::reverse(path.begin(), path.end());

	return path;
}

