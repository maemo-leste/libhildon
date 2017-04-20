#include "hildon-gtk.h"
#include "hildon-gtk-window.h"
#include "hildon-gtk-widget.h"
#include "hildon-gtk-entry.h"
#include "hildon-gtk-imcontext.h"
#include "hildon-gtk-tree-view.h"

static gboolean inited = FALSE;

G_MODULE_EXPORT void
hildon_gtk_module_init (gint *argc, gchar ***argv)
{
    if (inited)
      return;

    inited = TRUE;

    hildon_subclass_gtk_widget();
    hildon_subclass_gtk_window();
    hildon_subclass_gtk_entry();
    hildon_subclass_gtk_imcontext();
    hildon_subclass_gtk_tree_view();
}

G_MODULE_EXPORT void gtk_module_init (gint *argc, gchar ***argv);

G_MODULE_EXPORT void
gtk_module_init (gint *argc, gchar ***argv)
{
  hildon_gtk_module_init (argc, argv);
}
