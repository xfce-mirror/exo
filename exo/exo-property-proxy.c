/* $Id: exo-property-proxy.c,v 1.1.1.1 2004/09/14 22:32:58 bmeurer Exp $ */
/*-
 * Copyright (c) 2004 Benedikt Meurer <benny@xfce.org>
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

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-property-proxy.h>



typedef struct _ProxyItem ProxyItem;



static void proxy_item_get_value (ProxyItem    *item,
                                  GValue       *value);
static void proxy_item_set_value (ProxyItem    *item,
                                  const GValue *value);

static void exo_property_proxy_finalize (GObject *object);



struct _ProxyItem
{
  ExoPropertyProxy    *proxy;
  GObject             *object;
  gchar               *property_name;
  ExoPropertyConverter converter;
  gpointer             user_data;
  GDestroyNotify       destroy;
  guint                signal_id;
};

struct _ExoPropertyProxy
{
  GObject __parent__;
  GList  *items;
};



static GObjectClass *parent_class;



static void
proxy_item_get_value (ProxyItem *item,
                      GValue    *value)
{
  GParamSpec *pspec;

  pspec = g_object_class_find_property (G_OBJECT_GET_CLASS (item->object),
                                        item->property_name);
  if (G_UNLIKELY (pspec == NULL))
    return;

  g_value_init (value, G_PARAM_SPEC_VALUE_TYPE (pspec));
  g_object_get_property (item->object, item->property_name, value);
  if (item->converter != NULL)
    item->converter (value, FALSE, item->user_data);
}



static void
proxy_item_set_value (ProxyItem    *item,
                      const GValue *value)
{
  GValue valbuf = { 0, };

  if (item->converter != NULL)
    {
      g_value_init (&valbuf, G_VALUE_TYPE (value));
      g_value_copy (value, &valbuf);
      item->converter (&valbuf, TRUE, item->user_data);
      value = &valbuf;
    }

  /* make sure, we don't recurse here */
  g_signal_handler_block (item->object, item->signal_id);
  g_object_set_property (item->object, item->property_name, value);
  g_signal_handler_unblock (item->object, item->signal_id);

  if (item->converter != NULL)
    g_value_unset (&valbuf);
}



G_DEFINE_TYPE (ExoPropertyProxy, exo_property_proxy, G_TYPE_OBJECT);



static void
exo_property_proxy_class_init (ExoPropertyProxyClass *klass)
{
  GObjectClass *gobject_class;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_property_proxy_finalize;
}



static void
exo_property_proxy_init (ExoPropertyProxy *proxy)
{
}



static void
exo_property_proxy_finalize (GObject *object)
{
  ExoPropertyProxy *proxy = EXO_PROPERTY_PROXY (object);

  if (proxy->items != NULL)
    {
      g_warning ("Finalizing an ExoPropertyProxy, that has active items, looks like "
                 "someone has messed the refcounting!");
    }

  G_OBJECT_CLASS (parent_class)->finalize (object);
}



static void
exo_property_proxy_object_notify (GObject     *object,
                                  GParamSpec  *pspec,
                                  ProxyItem   *item)
{
  GValue value = { 0, };
  GList *lp;

  proxy_item_get_value (item, &value);
  for (lp = item->proxy->items; lp != NULL; lp = lp->next)
    if (lp->data != item)
      proxy_item_set_value (lp->data, &value);
  g_value_unset (&value);
}



static void
exo_property_proxy_object_destroyed (gpointer data,
                                     GObject *object)
{
  ProxyItem *item = data;;

  item->proxy->items = g_list_remove (item->proxy->items, item);

  g_object_unref (G_OBJECT (item->proxy));

  if (item->destroy != NULL)
    item->destroy (item->user_data);
  g_free (item->property_name);
  g_free (item);
}



/**
 * exo_property_proxy_new:
 *
 * Return value :
 **/
ExoPropertyProxy*
exo_property_proxy_new (void)
{
  return g_object_new (EXO_TYPE_PROPERTY_PROXY, NULL);
}



/**
 * exo_property_proxy_add:
 * @proxy         : A #ExoPropertyProxy.
 * @object        : A #GObject.
 * @property_name : The name of a property of @object.
 * @converter     : A converter routine or %NULL.
 * @user_data     : Additional data to pass to @converter.
 * @destroy       : 
 **/
void
exo_property_proxy_add (ExoPropertyProxy    *proxy,
                        GObject             *object,
                        const gchar         *property_name,
                        ExoPropertyConverter converter,
                        gpointer             user_data,
                        GDestroyNotify       destroy)
{
  ProxyItem *first;
  ProxyItem *item;
  GValue     value = { 0, };
  GList     *lp;
  gchar     *signal;

  g_return_if_fail (EXO_IS_PROPERTY_PROXY (proxy));
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (property_name != NULL);

  for (lp = proxy->items; lp != NULL; lp = lp->next)
    {
      item = lp->data;
      if (item->object == object && strcmp (item->property_name, property_name) == 0)
        {
          g_warning ("Trying to add property %s of object %p to property proxy %p, "
                     "which is already present.", property_name, object, proxy);
          return;
        }
    }

  first = (proxy->items != NULL) ? proxy->items->data : NULL;

  item = g_new (ProxyItem, 1);
  item->proxy = proxy;
  item->object = object;
  item->property_name = g_strdup (property_name);
  item->converter = converter;
  item->user_data = user_data;
  item->destroy = destroy;

  proxy->items = g_list_append (proxy->items, item);
  g_object_ref (G_OBJECT (proxy));

  g_object_weak_ref (object, exo_property_proxy_object_destroyed, item);

  signal = g_strconcat ("notify::", property_name, NULL);
  item->signal_id = g_signal_connect (object, signal, G_CALLBACK (exo_property_proxy_object_notify), item);
  g_free (signal);

  if (first != NULL)
    {
      proxy_item_get_value (first, &value);
      proxy_item_set_value (item, &value);
      g_value_unset (&value);
    }
}



/**
 * exo_property_proxy_remove:
 * @proxy         :
 * @object        :
 * @property_name :
 **/
void
exo_property_proxy_remove (ExoPropertyProxy *proxy,
                           GObject          *object,
                           const gchar      *property_name)
{
  ProxyItem *item;
  GList     *lp;

  g_return_if_fail (EXO_IS_PROPERTY_PROXY (proxy));
  g_return_if_fail (G_IS_OBJECT (object));
  g_return_if_fail (property_name != NULL);

  g_object_ref (G_OBJECT (proxy));

again:
  for (lp = proxy->items; lp != NULL; lp = lp->next)
    {
      item = lp->data;
      if (item->object == object && strcmp (item->property_name, property_name) == 0)
        {
          proxy->items = g_list_delete_link (proxy->items, lp);

          g_object_weak_unref (item->object, exo_property_proxy_object_destroyed, item);
          g_signal_handler_disconnect (item->object, item->signal_id);

          g_object_unref (G_OBJECT (proxy));

          if (item->destroy != NULL)
            item->destroy (item->user_data);
          g_free (item->property_name);
          g_free (item);
          goto again;
        }
    }

  g_object_unref (G_OBJECT (proxy));
}

