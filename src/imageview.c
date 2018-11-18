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

#include "imageview.h"
#include <stdbool.h>
#include <libmango.h>

struct _MangoImageView {
    GtkBin parent_instance;
    GdkPixbuf *pixbuf;
    GtkScrolledWindow *scrollwindow;
    GtkImage *gtk_img;
    MangoFile mangofile;
    int current_img;
};

G_DEFINE_TYPE(MangoImageView, mango_img_view, GTK_TYPE_BIN)

static void mango_img_view_class_init(MangoImageViewClass *class) {

}

static void mango_img_view_create_initials(MangoImageView *self) {
    self->scrollwindow = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new(NULL, NULL));
    gtk_widget_set_vexpand_set(self->scrollwindow, true);
    self->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, 0, 8, 0, 0);
    self->gtk_img =  GTK_IMAGE (gtk_image_new_from_pixbuf(self->pixbuf));
    self->current_img = -1;
    self->mangofile = NULL;
}

static void mango_img_view_init(MangoImageView *self) {
    mango_img_view_create_initials(self);

    gtk_container_add(GTK_CONTAINER (self->scrollwindow), GTK_WIDGET (self->gtk_img));
    gtk_container_add(GTK_CONTAINER (&self->parent_instance), self->scrollwindow);
}


static void mango_img_view_next(GtkWidget *widget, gpointer user_data) {
    MangoImageView *self = MANGO_IMAGEVIEW (user_data);
    
    if (self->current_img < mango_img_view_image_count(self) - 1) {
        mango_img_view_change_image(self, self->current_img + 1);
    }
}

static void mango_img_view_previous(GtkWidget *widget, gpointer user_data) {
    MangoImageView *self = MANGO_IMAGEVIEW (user_data);
    
    if (self->current_img > 0) {
        mango_img_view_change_image(self, self->current_img - 1);
    }
}

MangoImageView *mango_img_view_new(MangoImageViewControls *controls) {
    MangoImageView *instance = g_object_new(MANGO_TYPE_IMAGEVIEW, NULL);

    g_signal_connect(G_OBJECT(controls), "next_image", mango_img_view_next, instance);
    g_signal_connect(G_OBJECT(controls), "previous_image", mango_img_view_previous, instance);

    return instance;
}

int mango_img_view_load_file(MangoImageView *self, char *path) {
    int err = 99;
    self->mangofile = mangofile_open(path, &err);

    if (err != 0) {
       return 1; 
    }

    self->current_img = 0;
    MangoImage mimg = mangofile_get_image(self->mangofile, self->current_img);
    mangoimg_uncompress(mimg);
    ImageData img_data = mangoimg_get_image_data(mimg);

    GError *error = NULL;
    GInputStream *my_cool_g_stream = g_memory_input_stream_new_from_data(img_data.pointer, img_data.length, NULL);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(my_cool_g_stream, NULL, &error);

    if (error != NULL) {
        return 2;
    }

    gtk_image_set_from_pixbuf(self->gtk_img, pixbuf);

    return 0;
}

int mango_img_view_image_count(MangoImageView *self) {
    if (self->mangofile == NULL) {
        return -1;
    } else {
        return mangofile_get_image_count(self->mangofile);
    }
} 

void mango_img_view_change_image(MangoImageView *self, int index) {
    self->current_img = index;

    MangoImage mimg = mangofile_get_image(self->mangofile, self->current_img);
    mangoimg_uncompress(mimg);
    ImageData img_data = mangoimg_get_image_data(mimg);

    GError *error = NULL;
    GInputStream *my_cool_g_stream = g_memory_input_stream_new_from_data(img_data.pointer, img_data.length, NULL);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream(my_cool_g_stream, NULL, &error);

    if (error != NULL) {
        // TODO display a place holder screen if the data is encrypted 
        //      or unreadable for some other reason
    } else {
       // TODO maybe free the old pixbuf? is this needed? maybe? probabbly... 
       gtk_image_set_from_pixbuf(self->gtk_img, pixbuf);
    }
}