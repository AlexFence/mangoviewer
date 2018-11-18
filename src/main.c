/**
 * MangoViewer
 * Copyright (C) 2018  Alex Fence
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <libmango.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include "imageview.h"
#include "imageview_controls.h"

//---------------------------------------------------------------------------------------
// Event Handlers
//---------------------------------------------------------------------------------------
static void open_file(GtkWidget *widget, gpointer user_data) {
    MangoImageView* image_view = MANGO_IMAGEVIEW(user_data);

    GtkWidget *toplevel = gtk_widget_get_toplevel(widget);
    GtkFileFilter *filter = gtk_file_filter_new ();
	GtkWidget *dialog = gtk_file_chooser_dialog_new("Open image",
	                                                GTK_WINDOW (toplevel),
	                                                GTK_FILE_CHOOSER_ACTION_OPEN,
	                                                GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
	                                                GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                                NULL);

	gtk_file_filter_add_pattern(filter, "*.mango");
    gtk_file_filter_set_name(filter, "mangofile");   
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);


    int error =  -1;

    switch (gtk_dialog_run(GTK_DIALOG(dialog))) {
		case GTK_RESPONSE_ACCEPT: {
            char *filename =  gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
            g_print("open: %s", filename);
            error = mango_img_view_load_file(image_view, filename); 
			break;
		}
		default:
			break;
	}

	gtk_widget_destroy (dialog);

    // handle errors
    if (error == 1) {
        // dialog could not open file
        show_error_dialog(toplevel, "could not open mangofile");
    } else if (error == 2) {
        // dialog read image
        show_error_dialog(toplevel, "could not read mangofile \n(probablly encrypted)");
    }
}

//---------------------------------------------------------------------------------------
// General Functions
//---------------------------------------------------------------------------------------
GtkWidget *create_menubar(GtkWidget *imgview) {
    GtkWidget *menubar = gtk_menu_bar_new();

    GtkWidget *filemenu = gtk_menu_new();
    GtkWidget *file = gtk_menu_item_new_with_label("File");
    GtkWidget *file_open = gtk_menu_item_new_with_label("Open");
    
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), file_open);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);

    g_signal_connect(G_OBJECT(file_open), "activate", G_CALLBACK(open_file), imgview);

    return menubar;
}

void show_error_dialog(GtkWidget *parent, const char *message) {
    GtkWidget *dialog; 
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_message_dialog_new(parent,
                                    flags,
                                    GTK_MESSAGE_ERROR,
                                    GTK_BUTTONS_CLOSE,
                                    message);
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

//---------------------------------------------------------------------------------------
// Main
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    
    GtkWidget *window;
    GtkWidget *menubar;
    GtkWidget *box;
    GtkWidget *overlay;
    GtkWidget *imageview;
    GtkWidget *imageview_controls;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Icon");
    gtk_window_set_default_size(GTK_WINDOW(window), 230, 150);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    overlay = gtk_overlay_new();

    imageview_controls = g_object_new(MANGO_TYPE_IMAGEVIEWCONTROLS, NULL);
    gtk_overlay_add_overlay(GTK_OVERLAY (overlay), imageview_controls);
    gtk_overlay_set_overlay_pass_through(GTK_OVERLAY (overlay), imageview_controls, TRUE);

    imageview = mango_img_view_new(MANGO_IMAGEVIEWCONTROLS (imageview_controls));
    gtk_widget_set_vexpand(imageview, true);
    gtk_container_add(GTK_CONTAINER (overlay), imageview);


    menubar = create_menubar(imageview);

    gtk_container_add(GTK_CONTAINER (box), menubar);
    gtk_container_add(GTK_CONTAINER (box), overlay);

    gtk_container_add(GTK_CONTAINER (window), box);

    gtk_widget_show_all(window);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
        
    gtk_main();

    return 0;
}
