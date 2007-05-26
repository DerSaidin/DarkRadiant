/*
Copyright (C) 1999-2006 Id Software, Inc. and contributors.
For a list of contributors, see the accompanying CONTRIBUTORS file.

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

#include "csg.h"

#include "debugging/debugging.h"

#include <list>

#include "math/Plane3.h"
#include "map.h"
#include "brushmanip.h"
#include "brush/BrushVisit.h"
#include "brush/BrushNode.h"
#include "igrid.h"

void Face_makeBrush(Face& face, const Brush& brush, BrushVector& out, float offset)
{
  if(face.contributes())
  {
    out.push_back(new Brush(brush));
    Face* newFace = out.back()->addFace(face);
    if(newFace != 0)
    {
      newFace->flipWinding();
      newFace->getPlane().offset(offset);
      newFace->planeChanged();
    }
  }
}

class FaceMakeBrush
{
  const Brush& brush;
  BrushVector& out;
  float offset;
public:
  FaceMakeBrush(const Brush& brush, BrushVector& out, float offset)
    : brush(brush), out(out), offset(offset)
  {
  }
  void operator()(Face& face) const
  {
    Face_makeBrush(face, brush, out, offset);
  }
};

void Brush_makeHollow(const Brush& brush, BrushVector& out, float offset)
{
  Brush_forEachFace(brush, FaceMakeBrush(brush, out, offset));
}

class BrushHollowSelectedWalker : public scene::Graph::Walker
{
  float m_offset;
public:
  BrushHollowSelectedWalker(float offset)
    : m_offset(offset)
  {
  }
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    if(path.top()->visible())
    {
      Brush* brush = Node_getBrush(path.top());
      if(brush != 0
        && Instance_getSelectable(instance)->isSelected()
        && path.size() > 1)
      {
        BrushVector out;
        Brush_makeHollow(*brush, out, m_offset);
        for(BrushVector::const_iterator i = out.begin(); i != out.end(); ++i)
        {
          (*i)->removeEmptyFaces();
          scene::INodePtr node(new BrushNode());
          Node_getBrush(node)->copy(*(*i));
          delete (*i);
          Node_getTraversable(path.parent())->insert(node);
        }
      }
    }
    return true;
  }
};

typedef std::list<Brush*> brushlist_t;

class BrushGatherSelected : public scene::Graph::Walker
{
  BrushVector& m_brushlist;
public:
  BrushGatherSelected(BrushVector& brushlist)
    : m_brushlist(brushlist)
  {
  }
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    if(path.top()->visible())
    {
      Brush* brush = Node_getBrush(path.top());
      if(brush != 0
        && Instance_getSelectable(instance)->isSelected())
      {
        m_brushlist.push_back(brush);
      }
    }
    return true;
  }
};

class BrushDeleteSelected : public scene::Graph::Walker
{
public:
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    return true;
  }
  void post(const scene::Path& path, scene::Instance& instance) const
  {
    if(path.top()->visible())
    {
      Brush* brush = Node_getBrush(path.top());
      if(brush != 0
        && Instance_getSelectable(instance)->isSelected()
        && path.size() > 1)
      {
        Path_deleteTop(path);
      }
    }
  }
};

void Scene_BrushMakeHollow_Selected(scene::Graph& graph)
{
  GlobalSceneGraph().traverse(BrushHollowSelectedWalker(GlobalGrid().getGridSize()));
  GlobalSceneGraph().traverse(BrushDeleteSelected());
}

/*
=============
CSG_MakeHollow
=============
*/

void CSG_MakeHollow (void)
{
  UndoableCommand undo("brushHollow");

  Scene_BrushMakeHollow_Selected(GlobalSceneGraph());

  SceneChangeNotify();
}

template<typename Type>
class RemoveReference
{
public:
  typedef Type type;
};

template<typename Type>
class RemoveReference<Type&>
{
public:
  typedef Type type;
};

template<typename Functor>
class Dereference
{
  const Functor& functor;
public:
  typedef typename RemoveReference<typename Functor::first_argument_type>::type* first_argument_type;
  typedef typename Functor::result_type result_type;
  Dereference(const Functor& functor) : functor(functor)
  {
  }
  result_type operator()(first_argument_type firstArgument) const
  {
    return functor(*firstArgument);
  }
};

template<typename Functor>
inline Dereference<Functor> makeDereference(const Functor& functor)
{
  return Dereference<Functor>(functor);
}

typedef Face* FacePointer;
const FacePointer c_nullFacePointer = 0;

template<typename Predicate>
Face* Brush_findIf(const Brush& brush, const Predicate& predicate)
{
  Brush::const_iterator i = std::find_if(brush.begin(), brush.end(), makeDereference(predicate));
  return i == brush.end() ? c_nullFacePointer : *i; // uses c_nullFacePointer instead of 0 because otherwise gcc 4.1 attempts conversion to int
}

