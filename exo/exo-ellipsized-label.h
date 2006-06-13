/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
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

#if !defined (EXO_INSIDE_EXO_H) && !defined (EXO_COMPILATION)
#error "Only <exo/exo.h> can be included directly, this file may disappear or change contents."
#endif

/* ExoEllipsizedLabel is deprecated since 0.3.1.8. Use GtkLabel instead. */
#ifndef EXO_DISABLE_DEPRECATED

#ifndef __EXO_ELLIPSIZED_LABEL_H__
#define __EXO_ELLIPSIZED_LABEL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS;

#define EXO_TYPE_ELLIPSIZED_LABEL            (exo_ellipsized_label_get_type ())
#define EXO_ELLIPSIZED_LABEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_ELLIPSIZED_LABEL, ExoEllipsizedLabel))
#define EXO_ELLIPSIZED_LABEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_ELLIPSIZED_LABEL, ExoEllipsizedLabelClass))
#define EXO_IS_ELLIPSIZED_LABEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_ELLIPSIZED_LABEL))
#define EXO_IS_ELLIPSIZED_LABEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_ELLIPSIZED_LABEL))
#define EXO_ELLIPSIZED_LABEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_ELLIPSIZED_LABEL, ExoEllipsizedLabelClass))

typedef struct _ExoEllipsizedLabelPrivate  ExoEllipsizedLabelPrivate;
typedef struct _ExoEllipsizedLabelClass    ExoEllipsizedLabelClass;
typedef struct _ExoEllipsizedLabel         ExoEllipsizedLabel;

struct _ExoEllipsizedLabelClass
{
  /*< private >*/
  GtkLabelClass __parent__;
};

struct _ExoEllipsizedLabel
{
  /*< private >*/
  GtkLabel __parent__;
  ExoEllipsizedLabelPrivate *priv;
};

GType              exo_ellipsized_label_get_type      (void) G_GNUC_CONST;

GtkWidget         *exo_ellipsized_label_new           (const gchar        *text) G_GNUC_MALLOC;

PangoEllipsizeMode exo_ellipsized_label_get_ellipsize (ExoEllipsizedLabel *label);
void               exo_ellipsized_label_set_ellipsize (ExoEllipsizedLabel *label,
                                                       PangoEllipsizeMode  ellipsize);

G_END_DECLS;

#endif /* !__EXO_ELLIPSIZED_LABEL_H__ */

#endif /* !EXO_DISABLE_DEPRECATED */
