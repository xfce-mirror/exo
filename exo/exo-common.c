/* vi:set et ai sw=2 sts=2 ts=2: */
/*-
 * Copyright (c) 2009 Jannis Pohlmann <jannis@xfce.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-common.h>

/**
 * exo_dialog_get_action_area:
 * @dialog : a #GtkDialog.
 *
 * Returns the action area of a #GtkDialog. The internal function has been
 * deprecated in GTK+, so this wraps and dispels the deprecation warning.
 *
 * Returns: the action area.
 **/
GtkWidget *
exo_dialog_get_action_area (GtkDialog *dialog)
{
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
    return gtk_dialog_get_action_area (dialog);
G_GNUC_END_IGNORE_DEPRECATIONS
}

/**
 * exo_dialog_add_secondary_button:
 * @dialog : a #GtkDialog.
 * @button : a #GtkButton to add and mark as secondary.
 *
 * Convenience function to add a secondary button to a #GtkDialog.
 **/
void
exo_dialog_add_secondary_button (GtkDialog *dialog,
                                 GtkWidget *button)
{
    GtkWidget *button_box;

    button_box = exo_dialog_get_action_area (dialog);
    gtk_box_pack_start (GTK_BOX (button_box), button, FALSE, FALSE, 0);
    gtk_button_box_set_child_secondary (GTK_BUTTON_BOX (button_box), button, TRUE);
}
