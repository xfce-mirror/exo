/* $Id$ */
/*-
 * Copyright (c) 2004 os-cillation e.K.
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

#ifndef __EXO_PROPERTY_PROXY_H__
#define __EXO_PROPERTY_PROXY_H__

#include <glib-object.h>

G_BEGIN_DECLS;

#define EXO_TYPE_PROPERTY_PROXY             (exo_property_proxy_get_type ())
#define EXO_PROPERTY_PROXY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_TYPE_PROPERTY_PROXY, ExoPropertyProxy))
#define EXO_PROPERTY_PROXY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_TYPE_PROPERTY_PROXY, ExoPropertyProxyClass))
#define EXO_IS_PROPERTY_PROXY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_TYPE_PROPERTY_PROXY))
#define EXO_IS_PROPERTY_PROXY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EXO_TYPE_PROPERTY_PROXY))
#define EXO_PROPERTY_PROXY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_TYPE_PROPERTY_PROXY, ExoPropertyProxyClass))

typedef struct _ExoPropertyProxyClass ExoPropertyProxyClass;
typedef struct _ExoPropertyProxy      ExoPropertyProxy;

typedef void (*ExoPropertyConverter) (GValue *value, gboolean in, gpointer user_data);

struct _ExoPropertyProxyClass
{
  GObjectClass __parent__;
};

GType             exo_property_proxy_get_type (void) G_GNUC_CONST;

ExoPropertyProxy *exo_property_proxy_new      (void);

void              exo_property_proxy_add      (ExoPropertyProxy     *proxy,
                                               GObject              *object,
                                               const gchar          *property_name,
                                               ExoPropertyConverter  converter,
                                               gpointer              user_data,
                                               GDestroyNotify        destroy);

void              exo_property_proxy_remove   (ExoPropertyProxy     *proxy,
                                               GObject              *object,
                                               const gchar          *property_name);

G_END_DECLS;

#endif /* !__EXO_PROPERTY_PROXY_H__ */
