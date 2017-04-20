/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008, 2009 Nokia Corporation, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; version 2 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 */

#ifndef                                         __HILDON_GTK_H__
#define                                         __HILDON_GTK_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
  HILDON_SIZE_AUTO_WIDTH       = 0 << 0, /* set to automatic width */
  HILDON_SIZE_HALFSCREEN_WIDTH = 1 << 0, /* set to 50% screen width */
  HILDON_SIZE_FULLSCREEN_WIDTH = 2 << 0, /* set to 100% screen width */
  HILDON_SIZE_AUTO_HEIGHT      = 0 << 2, /* set to automatic height */
  HILDON_SIZE_FINGER_HEIGHT    = 1 << 2, /* set to finger height */
  HILDON_SIZE_THUMB_HEIGHT     = 2 << 2, /* set to thumb height */
  HILDON_SIZE_AUTO             = (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_AUTO_HEIGHT)
} HildonSizeType;

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

typedef enum
{
  GTK_TAP_AND_HOLD_NONE         = 0,
  GTK_TAP_AND_HOLD_PASS_PRESS   = 1 << 0,
  GTK_TAP_AND_HOLD_NO_SIGNALS   = 1 << 1,
  GTK_TAP_AND_HOLD_NO_INTERNALS = 1 << 2
} GtkWidgetTapAndHoldFlags;

typedef enum
{
  GTK_INVALID_INPUT_MAX_CHARS_REACHED,
  GTK_INVALID_INPUT_MODE_RESTRICTION
} GtkInvalidInputType;

GtkWidget *
hildon_gtk_menu_new                             (void);

GtkWidget *
hildon_gtk_button_new                           (HildonSizeType size);

GtkWidget *
hildon_gtk_toggle_button_new                    (HildonSizeType size);

GtkWidget *
hildon_gtk_radio_button_new                     (HildonSizeType  size,
                                                 GSList         *group);

GtkWidget *
hildon_gtk_radio_button_new_from_widget         (HildonSizeType  size,
                                                 GtkRadioButton *radio_group_member);

#ifdef MAEMO_GTK
GtkWidget *
hildon_gtk_tree_view_new                        (HildonUIMode mode);

GtkWidget *
hildon_gtk_tree_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model);

void
hildon_gtk_tree_view_set_ui_mode                (GtkTreeView  *treeview,
                                                 HildonUIMode  mode);

GtkWidget *
hildon_gtk_icon_view_new                        (HildonUIMode mode);

GtkWidget *
hildon_gtk_icon_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model);

void
hildon_gtk_icon_view_set_ui_mode                (GtkIconView  *iconview,
                                                 HildonUIMode  mode);
#endif /* MAEMO_GTK */

void
hildon_gtk_window_set_progress_indicator        (GtkWindow *window,
                                                 guint      state);

void
hildon_gtk_window_set_do_not_disturb            (GtkWindow *window,
                                                 gboolean   dndflag);

/**
 * HildonPortraitFlags:
 *
 * These flags are used to tell the window manager whether the current
 * window needs to be in portrait or landscape mode.
 *
 * If no flags are set then the window is meant to be used in
 * landscape mode only.
 *
 * If %HILDON_PORTRAIT_MODE_REQUEST is set then the window is meant to
 * be used in portrait mode only.
 *
 * If only %HILDON_PORTRAIT_MODE_SUPPORT is set then the current
 * orientation will be kept, no matter if it's portrait or landscape.
 *
 * It is important to note that, while these flags can be used to
 * change between portrait and landscape according to the physical
 * orientation of the display, Hildon does not provide any method to
 * obtain this information.
 **/
typedef enum {
    HILDON_PORTRAIT_MODE_REQUEST = 1 << 0,
    HILDON_PORTRAIT_MODE_SUPPORT = 1 << 1
} HildonPortraitFlags;

void
hildon_gtk_window_set_portrait_flags            (GtkWindow           *window,
                                                 HildonPortraitFlags  portrait_flags);

void
hildon_gtk_window_take_screenshot               (GtkWindow *window,
                                                 gboolean   take);
void
hildon_gtk_window_take_screenshot_sync          (GtkWindow *window,
                                                 gboolean   take);

void
hildon_gtk_window_enable_zoom_keys              (GtkWindow *window,
                                                 gboolean   enable);

GtkWidget*
hildon_gtk_hscale_new                           (void);

GtkWidget*
hildon_gtk_vscale_new                           (void);

/* GtkImContext */
typedef enum
{
  GTK_IM_CONTEXT_CLIPBOARD_OP_COPY,
  GTK_IM_CONTEXT_CLIPBOARD_OP_CUT,
  GTK_IM_CONTEXT_CLIPBOARD_OP_PASTE
} GtkIMContextClipboardOperation;

gboolean
hildon_gtk_im_context_filter_event (GtkIMContext   *context,
                                    GdkEvent        *event);

void
hildon_gtk_im_context_hide (GtkIMContext *context);

/* GtkWindow */
void
gtk_window_set_is_temporary (GtkWindow *window,
                             gboolean   setting);

gboolean
gtk_window_get_is_temporary (GtkWindow *window);

void
gtk_window_close_other_temporaries (GtkWindow *window);

/* GtkEntry */
HildonGtkInputMode
hildon_gtk_entry_get_input_mode (GtkEntry *entry);

void
hildon_gtk_entry_set_input_mode (GtkEntry           *entry,
                                 HildonGtkInputMode  mode);

void
hildon_gtk_entry_set_placeholder_text (GtkEntry    *entry,
                                       const gchar *placeholder_text);

const gchar *
hildon_gtk_entry_get_placeholder_text (GtkEntry *entry);

void
hildon_gtk_entry_set_input_default (GtkEntry           *entry,
                                    HildonGtkInputMode  mode);

HildonGtkInputMode
hildon_gtk_entry_get_input_default (GtkEntry *entry);

/* GtkTreeView */
HildonUIMode
hildon_tree_view_get_hildon_ui_mode  (GtkTreeView *tree_view);

void
hildon_tree_view_set_hildon_ui_mode  (GtkTreeView *tree_view,
                                      HildonUIMode mode);

/* GtkTextView */
void
hildon_gtk_text_view_set_placeholder_text (GtkTextView *text_view,
                                           const gchar *placeholder_text);

/* GtkWidget */
void
hildon_gtk_widget_set_theme_size (GtkWidget      *widget,
                                  HildonSizeType  size);

void
gtk_widget_tap_and_hold_setup (GtkWidget                *widget,
                               GtkWidget                *menu,
                               GtkCallback               func,
                               GtkWidgetTapAndHoldFlags  flags);

void
gtk_widget_tap_and_hold_menu_position_top(GtkWidget *menu,
                                          gint *x,
                                          gint *y,
                                          gboolean *push_in,
                                          GtkWidget *widget);

void
gtk_widget_insensitive_press(GtkWidget *widget);

/* GtkDialog */
void
gtk_dialog_set_padding (GtkDialog *dialog,
                        guint top_padding,
                        guint bottom_padding,
                        guint left_padding,
                        guint right_padding);

G_MODULE_EXPORT void hildon_gtk_module_init (gint *argc, gchar ***argv);

G_END_DECLS

#endif /* __HILDON_GTK_H__ */
