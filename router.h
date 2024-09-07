#ifndef ROUTER_H
#define ROUTER_H

#include <vector>
#include <cmath>
#include <queue>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cfloat>

#include "base_classes.h"
#include "geopoint.h"

class Router : public RouterBase
{
public:
	Router(const GeoDatabaseBase& geo_db) : m_geo_db(geo_db) {}
	virtual ~Router() {}
	virtual std::vector<GeoPoint> route(const GeoPoint& pt1,
		const GeoPoint& pt2) const;

private:
	const GeoDatabaseBase& m_geo_db;

	//STRUCT FOR PRIORITY QUEUE
	template<typename T, typename P>
	struct PriorityQueue {
		typedef std::pair<P, T> PQElement;
		std::priority_queue<PQElement, std::vector<PQElement>,
			std::greater<PQElement>> elements;

		inline bool empty() const {
			return elements.empty();
		}

		inline void put(T item, P priority) {
			elements.emplace(priority, item);
		}

		T get() {
			T best_item = elements.top().second;
			elements.pop();
			return best_item;
		}
	};

	//HELPER FUNCTION TO GET THE DISTANCE BETWEEN TWO POINTS
	double distance(const GeoPoint& pt1, const GeoPoint& pt2) const;
};

inline
double Router::distance(const GeoPoint& pt1, const GeoPoint& pt2) const
{
	double dx = pt1.latitude - pt2.latitude;
	double dy = pt1.longitude - pt2.longitude;
	return std::sqrt(dx * dx + dy * dy);
}

#endif
