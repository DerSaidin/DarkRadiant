#pragma once

#include "Manipulator.h"
#include "Renderables.h"
#include "Pivot2World.h"
#include "Manipulatables.h"
#include "selectionlib.h"

/**
 * Manipulator for performing axis-aligned translations.
 */
class TranslateManipulator
: public Manipulator
{
private:
  TranslateFree _translateFree;
  TranslateAxis _translateAxis;
  RenderableArrowLine _arrowX;
  RenderableArrowLine _arrowY;
  RenderableArrowLine _arrowZ;
  RenderableArrowHead _arrowHeadX;
  RenderableArrowHead _arrowHeadY;
  RenderableArrowHead _arrowHeadZ;
  RenderableQuad _quadScreen;
  SelectableBool _selectableX;
  SelectableBool _selectableY;
  SelectableBool _selectableZ;
  SelectableBool _selectableScreen;
  Pivot2World _pivot;
public:
  static ShaderPtr _stateWire;
  static ShaderPtr _stateFill;

  // Constructor
  TranslateManipulator(Translatable& translatable, std::size_t segments, float length);

  void UpdateColours();
  bool manipulator_show_axis(const Pivot2World& pivot, const Vector3& axis);

  void render(RenderableCollector& collector, const VolumeTest& volume, const Matrix4& pivot2world);
  void testSelect(const View& view, const Matrix4& pivot2world);
  ManipulatorComponent* getActiveComponent();

  void setSelected(bool select);
  bool isSelected() const;
}; // class TranslateManipulator

