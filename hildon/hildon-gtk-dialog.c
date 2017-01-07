#include "hildon-gtk.h"
#include "hildon-gtk-dialog.h"

void
gtk_dialog_set_padding (GtkDialog *dialog,
                        guint      top_padding,
                        guint      bottom_padding,
                        guint      left_padding,
                        guint      right_padding)
{
#if 0
  GtkDialogPrivate *priv;

  g_return_if_fail (GTK_IS_DIALOG (dialog));

  priv = GET_PRIVATE (dialog);

  gtk_alignment_set_padding (GTK_ALIGNMENT (priv->alignment), top_padding, bottom_padding, left_padding, right_padding);
#else
    g_warning("gtk_dialog_set_padding");
#endif
}
