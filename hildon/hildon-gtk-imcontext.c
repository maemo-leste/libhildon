#include "hildon-gtk.h"
#include "hildon-gtk-imcontext.h"
#include "hildon-enum-types.h"

enum {
  HAS_SELECTION,
  CLIPBOARD_OPERATION,
  LAST_SIGNAL
};

enum {
  PROP_HILDON_INPUT_MODE = 1,
  PROP_HILDON_INPUT_DEFAULT
};

static guint im_context_signals[LAST_SIGNAL] = { 0 };

/**
 * hildon_gtk_im_context_filter_event:
 * @context: a #GtkIMContext
 * @event: the event
 *
 * Allow an input method to internally handle an event.
 * If this function returns %TRUE, then no further processing
 * should be done for this event.
 *
 * <note><para>
 * Input methods must be able to accept all types of events (simply
 * returning %FALSE if the event was not handled), but there is no
 * obligation for a widget to submit any events to this function.
 * </para><para>
 * Widget events that are recommended to be run through this function
 * are %GDK_BUTTON_PRESS, %GDK_BUTTON_RELEASE, %GDK_2BUTTON_PRESS,
 * %GDK_3BUTTON_PRESS, %GDK_KEY_PRESS and %GDK_KEY_RELEASE.
 * </para><para>
 * Note that if the event passes the filter with the function returning
 * %FALSE, the widget still needs to process the event itself, this can
 * include calling gtk_im_context_focus_in(), gtk_im_context_focus_out()
 * or gtk_im_context_filter_keypress() for focus and keypress events
 * where applicable.
 * </para></note>
 *
 * Return value: %TRUE if the input method handled the event.
 *
 * Since: maemo 2.0
 * Stability: Unstable
 */
gboolean
hildon_gtk_im_context_filter_event (GtkIMContext   *context,
                                    GdkEvent       *event)
{
    GtkIMContextClass *klass;

    g_return_val_if_fail (GTK_IS_IM_CONTEXT (context), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    klass = GTK_IM_CONTEXT_GET_CLASS (context);

    /*
     * This is a bit hacky here, but I coudn't find a better way to do it. So,
     * filter_keypress takes GdkEventKey, but we pass GdkEvent as well for
     * those IM contexts that have "hildon-gtk-im-context" property. For the
     * others we simply return FALSE.
     */
    if (event->type != GDK_KEY_PRESS && event->type != GDK_KEY_RELEASE &&
        !g_object_class_find_property (G_OBJECT_CLASS(klass),
                                       "hildon-gtk-im-context")) {
        return FALSE;
    }

    return klass->filter_keypress (context, (GdkEventKey *)event);
}

/**
 * hildon_gtk_im_context_hide:
 * @context: a #GtkIMContext
 *
 * Notify the input method that widget thinks the actual
 * input method show be closed.
 *
 * Since: maemo 2.0
 * Stability: Unstable
 **/
void
hildon_gtk_im_context_hide (GtkIMContext *context)
{
#if 0
  GtkIMContextClass *klass;

  g_return_if_fail (GTK_IS_IM_CONTEXT (context));

  klass = GTK_IM_CONTEXT_GET_CLASS (context);
  if (klass->hide)
    klass->hide (context);
#else
  g_warning("hildon_gtk_im_context_hide");
#endif
}

static guint
get_flags(GObject *object, const char *name)
{
    return GPOINTER_TO_UINT(g_object_get_data(object, name));
}

static void
set_flags(GObject *object, const char *name, guint flags)
{
    g_object_set_data(object, name, GUINT_TO_POINTER(flags));
}

static void
hildon_gtk_im_context_set_property (GObject      *object,
                                    guint         property_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  switch (property_id)
    {
      case PROP_HILDON_INPUT_MODE:
        set_flags(object, "hildon_input_mode", g_value_get_flags (value));
        break;
      case PROP_HILDON_INPUT_DEFAULT:
        set_flags(object, "hildon_input_default", g_value_get_flags (value));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
hildon_gtk_im_context_get_property (GObject    *object,
                                    guint       property_id,
                                    GValue     *value,
                                    GParamSpec *pspec)
{
  switch (property_id)
    {
      case PROP_HILDON_INPUT_MODE:
        g_value_set_flags (value, get_flags(object, "hildon_input_mode"));
        break;
      case PROP_HILDON_INPUT_DEFAULT:
        g_value_set_flags (value, get_flags(object, "hildon_input_default"));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

void
hildon_subclass_gtk_imcontext(void)
{
  GObjectClass *gobject_class;
  GtkIMContextClass *klass;
  g_warning("hildon_subclass_gtk_imcontext");

  klass = g_type_class_ref(GTK_TYPE_IM_MULTICONTEXT);
  gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->set_property = hildon_gtk_im_context_set_property;
  gobject_class->get_property = hildon_gtk_im_context_get_property;

  /**
   * GtkIMContext::has-selection:
   * @context: a #GtkIMContext
   *
   * This signal is emitted when input context needs to know if there is
   * any text selected in the widget. Return TRUE if there is.
   *
   * Since: maemo 2.0
   * Stability: Unstable
   **/
  im_context_signals[HAS_SELECTION] =
          g_signal_new ("has_selection",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        0,
                        NULL, NULL,
                        gtk_marshal_BOOLEAN__VOID,
                        G_TYPE_BOOLEAN, 0);
  /**
   * GtkIMContext::clipboard-operation:
   * @context: a #GtkIMContext
   * @operation: a #GtkIMContextClipboardOperation
   *
   * This signal is emitted when input context wants to copy, cut or paste
   * text. The widget needs to implement these operations.
   *
   * Since: maemo 2.0
   * Stability: Unstable
   **/
  /*im_context_signals[CLIPBOARD_OPERATION] =
          g_signal_new ("clipboard_operation",
                        G_TYPE_FROM_CLASS (klass),
                        G_SIGNAL_RUN_LAST,
                        0,
                        NULL, NULL,
                        _gtk_marshal_VOID__ENUM,
                        G_TYPE_NONE, 1, GTK_TYPE_IM_CONTEXT_CLIPBOARD_OPERATION);*/

  /**
   * GtkIMContext:hildon-input-mode:
   *
   * Allowed characters and input mode for this IM context.
   * See #HildonGtkInputMode.
   *
   * Since: maemo 2.0
   * Stability: Unstable
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_HILDON_INPUT_MODE,
                                   g_param_spec_flags ("hildon-input-mode",
                                                       "Hildon input mode",
                                                       "Allowed characters and input mode",
                                                       HILDON_TYPE_GTK_INPUT_MODE,
                                                       HILDON_GTK_INPUT_MODE_FULL |
                                                       HILDON_GTK_INPUT_MODE_AUTOCAP |
                                                       HILDON_GTK_INPUT_MODE_DICTIONARY,
                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /**
   * GtkIMContext:hildon-input-default:
   *
   * Default input mode for this IM context.  See #HildonGtkInputMode.
   * The default setting for this property is %HILDON_GTK_INPUT_MODE_FULL,
   * which means that the default input mode to be used is up to the
   * implementation of the IM context.
   *
   * Since: maemo 5.0
   **/
      g_object_class_install_property (gobject_class,
                       PROP_HILDON_INPUT_DEFAULT,
                       g_param_spec_flags ("hildon-input-default",
                                   "Hildon input default",
                                   "Default input mode",
                                   HILDON_TYPE_GTK_INPUT_MODE,
                                   HILDON_GTK_INPUT_MODE_FULL,
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
}
