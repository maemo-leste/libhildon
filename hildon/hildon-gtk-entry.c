#include "hildon-gtk.h"
#include "hildon-gtk-entry.h"

#include <string.h>

static gchar *
get_placeholder_text(GtkEntry *entry)
{
  return g_object_get_data(G_OBJECT(entry), "placeholder_text");
}

static void
set_placeholder_text(GtkEntry *entry, const gchar *placeholder_text)
{
  if (placeholder_text)
      g_object_set_data(G_OBJECT(entry), "placeholder_text",
                        g_strdup (placeholder_text));
  else
      g_object_set_data(G_OBJECT(entry), "placeholder_text", NULL);
}

static PangoLayout *
gtk_entry_create_placeholder_layout (GtkEntry *entry)
{
  GtkWidget *widget = GTK_WIDGET (entry);
  PangoLayout *layout = gtk_widget_create_pango_layout (widget, NULL);
  PangoDirection pango_dir;
  GdkColor font_color;
  gchar *placeholder_text = get_placeholder_text(entry);

  pango_layout_set_single_paragraph_mode (layout, TRUE);

  pango_dir = pango_find_base_dir (placeholder_text, strlen (placeholder_text));

  if (pango_dir == PANGO_DIRECTION_NEUTRAL)
    {
      if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL)
        pango_dir = PANGO_DIRECTION_RTL;
      else
        pango_dir = PANGO_DIRECTION_LTR;
    }

  pango_context_set_base_dir (gtk_widget_get_pango_context (widget),
                              pango_dir);

  pango_layout_set_alignment (layout, pango_dir);

  pango_layout_set_text (layout, placeholder_text, strlen (placeholder_text));

  if (gtk_style_lookup_color (widget->style, "ReversedSecondaryTextColor",
                              &font_color))
    {
      PangoAttrList *list;
      PangoAttribute *attr;

      list = pango_attr_list_new ();
      attr = pango_attr_foreground_new (font_color.red,
                                        font_color.green,
                                        font_color.blue);
      attr->start_index = 0;
      attr->end_index = G_MAXINT;
      pango_attr_list_insert (list, attr);

      pango_layout_set_attributes (layout, list);

      pango_attr_list_unref (list);
    }

  return layout;
}

static inline gboolean
show_placeholder (GtkEntry *entry)
{
  if (!GTK_WIDGET_HAS_FOCUS (entry) && entry->x_n_bytes == 0 &&
      get_placeholder_text(entry))
    {
      return TRUE;
    }

  return FALSE;
}

/**
 * hildon_gtk_entry_set_placeholder_text:
 * @entry: a #GtkEntry
 * @placeholder_text: a string to be displayed when @entry is empty
 * and unfocused or %NULL to remove current placeholder text.
 *
 * Sets a text string to be displayed when @entry is empty and unfocused.
 * This can be provided to give a visual hint of the expected contents
 * of the #GtkEntry.
 *
 * Since: maemo 5
 **/
void
hildon_gtk_entry_set_placeholder_text (GtkEntry    *entry,
                                       const gchar *placeholder_text)
{
  g_return_if_fail (GTK_IS_ENTRY (entry));

  gchar *old_placeholder_text = get_placeholder_text(entry);
  if (old_placeholder_text)
    {
      g_free (old_placeholder_text);
      g_object_unref (g_object_get_data(G_OBJECT(entry), "placeholder_layout"));
    }

  if (placeholder_text)
    {
      set_placeholder_text(entry, placeholder_text);
      g_object_set_data(G_OBJECT(entry), "placeholder_layout",
                        gtk_entry_create_placeholder_layout (entry));
    }
  else
    {
      set_placeholder_text(entry, NULL);
      g_object_set_data(G_OBJECT(entry), "placeholder_layout", NULL);
    }

  if (show_placeholder (entry))
    {
      gtk_widget_queue_draw (GTK_WIDGET (entry));
    }

  g_object_notify (G_OBJECT (entry), "hildon-placeholder-text");
}

/**
 * hildon_gtk_entry_get_placeholder_text:
 * @entry: a #GtkEntry
 *
 * Gets the text to be displayed if @entry is empty and unfocused.
 *
 * Returns: a string or %NULL if no placeholder text is set
 *
 * Since: maemo 5
 **/
const gchar *
hildon_gtk_entry_get_placeholder_text (GtkEntry *entry)
{
  g_return_val_if_fail (GTK_IS_ENTRY (entry), NULL);

  return get_placeholder_text(entry);
}

void
hildon_gtk_entry_set_input_mode (GtkEntry           *entry,
                                 HildonGtkInputMode  mode)
{
  g_return_if_fail (GTK_IS_ENTRY (entry));

  if (hildon_gtk_entry_get_input_mode (entry) != mode)
    {
      gtk_entry_set_visibility (entry,
                                mode & HILDON_GTK_INPUT_MODE_INVISIBLE
                                ? FALSE : TRUE);
      g_object_set (G_OBJECT (entry->im_context),
                    "hildon-input-mode", mode, NULL);
      g_object_notify (G_OBJECT (entry), "hildon-input-mode");
  }
}

HildonGtkInputMode
hildon_gtk_entry_get_input_mode (GtkEntry *entry)
{
  HildonGtkInputMode mode;

  g_return_val_if_fail (GTK_IS_ENTRY (entry), FALSE);

  g_object_get (G_OBJECT (entry->im_context),
                "hildon-input-mode", &mode, NULL);

  return mode;
}

/**
 * hildon_gtk_entry_set_input_default:
 * @entry: a #GtkEntry
 * @mode: a #HildonGtkInputMode
 *
 * Sets the default input mode of the widget.
 *
 * Since: maemo 5.0
 */
void
hildon_gtk_entry_set_input_default (GtkEntry           *entry,
                                    HildonGtkInputMode  mode)
{
  g_return_if_fail (GTK_IS_ENTRY (entry));

  if (hildon_gtk_entry_get_input_default (entry) != mode)
    {
      g_object_set (G_OBJECT (entry->im_context),
                    "hildon-input-default", mode, NULL);
      g_object_notify (G_OBJECT (entry), "hildon-input-default");
    }
}

/**
 * hildon_gtk_entry_get_input_default:
 * @entry: a #GtkEntry
 *
 * Gets the default input mode of the widget.
 *
 * Return value: the default input mode of the widget.
 *
 * Since: maemo 5.0
 */
HildonGtkInputMode
hildon_gtk_entry_get_input_default (GtkEntry *entry)
{
  HildonGtkInputMode mode;

  g_return_val_if_fail (GTK_IS_ENTRY (entry), FALSE);

  g_object_get (G_OBJECT (entry->im_context),
                "hildon-input-default", &mode, NULL);

  return mode;
}
