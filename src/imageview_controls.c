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

#include "imageview_controls.h"
#include <stdbool.h>
#include "imageview_controls_style.h";

struct _MangoImageViewControls {
    GtkBin parent_instance;
    GtkBox *container;
    GtkButton *next_button;
    GtkButton *prev_button;
};

G_DEFINE_TYPE(MangoImageViewControls, mango_img_view_controls, GTK_TYPE_BIN)

enum {
    NEXT_IMAGE,
    PREVIOUS_IMAGE,
    LAST_SIGNAL,
};

static guint signals[LAST_SIGNAL];

static void mango_img_view_controls_class_init(MangoImageViewControlsClass *class) {
    signals[NEXT_IMAGE] = g_signal_new("next_image", 
                                       G_TYPE_FROM_CLASS(class),
                                       G_SIGNAL_RUN_LAST,
                                       0,
                                       NULL, NULL, NULL,
                                       G_TYPE_NONE, 0);

    signals[PREVIOUS_IMAGE] = g_signal_new("previous_image", 
                                           G_TYPE_FROM_CLASS(class),
                                           G_SIGNAL_RUN_LAST,
                                           0,
                                           NULL, NULL, NULL,
                                           G_TYPE_NONE, 0);

}

static void next_button_clicked(GtkWidget *widget, gpointer user_data) {
    // forward the the click event to the handler in the imageview object
    g_signal_emit(MANGO_IMAGEVIEWCONTROLS (user_data), signals[NEXT_IMAGE], 0);
}

static void previous_button_clicked(GtkWidget *widget, gpointer user_data) {
    // forward the the click event to the handler in the imageview object
    g_signal_emit(MANGO_IMAGEVIEWCONTROLS (user_data), signals[PREVIOUS_IMAGE], 0);
}

static void handle_events(MangoImageViewControls *self) {
    g_signal_connect(G_OBJECT (self->next_button), "clicked", next_button_clicked, self);
    g_signal_connect(G_OBJECT (self->prev_button), "clicked", previous_button_clicked, self);
}

static void handle_styles(MangoImageViewControls *self) {
    GdkScreen *screen = gdk_screen_get_default();
    GtkCssProvider *provider = gtk_css_provider_new();

    gtk_css_provider_load_from_data(provider, 
                                    src_imageview_controls_css,
                                    src_imageview_controls_css_len, NULL);
    
    gtk_style_context_add_provider_for_screen(screen, provider, 799);

    GtkStyleContext *next = gtk_widget_get_style_context(GTK_WIDGET (self->next_button));
    gtk_style_context_add_class(next, "overlay-control");

    GtkStyleContext *prev = gtk_widget_get_style_context(GTK_WIDGET (self->prev_button));
    gtk_style_context_add_class(prev, "overlay-control");
}

static void mango_img_view_controls_init(MangoImageViewControls *self) {
    self->container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    self->next_button = GTK_BUTTON(gtk_button_new_with_label("next"));
    self->prev_button = GTK_BUTTON(gtk_button_new_with_label("previous"));

    // unset size requests
    // make the buttons as small as possible
    // next
    gtk_widget_set_size_request(GTK_WIDGET (self->next_button), -1, -1);
    gtk_widget_set_vexpand_set(GTK_WIDGET (self->next_button), false);
    gtk_widget_set_valign(GTK_WIDGET (self->next_button), GTK_ALIGN_CENTER);
    // previous
    gtk_widget_set_size_request(GTK_WIDGET (self->prev_button), -1, -1);
    gtk_widget_set_vexpand_set(GTK_WIDGET (self->prev_button), false);
    gtk_widget_set_valign(GTK_WIDGET (self->prev_button), GTK_ALIGN_CENTER);

    handle_styles(self);
    handle_events(self);

    gtk_box_pack_start(self->container, self->prev_button, false, true, 0);
    gtk_box_pack_end(self->container, self->next_button, false, true, 0);

    gtk_container_add(GTK_CONTAINER (&self->parent_instance), self->container);
}
