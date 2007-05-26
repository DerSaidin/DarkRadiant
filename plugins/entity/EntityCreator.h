#ifndef ENTITYCREATOR_H_
#define ENTITYCREATOR_H_

#include "ientity.h"
#include "ieclass.h"

namespace entity {

class Doom3EntityCreator : 
	public EntityCreator
{
public:
	/** greebo: Creates an entity for the given EntityClass
	 */
	scene::INodePtr createEntity(IEntityClassPtr eclass);
	
	/** greebo: Sets the function to call when any keyvalue gets changed.
	 * 	
	 * Note: this currently points to the EntityInspector in the radiant core.
	 */
	void setKeyValueChangedFunc(KeyValueChangedFunc func);
	
	void setCounter(Counter* counter);

	/* Connect two entities using a "target" key.
	 */
	void connectEntities(const scene::Path& path, const scene::Path& targetPath);

	void printStatistics() const;
	
private:
	/** greebo: Creates the right entity for the entityclass.
	 */
	scene::INodePtr getEntityForEClass(IEntityClassPtr eclass);
};

} // namespace entity

#endif /*ENTITYCREATOR_H_*/
