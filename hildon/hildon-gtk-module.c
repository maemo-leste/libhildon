#include "hildon-gtk.h"
#include "hildon-gtk-window.h"

G_MODULE_EXPORT void gtk_module_init (gint * argc, gchar *** argv);

G_MODULE_EXPORT void
gtk_module_init (gint * argc, gchar *** argv)
{
    hildon_subclass_gtk_window();
    hildon_subclass_gtk_widget();
}
