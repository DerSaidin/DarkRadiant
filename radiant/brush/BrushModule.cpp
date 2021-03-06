/*
Copyright (C) 2001-2006, William Joseph.
All Rights Reserved.

This file is part of GtkRadiant.

GtkRadiant is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

GtkRadiant is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GtkRadiant; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "BrushModule.h"

#include "i18n.h"
#include "iradiant.h"

#include "itextstream.h"
#include "ifilter.h"
#include "igame.h"
#include "ilayer.h"
#include "ieventmanager.h"
#include "brush/BrushNode.h"
#include "brush/BrushClipPlane.h"
#include "brush/BrushVisit.h"
#include "brushmanip.h"

#include "registry/registry.h"
#include "ipreferencesystem.h"
#include "modulesystem/StaticModule.h"

// ---------------------------------------------------------------------------------------

void BrushModuleClass::constructPreferences() {
	// Add a page to the given group
	PreferencesPagePtr page = GlobalPreferenceSystem().getPage(_("Settings/Primitives"));

	// Add the default texture scale preference and connect it to the according registryKey
	// Note: this should be moved somewhere else, I think
	page->appendEntry(_("Default texture scale"), "user/ui/textures/defaultTextureScale");

	// The checkbox to enable/disable the texture lock option
	page->appendCheckBox("", _("Enable Texture Lock (for Brushes)"), "user/ui/brush/textureLock");
}

void BrushModuleClass::construct()
{
	Brush_registerCommands();

	Brush::m_maxWorldCoord = registry::getValue<float>("game/defaults/maxWorldCoord");
}

void BrushModuleClass::destroy()
{
	Brush::m_maxWorldCoord = 0;
}

void BrushModuleClass::keyChanged() 
{
	_textureLockEnabled = registry::getValue<bool>(RKEY_ENABLE_TEXTURE_LOCK);
}

bool BrushModuleClass::textureLockEnabled() const {
	return _textureLockEnabled;
}

void BrushModuleClass::setTextureLock(bool enabled)
{
    registry::setValue(RKEY_ENABLE_TEXTURE_LOCK, enabled);
}

void BrushModuleClass::toggleTextureLock() {
	setTextureLock(!textureLockEnabled());
}

// ------------ BrushCreator implementation --------------------------------------------

scene::INodePtr BrushModuleClass::createBrush()
{
	scene::INodePtr node(new BrushNode);

	// Move it to the active layer
	node->moveToLayer(GlobalLayerSystem().getActiveLayer());

	return node;
}

// RegisterableModule implementation
const std::string& BrushModuleClass::getName() const {
	static std::string _name(MODULE_BRUSHCREATOR);
	return _name;
}

const StringSet& BrushModuleClass::getDependencies() const {
	static StringSet _dependencies;

	if (_dependencies.empty()) {
		_dependencies.insert(MODULE_GAMEMANAGER);
		_dependencies.insert(MODULE_EVENTMANAGER);
		_dependencies.insert(MODULE_XMLREGISTRY);
		_dependencies.insert(MODULE_PREFERENCESYSTEM);
		_dependencies.insert(MODULE_UNDOSYSTEM);
	}

	return _dependencies;
}

void BrushModuleClass::initialiseModule(const ApplicationContext& ctx) {
	rMessage() << "BrushModuleClass::initialiseModule called." << std::endl;

	construct();

	_textureLockEnabled = registry::getValue<bool>(RKEY_ENABLE_TEXTURE_LOCK);

	GlobalRegistry().signalForKey(RKEY_ENABLE_TEXTURE_LOCK).connect(
        sigc::mem_fun(this, &BrushModuleClass::keyChanged)
    );

	// add the preference settings
	constructPreferences();
}

void BrushModuleClass::shutdownModule() {
	rMessage() << "BrushModuleClass::shutdownModule called." << std::endl;
	destroy();
}

// -------------------------------------------------------------------------------------

// Define a static BrushModule
module::StaticModule<BrushModuleClass> staticBrushModule;

// greebo: The accessor function for the brush module containing the static instance
// TODO: Change this to return a reference instead of a raw pointer
BrushModuleClass* GlobalBrush() {
	return staticBrushModule.getModule().get();
}
