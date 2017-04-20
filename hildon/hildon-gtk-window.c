#include "hildon-gtk.h"
#include "hildon-gtk-window.h"

#include <gtk/gtkprivate.h>

#include <string.h>

#ifdef GDK_WINDOWING_X11
#include "gdk/gdkx.h"
#endif

#include <libintl.h>

#ifdef ENABLE_NLS
#define P_(String) g_dgettext("hildon-libs-properties",String)
#else
#define P_(String) (String)
#endif

static GdkAtom atom_temporaries = GDK_NONE;
static GdkAtom atom_rcfiles = GDK_NONE;

static GQuark quark_gtk_embedded = 0;

enum {
    PROP_TEMPORARY = 30 /* PROP_MNEMONICS_VISIBLE + 1 */
};

static void
delete_if_temporary (GtkWidget *widget, GdkEventClient *client);

/**
 * gtk_window_set_is_temporary:
 * @window: a #GtkWindow
 * @setting: %TRUE if the window should be closed when it receives the _GTK_DELETE_TEMPORARIES ClientMessage
 *
 * Since: maemo 4.0
 * Stability: Unstable
 */
void
gtk_window_set_is_temporary (GtkWindow *window,
                             gboolean   setting)
{
  g_return_if_fail (GTK_IS_WINDOW (window));

  g_object_set_data(G_OBJECT(window), "temporary", GINT_TO_POINTER(setting));
  g_object_notify (G_OBJECT (window), "temporary");
}

/**
 * gtk_window_get_is_temporary:
 * @window: a #GtkWindow
 *
 * Return value: %TRUE if the window is marked as temporary.
 *
 * Since: maemo 4.0
 * Stability: Unstable
 */
gboolean
gtk_window_get_is_temporary (GtkWindow *window)
{
  g_return_val_if_fail (GTK_IS_WINDOW (window), FALSE);

  return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(window), "temporary"));
}

#if defined(GDK_WINDOWING_X11)
static gboolean
_gtk_window_is_on_client_data (GtkWindow *window, GdkEventClient *event)
{
  XID xid = GDK_WINDOW_XID (GTK_WIDGET (window)->window);
  return memcmp (&xid, (XID*)(&event->data.b[12]), sizeof(XID)) == 0;
}

/**
 * gtk_window_close_other_temporaries:
 *
 * Sends a _GTK_DELETE_TEMPORARIES ClientEvent to all other toplevel windows
 *
 * Since: maemo 4.0
 * Stability: Unstable
 */
void
gtk_window_close_other_temporaries (GtkWindow *window)
{
  GList *toplevels;
  GdkEventClient client;
  XID xid = GDK_WINDOW_XID (GTK_WIDGET (window)->window);

  memset(&client, 0, sizeof(client));
  client.message_type = gdk_atom_intern ("_GTK_DELETE_TEMPORARIES", FALSE);
  client.data_format = 8;
  memcpy (((XID*)(&client.data.b[12])),&xid, sizeof(XID));
  gdk_event_send_clientmessage_toall ((GdkEvent*)&client);

  /* The client messages are sent out of process and won't be
   * delivered before this function returns.  If the caller is
   * a modal dialog and thus grabs, the delete events for this
   * process could get ignored.
   */
  toplevels = gtk_window_list_toplevels ();
  g_list_foreach (toplevels, (GFunc)g_object_ref, NULL);

  while (toplevels)
    {
      GtkWindow *toplevel = toplevels->data;
      toplevels = g_list_delete_link (toplevels, toplevels);

      /* We check for MAPPED here instead of comparing to the
       * window argument, because there can be unmapped toplevels
       * that are != window.
       */
      if (GTK_WIDGET_MAPPED (toplevel)
          && gtk_window_get_is_temporary (toplevel))
        delete_if_temporary (GTK_WIDGET (toplevel), &client);

      g_object_unref (toplevel);
    }

  g_list_free (toplevels);
}
#endif /* GDK_WINDOWING_X11 */

static void
delete_if_temporary (GtkWidget *widget, GdkEventClient *client)
{
    if (gtk_window_get_is_temporary(GTK_WINDOW (widget)) &&
        _gtk_window_is_on_client_data (GTK_WINDOW (widget), client) == FALSE)
    {
      /* synthesize delete-event to close the window */
      GdkEvent *event;

      event = gdk_event_new (GDK_DELETE);

      event->any.window = g_object_ref (widget->window);
      event->any.send_event = TRUE;

      gtk_main_do_event (event);
      gdk_event_free (event);
    }
}

static gint
(*old_gtk_window_client_event)(GtkWidget *widget, GdkEventClient *event);

static void
send_client_message_to_embedded_windows (GtkWidget *widget,
                                         GdkAtom    message_type)
{
  GList *embedded_windows =
          g_object_get_qdata (G_OBJECT (widget), quark_gtk_embedded);

  if (embedded_windows)
    {
      GdkEvent *send_event = gdk_event_new (GDK_CLIENT_EVENT);
      int i;

      for (i = 0; i < 5; i++)
          send_event->client.data.l[i] = 0;

      send_event->client.data_format = 32;
      send_event->client.message_type = message_type;

      while (embedded_windows)
        {
          GdkNativeWindow xid =
                  GDK_GPOINTER_TO_NATIVE_WINDOW(embedded_windows->data);

          gdk_event_send_client_message_for_display (
                      gtk_widget_get_display (widget), send_event, xid);
          embedded_windows = embedded_windows->next;
      }

      gdk_event_free (send_event);
    }
}

static gint
hildon_gtk_window_client_event (GtkWidget *widget, GdkEventClient *event)
{
    if (!atom_rcfiles)
      {
        atom_rcfiles = gdk_atom_intern_static_string ("_GTK_READ_RCFILES");
        atom_temporaries =
                gdk_atom_intern_static_string ("_GTK_DELETE_TEMPORARIES");
      }

    if (event->message_type == atom_temporaries)
      {
        send_client_message_to_embedded_windows (widget, atom_temporaries);
        if (GTK_WIDGET_MAPPED (widget)
            && gtk_window_get_is_temporary (GTK_WINDOW(widget)))
          delete_if_temporary (widget, event);
      }
    else if (event->message_type == atom_rcfiles)
      {
        send_client_message_to_embedded_windows (widget, atom_rcfiles);

        /*
         * The theme may have been changed and a resource file may happen to
         * have the exact same modification time, so we pass TRUE for
         * force_reload. See NB#151715 for a discussion.
         */
        gtk_rc_reparse_all_for_settings (gtk_widget_get_settings (widget),
                                         TRUE);
      }
    else
        return old_gtk_window_client_event(widget, event);

    return FALSE;
}

void
hildon_subclass_gtk_window(void)
{
  GtkWindowClass *klass;
  GtkWidgetClass *widget_class;

  quark_gtk_embedded = g_quark_from_static_string ("gtk-embedded");

  klass = g_type_class_ref(GTK_TYPE_WINDOW);
  widget_class = GTK_WIDGET_CLASS(klass);

  old_gtk_window_client_event = widget_class->client_event;
  widget_class->client_event = hildon_gtk_window_client_event;

  g_object_class_install_property (G_OBJECT_CLASS(klass),
                                   PROP_TEMPORARY,
                                   g_param_spec_boolean ("temporary",
                                                         P_("Temporary"),
                                                         P_("Whether the window should be closed when it receives the _GTK_DELETE_TEMPORARIES ClientMessage"),
                                                         FALSE,
                                                         GTK_PARAM_READWRITE));
}
