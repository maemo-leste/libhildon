#include "hildon-gtk.h"
#include "hildon-gtk-imcontext.h"

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
