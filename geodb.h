#ifndef GEODB_H
#define GEODB_H

#include "base_classes.h"
#include "geopoint.h"
#include "hashmap.h"
#include "support.h"

class GeoDatabase : public GeoDatabaseBase
{
public:
	GeoDatabase() : GeoDatabaseBase() {}
	virtual ~GeoDatabase() {}

	virtual bool load(const std::string& map_data_file);
	virtual bool get_poi_location(const std::string& poi, GeoPoint& point) const;
	virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
	virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;

private:
	HashMap<std::string> m_street_segments;
	HashMap<GeoPoint> m_POI;
	DubHashMap <GeoPoint> m_street_compliments;
};


#endif