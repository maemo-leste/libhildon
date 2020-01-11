/*
 * This file is a part of hildon
 *
 * Copyright (C) Nokia Corporation, all rights reserved.
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

#ifndef __MAEMOGTKCOMPAT_H__
#define __MAEMOGTKCOMPAT_H__

/* Provides the types from the Maemo Gtk+ API if upstream Gtk+ is used.
 * This was copy-pasted from hildonize-gtk-enums.diff from maemo-leste/gtk */
#ifndef MAEMO_GTK
typedef enum
{
  GTK_INVALID_INPUT_MAX_CHARS_REACHED,
  GTK_INVALID_INPUT_MODE_RESTRICTION
} GtkInvalidInputType;

typedef enum
{
  HILDON_GTK_INPUT_MODE_ALPHA             = 1 << 0,
  HILDON_GTK_INPUT_MODE_NUMERIC           = 1 << 1,
  HILDON_GTK_INPUT_MODE_SPECIAL           = 1 << 2,
  HILDON_GTK_INPUT_MODE_HEXA              = 1 << 3,
  HILDON_GTK_INPUT_MODE_TELE              = 1 << 4,
  HILDON_GTK_INPUT_MODE_FULL              = (HILDON_GTK_INPUT_MODE_ALPHA | HILDON_GTK_INPUT_MODE_NUMERIC | HILDON_GTK_INPUT_MODE_SPECIAL),
  HILDON_GTK_INPUT_MODE_NO_SCREEN_PLUGINS = 1 << 27,
  HILDON_GTK_INPUT_MODE_MULTILINE         = 1 << 28,
  HILDON_GTK_INPUT_MODE_INVISIBLE         = 1 << 29,
  HILDON_GTK_INPUT_MODE_AUTOCAP           = 1 << 30,
  HILDON_GTK_INPUT_MODE_DICTIONARY        = 1 << 31
} HildonGtkInputMode;

/* Temporary compatibility define */
#define GTK_TYPE_GTK_INPUT_MODE HILDON_TYPE_GTK_INPUT_MODE

typedef enum
{
  HILDON_DIABLO,
  HILDON_FREMANTLE
} HildonMode;

typedef enum
{
  HILDON_UI_MODE_NORMAL,
  HILDON_UI_MODE_EDIT
} HildonUIMode;

typedef enum {
   HILDON_SIZE_AUTO_WIDTH       = 0 << 0, /* set to automatic width */
   HILDON_SIZE_HALFSCREEN_WIDTH = 1 << 0, /* set to 50% screen width */
   HILDON_SIZE_FULLSCREEN_WIDTH = 2 << 0, /* set to 100% screen width */
   HILDON_SIZE_AUTO_HEIGHT      = 0 << 2, /* set to automatic height */
   HILDON_SIZE_FINGER_HEIGHT    = 1 << 2, /* set to finger height */
   HILDON_SIZE_THUMB_HEIGHT     = 2 << 2, /* set to thumb height */
   HILDON_SIZE_AUTO             = (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_AUTO_HEIGHT)
} HildonSizeType;

/* Support for placeholder texts is used extensively around libhildon,
 * but requires a patched version of Gtk+. It was added upstream in Gtk 3.2.
 * Therefore, for a non-Maemo version of Gtk+ we expose dummy functions. */
void
hildon_gtk_entry_set_placeholder_text           (GtkEntry *entry,
                                                 const gchar *text);

void
hildon_gtk_text_view_set_placeholder_text       (GtkEntry *text_view,
                                                 const gchar    *text);

#endif

#endif // __MAEMOGTKCOMPAT_H__
