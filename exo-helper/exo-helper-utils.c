/* $Id$ */
/*-
 * Copyright (c) 2003-2006 Benedikt Meurer <benny@xfce.org>.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo-helper/exo-helper-enum-types.h>
#include <exo-helper/exo-helper-utils.h>



/**
 * exo_helper_category_from_string:
 * @string          : string representation of an #ExoHelperCategory.
 * @category_return : return location for the #ExoHelperCategory.
 *
 * Transforms the @string representation of an #ExoHelperCategory to
 * an #ExoHelperCategory and places it in @category_return.
 *
 * Return value: %TRUE if @string was recognized and @category_return
 *               is set, else %FALSE.
 **/
gboolean
exo_helper_category_from_string (const gchar       *string,
                                 ExoHelperCategory *category_return)
{
  GEnumClass *klass;
  gboolean    found = FALSE;
  guint       n;

  g_return_val_if_fail (category_return != NULL, FALSE);

  if (G_LIKELY (string != NULL))
    {
      klass = g_type_class_ref (EXO_TYPE_HELPER_CATEGORY);
      for (n = 0; !found && n < klass->n_values; ++n)
        if (g_ascii_strcasecmp (string, klass->values[n].value_nick) == 0)
          {
            *category_return = klass->values[n].value;
            found = TRUE;
          }
      g_type_class_unref (klass);
    }

  return found;
}



/**
 * exo_helper_category_to_string:
 * @category : an #ExoHelperCategory.
 *
 * Transforms @category to its canonical string represenation.
 * The caller is responsible to free the returned string using
 * g_free() when no longer needed.
 *
 * Return value: the string representation for @category.
 **/
gchar*
exo_helper_category_to_string (ExoHelperCategory category)
{
  GEnumClass *klass;
  gchar      *string;

  g_return_val_if_fail (category < EXO_HELPER_N_CATEGORIES, NULL);
  g_return_val_if_fail (category >= 0, NULL);

  klass = g_type_class_ref (EXO_TYPE_HELPER_CATEGORY);
  string = g_strdup (klass->values[category].value_nick);
  g_type_class_unref (klass);

  return string;
}



static void
ebox_style_set (GtkWidget *widget,
                GtkStyle  *style)
{
  static guint recursive = 0;

  if (G_LIKELY (recursive == 0))
    {
      ++recursive;
      style = gtk_widget_get_style (widget);
      gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &style->bg[GTK_STATE_SELECTED]);
      --recursive;
    }
}



static void
label_style_set (GtkWidget *widget,
                 GtkStyle  *style)
{
  static guint recursive = 0;

  if (G_LIKELY (recursive == 0))
    {
      ++recursive;
      style = gtk_widget_get_style (widget);
      gtk_widget_modify_fg (widget, GTK_STATE_NORMAL, &style->fg[GTK_STATE_SELECTED]);
      --recursive;
    }
}



/**
 * exo_helper_create_header:
 * @icon : a named icon or %NULL.
 * @text : the text to display in the header.
 *
 * Creates a new header widget for settings dialogs,
 * compatible to the one used throughout Xfce.
 *
 * Return value: the header widget.
 **/
GtkWidget*
exo_helper_create_header (const gchar *icon,
                          const gchar *text)
{
  PangoAttribute *attribute;
  PangoAttrList  *attr_list_large_bold;
  GtkWidget      *label;
  GtkWidget      *image;
  GtkWidget      *hbox;
  GtkWidget      *ebox;

  g_return_val_if_fail (text != NULL, NULL);

  ebox = gtk_event_box_new ();
  g_signal_connect_after (G_OBJECT (ebox), "style-set", G_CALLBACK (ebox_style_set), NULL);

  hbox = gtk_hbox_new (FALSE, 12);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);
  gtk_container_add (GTK_CONTAINER (ebox), hbox);
  gtk_widget_show (hbox);

  if (G_LIKELY (icon != NULL))
    {
      image = gtk_image_new_from_icon_name (icon, GTK_ICON_SIZE_DIALOG);
      gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 0);
      gtk_widget_show (image);
    }

  label = gtk_label_new (text);
  g_signal_connect_after (G_OBJECT (label), "style-set", G_CALLBACK (label_style_set), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  /* make the label larger and bold */
  attr_list_large_bold = pango_attr_list_new ();
  attribute = pango_attr_scale_new (PANGO_SCALE_LARGE);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attr_list_large_bold, attribute);
  attribute = pango_attr_weight_new (PANGO_WEIGHT_BOLD);
  attribute->start_index = 0;
  attribute->end_index = -1;
  pango_attr_list_insert (attr_list_large_bold, attribute);
  gtk_label_set_attributes (GTK_LABEL (label), attr_list_large_bold);
  pango_attr_list_unref (attr_list_large_bold);

  return ebox;
}


