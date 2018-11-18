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

#ifndef IMAGEVIEW_H
#define IMAGEVIEW_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include "imageview_controls.h"

G_BEGIN_DECLS

#define MANGO_TYPE_IMAGEVIEW (mango_img_view_get_type())
G_DECLARE_FINAL_TYPE(MangoImageView, mango_img_view, MANGO, IMAGEVIEW, GtkBin);

MangoImageView *mango_img_view_new(MangoImageViewControls*);

int mango_img_view_load_file(MangoImageView*, char*);
int mango_img_view_image_count(MangoImageView*);
void mango_img_view_change_image(MangoImageView*, int);
G_END_DECLS

#endif