template<typename Caller>
class BindArguments1
{
  typedef typename Caller::second_argument_type FirstBound;
  FirstBound firstBound;
public:
  typedef typename Caller::result_type result_type;
  typedef typename Caller::first_argument_type first_argument_type;
  BindArguments1(FirstBound firstBound)
    : firstBound(firstBound)
  {
  }
  result_type operator()(first_argument_type firstArgument) const
  {
    return Caller::call(firstArgument, firstBound);
  }
};

template<typename Caller>
class BindArguments2
{
  typedef typename Caller::second_argument_type FirstBound;
  typedef typename Caller::third_argument_type SecondBound;
  FirstBound firstBound;
  SecondBound secondBound;
public:
  typedef typename Caller::result_type result_type;
  typedef typename Caller::first_argument_type first_argument_type;
  BindArguments2(FirstBound firstBound, SecondBound secondBound)
    : firstBound(firstBound), secondBound(secondBound)
  {
  }
  result_type operator()(first_argument_type firstArgument) const
  {
    return Caller::call(firstArgument, firstBound, secondBound);
  }
};

template<typename Caller, typename FirstBound, typename SecondBound>
BindArguments2<Caller> bindArguments(const Caller& caller, FirstBound firstBound, SecondBound secondBound)
{
  return BindArguments2<Caller>(firstBound, secondBound);
}

inline bool Face_testPlane(const Face& face, const Plane3& plane, bool flipped)
{
  return face.contributes() && !Winding_TestPlane(face.getWinding(), plane, flipped);
}
typedef Function3<const Face&, const Plane3&, bool, bool, Face_testPlane> FaceTestPlane;



/// \brief Returns true if
/// \li !flipped && brush is BACK or ON
/// \li flipped && brush is FRONT or ON
bool Brush_testPlane(const Brush& brush, const Plane3& plane, bool flipped)
{
  brush.evaluateBRep();
#if 1
  for(Brush::const_iterator i(brush.begin()); i != brush.end(); ++i)
  {
    if(Face_testPlane(*(*i), plane, flipped))
    {
      return false;
    }
  }
  return true;
#else
  return Brush_findIf(brush, bindArguments(FaceTestPlane(), makeReference(plane), flipped)) == 0;
#endif
}

BrushSplitType Brush_classifyPlane(const Brush& brush, const Plane3& plane)
{
  brush.evaluateBRep();
  BrushSplitType split;
  for(Brush::const_iterator i(brush.begin()); i != brush.end(); ++i)
  {
    if((*i)->contributes())
    {
      split += Winding_ClassifyPlane((*i)->getWinding(), plane);
    }
  }
  return split;
}

bool Brush_subtract(const Brush& brush, const Brush& other, BrushVector& ret_fragments)
{
  if(aabb_intersects_aabb(brush.localAABB(), other.localAABB()))
  {
    BrushVector fragments;
    fragments.reserve(other.size());
    Brush back(brush);

    for(Brush::const_iterator i(other.begin()); i != other.end(); ++i)
    {
      if((*i)->contributes())
      {
        BrushSplitType split = Brush_classifyPlane(back, (*i)->plane3());
        if(split.counts[ePlaneFront] != 0
          && split.counts[ePlaneBack] != 0)
        {
          fragments.push_back(new Brush(back));
          Face* newFace = fragments.back()->addFace(*(*i));
          if(newFace != 0)
          {
            newFace->flipWinding();
          }
          back.addFace(*(*i));
        }
        else if(split.counts[ePlaneBack] == 0)
        {
          for(BrushVector::iterator i = fragments.begin(); i != fragments.end(); ++i)
          {
            delete(*i);
          }
          return false;
        }
      }
    }
    ret_fragments.insert(ret_fragments.end(), fragments.begin(), fragments.end());
    return true;
  }
  return false;
}

