#include "ModelInfoTab.h"

#include <gtk/gtk.h>

#include "string/string.h"
#include "gtkutil/ScrolledFrame.h"
#include "gtkutil/TextColumn.h"

namespace ui {

	namespace {
		const std::string TAB_NAME("Models");
		const std::string TAB_ICON("model16green.png");
	   	
	   	enum {
	   		MODEL_COL,
	   		FACE_COUNT_COL,
			PATCH_COUNT_COL,
	   		NUM_COLS
	   	};
	}

ModelInfoTab::ModelInfoTab() :
	_widget(gtk_vbox_new(FALSE, 6))
{
	// Create all the widgets
	populateTab();
}

GtkWidget* ModelInfoTab::getWidget() {
	return _widget;
}

std::string ModelInfoTab::getLabel() {
	return TAB_NAME;
}

std::string ModelInfoTab::getIconName() {
	return TAB_ICON;
}

void ModelInfoTab::populateTab() {
	// Set the outer space of the vbox
	gtk_container_set_border_width(GTK_CONTAINER(_widget), 12);
	
	/*// Create the list store that contains the shader => count, count map 
	_listStore = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);

	// Create the treeview and pack two columns into it
	_treeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(_listStore));
	gtk_tree_view_set_headers_clickable(GTK_TREE_VIEW(_treeView), TRUE);

	GtkTreeViewColumn* shaderCol = gtkutil::TextColumn("Shader", SHADER_COL);
	gtk_tree_view_column_set_sort_column_id(shaderCol, SHADER_COL);
	
	GtkTreeViewColumn* faceCountCol = gtkutil::TextColumn("Faces", FACE_COUNT_COL);
	gtk_tree_view_column_set_sort_column_id(faceCountCol, FACE_COUNT_COL);

	GtkTreeViewColumn* patchCountCol = gtkutil::TextColumn("Patches", PATCH_COUNT_COL);
	gtk_tree_view_column_set_sort_column_id(patchCountCol, PATCH_COUNT_COL);

	gtk_tree_view_append_column(GTK_TREE_VIEW(_treeView), shaderCol);
	gtk_tree_view_append_column(GTK_TREE_VIEW(_treeView), faceCountCol);
	gtk_tree_view_append_column(GTK_TREE_VIEW(_treeView), patchCountCol);
	
    gtk_box_pack_start(GTK_BOX(_widget), gtkutil::ScrolledFrame(_treeView), TRUE, TRUE, 0);
    
    // Populate the liststore with the entity count information
    for (map::ShaderBreakdown::Map::const_iterator i = _shaderBreakdown.begin(); 
		 i != _shaderBreakdown.end(); 
		 ++i)
	{
		GtkTreeIter iter;
		gtk_list_store_append(_listStore, &iter);
		gtk_list_store_set(_listStore, &iter, 
						   SHADER_COL, i->first.c_str(),
						   FACE_COUNT_COL, i->second.faceCount, 
						   PATCH_COUNT_COL, i->second.patchCount, 
						   -1);
	}*/
}

} // namespace ui