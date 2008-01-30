#ifndef DIFFICULTY_SETTINGS_H_
#define DIFFICULTY_SETTINGS_H_

#include "ieclass.h"
#include "entitylib.h"
#include <map>
#include <boost/shared_ptr.hpp>
#include <gtk/gtktreestore.h>

#include "Setting.h"

namespace {
	const std::string RKEY_DIFFICULTY_LEVELS("game/difficulty/numLevels");
	const std::string RKEY_DIFFICULTY_ENTITYDEF_DEFAULT("game/difficulty/defaultSettingsEclass");
	const std::string RKEY_DIFFICULTY_ENTITYDEF_MAP("game/difficulty/mapSettingsEclass");

	enum {
		COL_DESCRIPTION,
		COL_TEXTCOLOUR,
		NUM_SETTINGS_COLS,
	};
}

namespace difficulty {

class DifficultySettings
{
	// the difficulty level, these settings are referring to
	int _level;

	// The settings map associates classnames with spawnarg change records.
	// Multiple settings can be made for a single classname.
	typedef std::multimap<std::string, SettingPtr> SettingsMap;
	SettingsMap _settings;

	// This maps classnames to GtkTreeIters, for faster lookup
	typedef std::map<std::string, GtkTreeIter> TreeIterMap;
	TreeIterMap _iterMap;

public:
	// Define the difficulty level in the constructor
	DifficultySettings(int level);

	// Returns the level these settings are referring to
	int getLevel() const;

	// Empties the internal structures
	void clear();

	// Loads the data into the given treestore
	void updateTreeModel(GtkTreeStore* store);

	// Loads all settings (matching the internal _level) from the given entityDef.
	void parseFromEntityDef(const IEntityClassPtr& def);

	// Loads all settings (matching the internal _level) from the given entity.
	void parseFromMapEntity(Entity* entity);

private:
	/**
	 * greebo: Returns the TreeIter pointing to the tree element <className> in <store>.
	 * If the item is not yet existing, it gets inserted into the tree, according
	 * to its inheritance tree.
	 */
	GtkTreeIter findOrInsertClassname(GtkTreeStore* store, const std::string& className);

	/**
	 * greebo: Inserts the given classname into the treestore, using the
	 *         given <parent> iter as insertion point. If <parent> is NULL,
	 *         the entry is inserted at root level.
	 */
	GtkTreeIter insertClassName(GtkTreeStore* store, 
							 const std::string& className, 
							 GtkTreeIter* parent = NULL);

	// returns the parent eclass name for the given <className> or "" if no parent
	std::string getParentClass(const std::string& className);
};
typedef boost::shared_ptr<DifficultySettings> DifficultySettingsPtr;

} // namespace difficulty

#endif /* DIFFICULTY_SETTINGS_H_ */