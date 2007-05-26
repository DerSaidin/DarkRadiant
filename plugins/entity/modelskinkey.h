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

#if !defined(INCLUDED_MODELSKINKEY_H)
#define INCLUDED_MODELSKINKEY_H

#include "modelskin.h"

#include "os/path.h"
#include "moduleobserver.h"
#include "entitylib.h"
#include "traverselib.h"

namespace {
	
/**
 * Replace backslashes with forward slashes and strip of the file extension of
 * the provided token, and store the result in the provided string.
 * 
 * @token
 * The raw texture path.
 */
std::string parseTextureName(const std::string& token)
{
	return os::standardPath(token).substr(0, token.rfind("."));
}

} // local namespace

class ModelSkinKey : public ModuleObserver
{
  std::string m_name;
  ModelSkin* m_skin;
  Callback m_skinChangedCallback;

  ModelSkinKey(const ModelSkinKey&);
  ModelSkinKey operator=(const ModelSkinKey&);

  void construct()
  {
    m_skin = &GlobalModelSkinCache().capture(m_name.c_str());
    m_skin->attach(*this);
  }
  void destroy()
  {
    m_skin->detach(*this);
  }

public:
  ModelSkinKey(const Callback& skinChangedCallback) : m_skinChangedCallback(skinChangedCallback)
  {
    construct();
  }
  ~ModelSkinKey()
  {
    destroy();
  }
  ModelSkin& get() const
  {
    return *m_skin;
  }
  void skinChanged(const std::string& value)
  {
    destroy();
    m_name = parseTextureName(value);
    construct();
  }
  typedef MemberCaller1<ModelSkinKey, const std::string&, &ModelSkinKey::skinChanged> SkinChangedCaller;

  void realise()
  {
    m_skinChangedCallback();
  }
  void unrealise()
  {
  }
};

class InstanceSkinChanged : public scene::Instantiable::Visitor
{
public:
  void visit(scene::Instance& instance) const
  {
    //\todo don't do this for instances that are not children of the entity setting the skin
    SkinnedModel* skinned = dynamic_cast<SkinnedModel*>(&instance);
    if(skinned != 0)
    {
      skinned->skinChanged();
    }
  }
};

inline void Node_modelSkinChanged(scene::INodePtr node)
{
  scene::InstantiablePtr instantiable = Node_getInstantiable(node);
  ASSERT_NOTNULL(instantiable);
  instantiable->forEachInstance(InstanceSkinChanged());
}

#endif