class SubtractBrushesFromUnselected : public scene::Graph::Walker
{
  const BrushVector& m_brushlist;
  std::size_t& m_before;
  std::size_t& m_after;
public:
  SubtractBrushesFromUnselected(const BrushVector& brushlist, std::size_t& before, std::size_t& after)
    : m_brushlist(brushlist), m_before(before), m_after(after)
  {
  }
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    return true;
  }
  void post(const scene::Path& path, scene::Instance& instance) const
  {
    if(path.top()->visible())
    {
      Brush* brush = Node_getBrush(path.top());
      if(brush != 0
        && !Instance_getSelectable(instance)->isSelected())
      {
        BrushVector buffer[2];
        bool swap = false;
        Brush* original = new Brush(*brush);
        buffer[static_cast<std::size_t>(swap)].push_back(original);
        
        {
          for(BrushVector::const_iterator i(m_brushlist.begin()); i != m_brushlist.end(); ++i)
          {
            for(BrushVector::iterator j(buffer[static_cast<std::size_t>(swap)].begin()); j != buffer[static_cast<std::size_t>(swap)].end(); ++j)
            {
              if(Brush_subtract(*(*j), *(*i), buffer[static_cast<std::size_t>(!swap)]))
              {
                delete (*j);
              }
              else
              {
                buffer[static_cast<std::size_t>(!swap)].push_back((*j));
              }
            }
            buffer[static_cast<std::size_t>(swap)].clear();
            swap = !swap;
          }
        }

        BrushVector& out = buffer[static_cast<std::size_t>(swap)];

        if(out.size() == 1 && out.back() == original)
        {
          delete original;
        }
        else
        {
          ++m_before;
          for(BrushVector::const_iterator i = out.begin(); i != out.end(); ++i)
          {
            ++m_after;
            scene::INodePtr node(new BrushNode());
            (*i)->removeEmptyFaces();
            ASSERT_MESSAGE(!(*i)->empty(), "brush left with no faces after subtract");
            Node_getBrush(node)->copy(*(*i));
            delete (*i);
            Node_getTraversable(path.parent())->insert(node);
          }
          Path_deleteTop(path);
        }
      }
    }
  }
};

void CSG_Subtract()
{
  BrushVector selected_brushes;
  GlobalSceneGraph().traverse(BrushGatherSelected(selected_brushes));

  if (selected_brushes.empty())
  {
    globalOutputStream() << "CSG Subtract: No brushes selected.\n";
  }
  else
  {
    globalOutputStream() << "CSG Subtract: Subtracting " << Unsigned(selected_brushes.size()) << " brushes.\n";

    UndoableCommand undo("brushSubtract");

    // subtract selected from unselected
    std::size_t before = 0;
    std::size_t after = 0;
    GlobalSceneGraph().traverse(SubtractBrushesFromUnselected(selected_brushes, before, after));
    globalOutputStream() << "CSG Subtract: Result: "
      << Unsigned(after) << " fragment" << (after == 1 ? "" : "s")
      << " from " << Unsigned(before) << " brush" << (before == 1? "" : "es") << ".\n";

    SceneChangeNotify();
  }
}

class BrushSplitByPlaneSelected : public scene::Graph::Walker
{
  const Vector3& m_p0;
  const Vector3& m_p1;
  const Vector3& m_p2;
  const std::string& m_shader;
  const TextureProjection& m_projection;
  EBrushSplit m_split;
public:
  BrushSplitByPlaneSelected(const Vector3& p0, const Vector3& p1, const Vector3& p2, const std::string& shader, const TextureProjection& projection, EBrushSplit split)
    : m_p0(p0), m_p1(p1), m_p2(p2), m_shader(shader), m_projection(projection), m_split(split)
  {
  }
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    return true; 
  }
  void post(const scene::Path& path, scene::Instance& instance) const
  {
    if(path.top()->visible())
    {
      Brush* brush = Node_getBrush(path.top());
      if(brush != 0
        && Instance_getSelectable(instance)->isSelected())
      {
        Plane3 plane(m_p0, m_p1, m_p2);
        if(plane.isValid())
        {
          BrushSplitType split = Brush_classifyPlane(*brush, m_split == eFront ? -plane : plane);
          if(split.counts[ePlaneBack] && split.counts[ePlaneFront])
          {
            // the plane intersects this brush
            if(m_split == eFrontAndBack)
            {
              scene::INodePtr node(new BrushNode());
              Brush* fragment = Node_getBrush(node);
              fragment->copy(*brush);
              Face* newFace = fragment->addPlane(m_p0, m_p1, m_p2, m_shader, m_projection);
              if(newFace != 0 && m_split != eFront)
              {
                newFace->flipWinding();
              }
              fragment->removeEmptyFaces();
              ASSERT_MESSAGE(!fragment->empty(), "brush left with no faces after split");

              Node_getTraversable(path.parent())->insert(node);
              {
                scene::Path fragmentPath = path;
                fragmentPath.top() = node;
                selectPath(fragmentPath, true);
              }
            }

            Face* newFace = brush->addPlane(m_p0, m_p1, m_p2, m_shader, m_projection);
            if(newFace != 0 && m_split == eFront)
            {
              newFace->flipWinding();
            }
            brush->removeEmptyFaces();
            ASSERT_MESSAGE(!brush->empty(), "brush left with no faces after split");
          }
          else
            // the plane does not intersect this brush
          if(m_split != eFrontAndBack && split.counts[ePlaneBack] != 0)
          {
            // the brush is "behind" the plane
            Path_deleteTop(path);
          }
        }
      }
    }
  }
};

