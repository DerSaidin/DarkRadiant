#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>

namespace map
{

struct ProcEntity
{
	// The reference into the scenegraph
	IEntityNodePtr	mapEntity;

	Vector3			origin;

	//primitive_t*	primitives;
	//struct tree_s *		tree;

	//int					numAreas;
	//uArea_t *			areas;
};

/**
 * This class represents the processed data (entity models and shadow volumes)
 * as generated by the dmap compiler. Use the saveToFile() method to write the
 * data into the .proc file.
 */
class ProcFile
{
public:
	typedef std::vector<ProcEntity> ProcEntities;
	ProcEntities entities;

	void saveToFile(const std::string& path)
	{
		// TODO
	}

	static const char* const Extension()
	{
		return ".proc";
	}
};
typedef boost::shared_ptr<ProcFile> ProcFilePtr;

} // namespace