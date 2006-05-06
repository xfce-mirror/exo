/* $Id$ */
/*-
 * Copyright (c) 2006 Benedikt Meurer <benny@xfce.org>.
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo-desktop-item-edit/exo-die-icon-button.h>



/* Property identifiers */
enum
{
  PROP_0,
  PROP_ICON,
};



static void exo_die_icon_button_class_init    (ExoDieIconButtonClass  *klass);
static void exo_die_icon_button_init          (ExoDieIconButton       *icon_button);
static void exo_die_icon_button_finalize      (GObject                *object);
static void exo_die_icon_button_get_property  (GObject                *object,
                                               guint                   prop_id,
                                               GValue                 *value,
                                               GParamSpec             *pspec);
static void exo_die_icon_button_set_property  (GObject                *object,
                                               guint                   prop_id,
                                               const GValue           *value,
                                               GParamSpec             *pspec);



struct _ExoDieIconButtonClass
{
  GtkAlignmentClass __parent__;
};

struct _ExoDieIconButton
{
  GtkAlignment __parent__;
  gchar       *icon;
};



static GObjectClass *exo_die_icon_button_parent_class;



GType
exo_die_icon_button_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      static const GTypeInfo info =
      {
        sizeof (ExoDieIconButtonClass),
        NULL,
        NULL,
        (GClassInitFunc) exo_die_icon_button_class_init,
        NULL,
        NULL,
        sizeof (ExoDieIconButton),
        0,
        (GInstanceInitFunc) exo_die_icon_button_init,
        NULL,
      };

      type = g_type_register_static (GTK_TYPE_ALIGNMENT, I_("ExoDieIconButton"), &info, 0);
    }

  return type;
}



static void
exo_die_icon_button_class_init (ExoDieIconButtonClass *klass)
{
  GObjectClass *gobject_class;

  /* determine the parent type class */
  exo_die_icon_button_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_die_icon_button_finalize;
  gobject_class->get_property = exo_die_icon_button_get_property;
  gobject_class->set_property = exo_die_icon_button_set_property;

  /**
   * ExoDieIconButton:icon:
   *
   * The name of the icon or the absolute path to an
   * image file.
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_ICON,
                                   g_param_spec_string ("icon",
                                                        "icon",
                                                        "icon",
                                                        NULL,
                                                        EXO_PARAM_READWRITE));
}



static void
exo_die_icon_button_init (ExoDieIconButton *icon_button)
{
  GtkWidget *button;

  /* setup the alignment */
  gtk_alignment_set (GTK_ALIGNMENT (icon_button), 0.0f, 0.5f, 0.0f, 0.0f);

  gtk_widget_push_composite_child ();

  /* allocate the button */
  button = gtk_button_new ();
  gtk_wi

  gtk_widget_pop_composite_child ();
}



static void exo_die_icon_button_finalize      (GObject                *object);
static void exo_die_icon_button_get_property  (GObject                *object,
                                               guint                   prop_id,
                                               GValue                 *value,
                                               GParamSpec             *pspec);
static void exo_die_icon_button_set_property  (GObject                *object,
                                               guint                   prop_id,
                                               const GValue           *value,
                                               GParamSpec             *pspec);




