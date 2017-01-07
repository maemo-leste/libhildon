#include "hildon-gtk.h"
#include "hildon-gtk-textview.h"

#define GTK_TEXT_USE_INTERNAL_UNSUPPORTED_API
#include <gtk/gtktextlayout.h>

static GtkTextBuffer *
get_placeholder_buffer(GtkTextView *text_view)
{
  return g_object_get_data(G_OBJECT(text_view), "placeholder_buffer");
}

static void
set_placeholder_buffer(GtkTextView *text_view,
                       GtkTextBuffer *placeholder_buffer)
{
  g_object_set_data(G_OBJECT(text_view), "placeholder_buffer",
                    placeholder_buffer);
}

static GtkTextLayout *
get_placeholder_layout(GtkTextView *text_view)
{
  return g_object_get_data(G_OBJECT(text_view), "placeholder_layout");
}

static void
set_placeholder_layout(GtkTextView *text_view,
                       GtkTextLayout *placeholder_layout)
{
  g_object_set_data(G_OBJECT(text_view), "placeholder_layout",
                    placeholder_layout);
}

static void
gtk_text_view_set_attributes_from_style (GtkTextView        *text_view,
                                         GtkTextAttributes  *values,
                                         GtkStyle           *style)
{
  values->appearance.bg_color = style->base[GTK_STATE_NORMAL];
  values->appearance.fg_color = style->text[GTK_STATE_NORMAL];

  if (values->font)
    pango_font_description_free (values->font);

  values->font = pango_font_description_copy (style->font_desc);
}

static GtkTextBuffer*
get_buffer (GtkTextView *text_view)
{
  if (text_view->buffer == NULL)
    {
      GtkTextBuffer *b;
      b = gtk_text_buffer_new (NULL);
      gtk_text_view_set_buffer (text_view, b);
      g_object_unref (b);
    }

  return text_view->buffer;
}

/* This is more or less a stripped down version of
 * gtk_text_view_ensure_layout().
 */
static void
gtk_text_view_ensure_placeholder_layout (GtkTextView *text_view)
{
  GtkTextLayout *placeholder_layout = get_placeholder_layout(text_view);

  if (placeholder_layout == NULL)
    {
      GdkColor font_color;
      GtkTextAttributes *style;
      PangoContext *ltr_context, *rtl_context;
      GtkWidget *widget = GTK_WIDGET (text_view);

      placeholder_layout = gtk_text_layout_new ();
      set_placeholder_layout(text_view, placeholder_layout);
      gtk_text_layout_set_buffer (placeholder_layout,
                                  get_placeholder_buffer(text_view));

      gtk_text_layout_set_cursor_visible (placeholder_layout, FALSE);

      ltr_context = gtk_widget_create_pango_context (GTK_WIDGET (text_view));
      pango_context_set_base_dir (ltr_context, PANGO_DIRECTION_LTR);
      rtl_context = gtk_widget_create_pango_context (GTK_WIDGET (text_view));
      pango_context_set_base_dir (rtl_context, PANGO_DIRECTION_RTL);

      gtk_text_layout_set_contexts (placeholder_layout, ltr_context,
                                    rtl_context);

      g_object_unref (ltr_context);
      g_object_unref (rtl_context);


      style = gtk_text_attributes_new ();

      gtk_widget_ensure_style (widget);
      gtk_text_view_set_attributes_from_style (text_view,
                                               style, widget->style);

      /* Override the color setting */
      if (gtk_style_lookup_color (widget->style, "ReversedSecondaryTextColor",
                                  &font_color))
        {
          style->appearance.fg_color = font_color;
        }

      style->pixels_above_lines = text_view->pixels_above_lines;
      style->pixels_below_lines = text_view->pixels_below_lines;
      style->pixels_inside_wrap = text_view->pixels_inside_wrap;
      style->left_margin = text_view->left_margin;
      style->right_margin = text_view->right_margin;
      style->indent = text_view->indent;
      style->tabs =
              text_view->tabs ? pango_tab_array_copy (text_view->tabs) : NULL;

      style->wrap_mode = text_view->wrap_mode;
      style->justification = text_view->justify;
      style->direction = gtk_widget_get_direction (GTK_WIDGET (text_view));

      gtk_text_layout_set_default_style (placeholder_layout, style);

      gtk_text_attributes_unref (style);
    }

  /* Now make sure the layout is validated.  Since we expect the
   * placeholder to only be a single line, this should be quick.
   */
  gtk_text_layout_validate (placeholder_layout, 2000);
}

/**
 * hildon_gtk_text_view_set_placeholder_text:
 * @text_view: a #GtkTextView.
 * @placeholder_text: a string to be displayed when @text_view is empty
 * and unfocused or %NULL to remove current placeholder text.
 *
 * Sets a text string to be displayed when @entry is empty and unfocused.
 * This can be provided to give a visual hint of the expected contents
 * of the #GtkEntry.
 *
 * Since: maemo 5.
 */
void
hildon_gtk_text_view_set_placeholder_text (GtkTextView *text_view,
                                           const gchar *placeholder_text)
{
  GtkTextBuffer *placeholder_buffer;

  g_return_if_fail (GTK_IS_TEXT_VIEW (text_view));

  placeholder_buffer = get_placeholder_buffer(text_view);

  if (!placeholder_buffer)
  {
    placeholder_buffer = gtk_text_buffer_new (NULL);
    set_placeholder_buffer(text_view, placeholder_buffer);
  }

  if (placeholder_text)
    {
      gtk_text_buffer_set_text (placeholder_buffer, placeholder_text, -1);
      gtk_text_view_ensure_placeholder_layout (text_view);
    }
  else
    {
      GtkTextLayout *placeholder_layout = get_placeholder_layout(text_view);

      g_object_unref (placeholder_layout);
      set_placeholder_layout(text_view, NULL);

      g_object_unref (get_placeholder_buffer(text_view));
      set_placeholder_buffer(text_view, NULL);
    }

  if (gtk_text_buffer_get_char_count (get_buffer (text_view)) == 0
      && !GTK_WIDGET_HAS_FOCUS (text_view))
    gtk_widget_queue_draw (GTK_WIDGET (text_view));

  g_object_notify (G_OBJECT (text_view), "hildon-placeholder-text");
}
