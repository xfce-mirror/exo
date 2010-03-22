/*
 * Copyright (c) 2009 Nick Schermer <nick@xfce.org>.
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

#include <exo/exo.h>
#include <gio/gio.h>
#include <gio/gdesktopappinfo.h>

#define EXO_HELPER_LAUNCH LIBEXECDIR "/exo-helper-" LIBEXO_VERSION_API " --launch "




#define EXO_TYPE_GIO_MODULE        (exo_gio_module_get_type ())
#define EXO_GIO_MODULE(o)          (G_TYPE_CHECK_INSTANCE_CAST ((o), G_TYPE_GIO_MODULE, ExoGioModule))
#define EXO_GIO_MODULE_CLASS(k)    (G_TYPE_CHECK_CLASS_CAST ((k), G_TYPE_GIO_MODULE, ExoGioModuleClass))
#define EXO_IS_GIO_MODULE(o)       (G_TYPE_CHECK_INSTANCE_TYPE ((o), G_TYPE_GIO_MODULE))
#define EXO_IS_GIO_MODULE_CLASS(k) (G_TYPE_CHECK_CLASS_TYPE ((k), G_TYPE_GIO_MODULE))



GType            exo_gio_module_get_type                   (void);
static void      exo_gio_module_app_info_lookup_iface_init (GDesktopAppInfoLookupIface *iface);
static GAppInfo *exo_gio_module_get_default_for_uri_scheme (GDesktopAppInfoLookup      *lookup,
                                                            const gchar                *uri_scheme);




typedef struct _ExoGioModuleClass ExoGioModuleClass;
typedef struct _ExoGioModule      ExoGioModule;
typedef struct _KnownSchemes      KnownSchemes;

struct _ExoGioModuleClass
{
  GObjectClass __parent__;
};

struct _ExoGioModule
{
  GObject __parent__;
};

struct _KnownSchemes
{
  const gchar *pattern;
  const gchar *category;
};

static KnownSchemes known_schemes[] =
{
  { "^(https?|ftps?|gopher)$", "WebBrowser" },
  { "^mailto$",                "MailReader" },
  { "^(file|trash|ssh)$",      "FileManager" }
};



#define _G_IMPLEMENT_INTERFACE_DYNAMIC(TYPE_IFACE, iface_init) \
{ \
  const GInterfaceInfo g_implement_interface_info = { \
    (GInterfaceInitFunc) iface_init, NULL, NULL \
  }; \
  g_type_module_add_interface (type_module, g_define_type_id, \
                               TYPE_IFACE, &g_implement_interface_info); \
}



G_DEFINE_DYNAMIC_TYPE_EXTENDED (ExoGioModule, exo_gio_module, G_TYPE_OBJECT, 0,
    _G_IMPLEMENT_INTERFACE_DYNAMIC (G_TYPE_DESKTOP_APP_INFO_LOOKUP,
                                    exo_gio_module_app_info_lookup_iface_init))



static void
exo_gio_module_class_init (ExoGioModuleClass *klass)
{
}



static void
exo_gio_module_init (ExoGioModule *module)
{
}



static void
exo_gio_module_class_finalize (ExoGioModuleClass *klass)
{
}



static void
exo_gio_module_app_info_lookup_iface_init (GDesktopAppInfoLookupIface *iface)
{
  iface->get_default_for_uri_scheme = exo_gio_module_get_default_for_uri_scheme;
}



static GAppInfo *
exo_gio_module_get_default_for_uri_scheme (GDesktopAppInfoLookup *lookup,
                                           const gchar           *uri_scheme)
{
  GAppInfo *info = NULL;
  GError   *error = NULL;
  gchar    *command;
  gboolean  found;
  guint     i;

  /* do nothing when there is no scheme defined */
  if (G_UNLIKELY (uri_scheme == NULL))
    return NULL;

  for (i = 0, found = FALSE; !found && i < G_N_ELEMENTS (known_schemes); i++)
    {
      /* see if the scheme matches */
      found = g_regex_match_simple (known_schemes[i].pattern, uri_scheme, G_REGEX_CASELESS, 0);
      if (found)
        {
          /* use the exo-helper directly here to avoid possible roundtrips with exo-open */
          command = g_strconcat (EXO_HELPER_LAUNCH, known_schemes[i].category, NULL);
          info = g_app_info_create_from_commandline (command, NULL, G_APP_INFO_CREATE_SUPPORTS_URIS, &error);
          if (G_UNLIKELY (info == NULL))
            {
              /* show error */
              g_critical ("Failed to create GAppInfo from \"%s\" for URI-scheme \"%s\": %s.",
                          command, uri_scheme, error->message);
              g_error_free (error);
            }

          /* cleanup */
          g_free (command);
        }
    }

#ifndef NDEBUG
  /* print debug message if scheme is not recognized by exo */
  if (!found)
    g_debug ("Unknown URI-scheme \"%s\".", uri_scheme);
#endif

  return info;
}



G_MODULE_EXPORT void
g_io_module_load (GIOModule *module)
{
  exo_gio_module_register_type (G_TYPE_MODULE (module));
  g_io_extension_point_implement (G_DESKTOP_APP_INFO_LOOKUP_EXTENSION_POINT_NAME,
                                  EXO_TYPE_GIO_MODULE, "ExoGioModule", 15);
}



G_MODULE_EXPORT void
g_io_module_unload (GIOModule   *module)
{
}

G_MODULE_EXPORT gchar **
g_io_module_query (void)
{
  gchar *eps[] = {
    G_DESKTOP_APP_INFO_LOOKUP_EXTENSION_POINT_NAME,
    NULL
  };
  return g_strdupv (eps);
}
