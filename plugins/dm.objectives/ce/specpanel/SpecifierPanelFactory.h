#ifndef SPECIFIERPANELFACTORY_H_
#define SPECIFIERPANELFACTORY_H_

#include "SpecifierPanel.h"
#include "../../Specifier.h"

#include <map>

namespace objectives
{

namespace ce
{

/**
 * Factory class for creating SpecifierPanel subclasses for a particular type
 * of Specifier.
 */
class SpecifierPanelFactory
{
	// Static map instance
	typedef std::map<objectives::Specifier, SpecifierPanelPtr> PanelMap;
	static PanelMap& getMap();
	
public:
	
	/**
	 * Register a SpecifierPanel subclass.
	 * 
	 * This method is invoked by SpecifierPanel subclasses to register
	 * themselves for virtual construction. Once a SpecifierPanel is registered
	 * it can be created and returned by the SpecifierPanelFactory::create()
	 * method based on its Specifier type.
	 * 
	 * @param type
	 * The Specifier type that this panel will edit.
	 * 
	 * @param cls
	 * The SpecifierPanel subclass to register.
	 */
	static void registerType(const objectives::Specifier& type, 
							 SpecifierPanelPtr cls);

	/**
	 * Create a SpecifierPanel to edit the given Specifier type.
	 * 
	 * @param type
	 * The type of Specifier for which a SpecifierPanel must be created.
	 * 
	 * @return
	 * Shared pointer to a SpecifierPanel which contains widgets to edit this
	 * Specifier type. If no SpecifierPanl has been registered for this type,
	 * a NULL shared_ptr is returned.
	 */
	static SpecifierPanelPtr create(objectives::Specifier& type);
};

}

}

#endif /*SPECIFIERPANELFACTORY_H_*/