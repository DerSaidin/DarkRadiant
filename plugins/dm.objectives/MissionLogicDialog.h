#ifndef MISSION_LOGIC_DIALOG_H_
#define MISSION_LOGIC_DIALOG_H_

#include <map>
#include <vector>
#include <string>

#include "LogicEditor.h"

#include <gtkutil/window/BlockingTransientWindow.h>

namespace objectives {

/* FORWARD DECLS */
class ObjectiveEntity;

/**
 * Dialog for displaying and editing the mission success/failure logic
 * which can optionally be different for each difficulty setting.
 */
class MissionLogicDialog : 
	public gtkutil::BlockingTransientWindow
{
	// Widgets map
	std::map<int, GtkWidget*> _widgets;

	// A container for the logic editors of the various difficulty levels
	typedef std::map<int, LogicEditorPtr> LogicEditorMap;
	LogicEditorMap _logicEditors;

	// The objective entity we're working on
	ObjectiveEntity& _objectiveEnt;
	
public:
	/**
	 * Constructor creates widgets.
	 * 
	 * @param parent
	 * The parent window for which this dialog should be a transient.
	 * 
	 * @param objectiveEnt
	 * The Objective Entity object for which the logic should be edited.
	 */
	MissionLogicDialog(GtkWindow* parent, ObjectiveEntity& objectiveEnt);

private:

	// Helper methods
	GtkWidget* createButtons();

	// Creates one logic editor for each difficulty level plus the default one
	void createLogicEditors();

	// Populate the logic editors from the objective entity object
	void populateLogicEditors();

	// Writes the contents of the widgets to the objective entity
	void save();

	// GTK CALLBACKS
	static void _onOK(GtkWidget*, MissionLogicDialog*);
	static void _onCancel(GtkWidget*, MissionLogicDialog*);
};

} // namespace objectives

#endif /* MISSION_LOGIC_DIALOG_H_ */