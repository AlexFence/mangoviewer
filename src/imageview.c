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
    double zoom;
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
    // 1 = 100%, 0.5 = 50%, 1.5 = 150%
    self->zoom = 1;
}

static void mango_img_view_init(MangoImageView *self) {
    mango_img_view_create_initials(self);

    gtk_container_add(GTK_CONTAINER (self->scrollwindow), GTK_WIDGET (self->gtk_img));
    gtk_container_add(GTK_CONTAINER (&self->parent_instance), self->scrollwindow);
    gtk_widget_add_events(GTK_WIDGET (&self->parent_instance), GDK_SCROLL_MASK);
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


static void zoom_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
    MangoImageView *self = MANGO_IMAGEVIEW (user_data);
    GdkModifierType modifiers;
    modifiers = gtk_accelerator_get_default_mod_mask();

    //if ((event->key.state & modifiers)  == GDK_CONTROL_MASK && event->key.keyval == GDK_KEY_Control_L)  { 
        g_print("%s", "hey!\n");

        double x = 0;
        double y = 0;

        gdk_event_get_scroll_deltas(event, &x, &y);
        printf("x:%1.2f\ny:%1.2f\n \n", x, y);

        if (y < 0.0) {
            mango_img_view_set_zoom(self, self->zoom + 0.1);
        } else if (y > 0.0) {
            mango_img_view_set_zoom(self, self->zoom - 0.1);
        }
    //}
}

MangoImageView *mango_img_view_new(MangoImageViewControls *controls) {
    MangoImageView *instance = g_object_new(MANGO_TYPE_IMAGEVIEW, NULL);

    g_signal_connect(G_OBJECT(controls), "next_image", mango_img_view_next, instance);
    g_signal_connect(G_OBJECT(controls), "previous_image", mango_img_view_previous, instance);
    g_signal_connect(G_OBJECT(instance->scrollwindow), "scroll-event", zoom_event, instance);

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


GdkPixbuf* mango_img_view_get_current_pixbuf(MangoImageView *self) {
    MangoImage mimg = mangofile_get_image(self->mangofile, self->current_img);
    mangoimg_uncompress(mimg);
    ImageData img_data = mangoimg_get_image_data(mimg);

    GError *error = NULL;
    GInputStream *my_cool_g_stream = g_memory_input_stream_new_from_data(img_data.pointer, img_data.length, NULL);
    return gdk_pixbuf_new_from_stream(my_cool_g_stream, NULL, &error);
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
        // TODO display a place holderGDK_INTERP_BILINEAR screen if the data is encrypted 
        //      or unreadable for some other reason
    } else {
       // TODO maybe free the old pixbuf? is this needed? maybe? probabbly... 
       gtk_image_set_from_pixbuf(self->gtk_img, pixbuf);
    }
}

static GdkPixbuf* scale_image(GdkPixbuf *pixbuf, double factor) {
    int w  = gdk_pixbuf_get_width(pixbuf);
    int h  = gdk_pixbuf_get_height(pixbuf);
    double new_w = w * factor;
    double new_h = h * factor;

    return gdk_pixbuf_scale_simple(pixbuf, new_w, new_h, GDK_INTERP_BILINEAR);
}

void mango_img_view_set_zoom(MangoImageView *self, double value) {
    if (value > 0.1)  {
        self->zoom = value;
    } else {
        self->zoom = 0.1;
    }
    printf("%.6f\n", self->zoom);
    GdkPixbuf * new_img = scale_image(mango_img_view_get_current_pixbuf(self), self->zoom);
    printf("aaa");
    gtk_image_set_from_pixbuf(self->gtk_img, new_img);
}