void Scene_BrushSplitByPlane(const Vector3 planePoints[3], const std::string& shader, EBrushSplit split) { 
  TextureProjection projection;
  projection.constructDefault();
  GlobalSceneGraph().traverse(BrushSplitByPlaneSelected(planePoints[0], planePoints[1], planePoints[2], shader, projection, split));
  SceneChangeNotify();
}


class BrushInstanceSetClipPlane : public scene::Graph::Walker
{
  Plane3 m_plane;
public:
  BrushInstanceSetClipPlane(const Plane3& plane)
    : m_plane(plane)
  {
  }
  bool pre(const scene::Path& path, scene::Instance& instance) const
  {
    BrushInstance* brush = Instance_getBrush(instance);
    if(brush != 0
      && path.top()->visible()
      && brush->isSelected())
    {
      BrushInstance& brushInstance = *brush;
      brushInstance.setClipPlane(m_plane);
    }
    return true; 
  }
};

void Scene_BrushSetClipPlane(const Plane3& plane) {
  GlobalSceneGraph().traverse(BrushInstanceSetClipPlane(plane));
}

/*
=============
CSG_Merge
=============
*/
bool Brush_merge(Brush& brush, const BrushVector& in, bool onlyshape)
{
  // gather potential outer faces 

  {
    typedef std::vector<const Face*> Faces;
    Faces faces;
    for(BrushVector::const_iterator i(in.begin()); i != in.end(); ++i)
    {
      (*i)->evaluateBRep();
      for(Brush::const_iterator j((*i)->begin()); j != (*i)->end(); ++j)
      {
        if(!(*j)->contributes())
        {
          continue;
        }

        const Face& face1 = *(*j);

        bool skip = false;
        // test faces of all input brushes
        //!\todo SPEEDUP: Flag already-skip faces and only test brushes from i+1 upwards.
        for(BrushVector::const_iterator k(in.begin()); !skip && k != in.end(); ++k)
        {
          if(k != i) // don't test a brush against itself
          {
            for(Brush::const_iterator l((*k)->begin()); !skip && l != (*k)->end(); ++l)
            {
              const Face& face2 = *(*l);

              // face opposes another face
              if (face1.plane3() == -face2.plane3())
              {
                // skip opposing planes
                skip  = true;
                break;
              }
            }
          }
        }

        // check faces already stored
        for(Faces::const_iterator m = faces.begin(); !skip && m != faces.end(); ++m)
        {
          const Face& face2 = *(*m);

          // face equals another face
          if (face1.plane3() == face2.plane3())
          {
            //if the texture/shader references should be the same but are not
            if (!onlyshape && !shader_equal(face1.getShader().getShader(), face2.getShader().getShader()))
            {
              return false;
            }
            // skip duplicate planes
            skip = true;
            break;
          }

          // face1 plane intersects face2 winding or vice versa
          if (Winding_PlanesConcave(face1.getWinding(), face2.getWinding(), face1.plane3(), face2.plane3()))
          {
            // result would not be convex
            return false;
          }
        }

        if(!skip)
        {
          faces.push_back(&face1);
        }
      }
    }
    for(Faces::const_iterator i = faces.begin(); i != faces.end(); ++i)
    {
      if(!brush.addFace(*(*i)))
      {
        // result would have too many sides
        return false;
      }
    }
  }

  brush.removeEmptyFaces();

  return true;
}

void CSG_Merge(void)
{
  BrushVector selected_brushes;

  // remove selected
  GlobalSceneGraph().traverse(BrushGatherSelected(selected_brushes));

  if (selected_brushes.empty())
  {
    globalOutputStream() << "CSG Merge: No brushes selected.\n";
    return;
  }

  if (selected_brushes.size() < 2)
  {
    globalOutputStream() << "CSG Merge: At least two brushes have to be selected.\n";
    return;
  }

  globalOutputStream() << "CSG Merge: Merging " << Unsigned(selected_brushes.size()) << " brushes.\n";

  UndoableCommand undo("brushMerge");

  scene::Path merged_path = GlobalSelectionSystem().ultimateSelected().path();

  scene::INodePtr node(new BrushNode());
  Brush* brush = Node_getBrush(node);
  // if the new brush would not be convex
  if(!Brush_merge(*brush, selected_brushes, true))
  {
    globalOutputStream() << "CSG Merge: Failed - result would not be convex.\n";
  }
  else
  {
    ASSERT_MESSAGE(!brush->empty(), "brush left with no faces after merge");

    // free the original brushes
    GlobalSceneGraph().traverse(BrushDeleteSelected());

    merged_path.pop();
    Node_getTraversable(merged_path.top())->insert(node);
    merged_path.push(node);

    selectPath(merged_path, true);

    globalOutputStream() << "CSG Merge: Succeeded.\n";
    SceneChangeNotify();
  }
}
