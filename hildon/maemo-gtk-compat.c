/*
 * This file is a part of hildon
 *
 * Copyright (C) 2020 Marcin Mielniczuk, all rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef MAEMO_GTK
#include <gtk/gtk.h>

void
hildon_gtk_entry_set_placeholder_text           (GtkEntry    *entry,
                                                 const gchar *text)
{
    g_warning("hildon_gtk_entry_set_placeholder_text unsupported");
}

void
hildon_gtk_text_view_set_placeholder_text       (GtkEntry       *text_view,
                                                 const gchar    *text)
{
    g_warning("hildon_gtk_text_view_set_placeholder_text unsupported");
}
#endif
