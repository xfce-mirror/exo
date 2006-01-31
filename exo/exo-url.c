/* $Id$ */
/*-
 * Copyright (c) 2005-2006 Benedikt Meurer <benny@xfce.org>.
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
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <exo/exo-execute.h>
#include <exo/exo-private.h>
#include <exo/exo-url.h>
#include <exo/exo-alias.h>



/* regular expressions for exo_url_show_on_screen() */
#define USERCHARS       "-A-Za-z0-9"
#define PASSCHARS       "-A-Za-z0-9,?;.:/!%$^*&~\"#'"
#define HOSTCHARS       "-A-Za-z0-9"
#define USER            "[" USERCHARS "]+(:["PASSCHARS "]+)?"
#define MATCH_BROWSER1  "((file|https?|ftps?)://(" USER "@)?)[" HOSTCHARS ".]+(:[0-9]+)?" \
                        "(/[-A-Za-z0-9_$.+!*(),;:@&=?/~#%]*[^]'.}>) \t\r\n,\\\"])?"
#define MATCH_BROWSER2  "(www|ftp)[" HOSTCHARS "]*\\.[" HOSTCHARS ".]+(:[0-9]+)?" \
                        "(/[-A-Za-z0-9_$.+!*(),;:@&=?/~#%]*[^]'.}>) \t\r\n,\\\"])?"
#if !defined(__GLIBC__)
#define MATCH_MAILER    "(mailto:)?[a-z0-9][a-z0-9.-]*@[a-z0-9][a-z0-9-]*(\\.[a-z0-9][a-z0-9-]*)+"
#else
#define MATCH_MAILER    "\\<(mailto:)?[a-z0-9][a-z0-9.-]*@[a-z0-9][a-z0-9-]*(\\.[a-z0-9][a-z0-9-]*)+\\>"
#endif



/**
 * exo_url_error_quark:
 *
 * Returns the #GError domain used for #ExoUrlError<!---->s
 * returned from exo_url_show() and exo_url_show_on_screen().
 *
 * Return value: the #GError domain used for #ExoUrlError<!---->s.
 *
 * Since: 0.3.1.3
 **/
GQuark
exo_url_error_quark (void)
{
  static GQuark quark = 0;

  if (G_UNLIKELY (quark == 0))
    quark = g_quark_from_static_string ("exo-url-error-quark");

  return quark;
}



/**
 * exo_url_show:
 * @url   : the URL that should be shown.
 * @envp  : child environment for the url handler or
 *          %NULL to inherit parent's environment.
 * @error : return location for errors or %NULL.
 *
 * Convenience wrapper to exo_url_show_on_screen(), which
 * shows the @url on the default #GdkScreen.
 *
 * Return value: %TRUE on success, %FALSE on error.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_url_show (const gchar *url,
              gchar      **envp,
              GError     **error)
{
  g_return_val_if_fail (url != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return exo_url_show_on_screen (url, envp, gdk_screen_get_default (), error);
}



static gboolean
match (const gchar *pattern, const gchar *url)
{
#ifdef HAVE_REGEXEC
  regex_t regex;
  gint    result = -1;

  if (regcomp (&regex, pattern, REG_EXTENDED) == 0)
    {
      result = regexec (&regex, url, 0, NULL, 0);
      regfree (&regex);
    }

  return (result == 0);
#else
#error "No POSIX regular expressions available, please report this to thunar-dev@xfce.org"
#endif
}



/**
 * exo_url_show_on_screen:
 * @url    : the URL that should be shown.
 * @envp   : child environment for the url handler or
 *           %NULL to inherit parent's environment.
 * @screen : the #GdkScreen on which to open the
 *           URL handler for @url.
 * @error  : return location for errors or %NULL.
 *
 * Tries to find a suitable handler for @url in the list of
 * preferred application categories and runs that handler 
 * with @url on @screen.
 *
 * Return value: %TRUE on success, %FALSE on error.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_url_show_on_screen (const gchar *url,
                        gchar      **envp,
                        GdkScreen   *screen,
                        GError     **error)
{
  const gchar *category;

  g_return_val_if_fail (url != NULL, FALSE);
  g_return_val_if_fail (GDK_IS_SCREEN (screen), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* now, let's see what we have here */
  if (match (MATCH_BROWSER1, url) || match (MATCH_BROWSER2, url))
    {
      category = "WebBrowser";
    }
  else if (match (MATCH_MAILER, url))
    {
      /* ignore mailto: prefix, as not all mailers can handle it */
      if (g_str_has_prefix (url, "mailto:"))
        url += strlen ("mailto:");
      category = "MailReader";
    }
  else
    {
      /* be sure to initialize i18n support first,
       * so we get a translated error message.
       */
      _exo_i18n_init ();

      /* and prepare the error */
      g_set_error (error, EXO_URL_ERROR, EXO_URL_ERROR_NOT_SUPPORTED,
                   _("The URL `%s' is not supported"), url);
      return FALSE;
    }

  /* oki doki then, let's open it */
  return exo_execute_preferred_application_on_screen (category, url, NULL, envp, screen, error);
}



/**
 * exo_url_about_dialog_hook:
 * @about_dialog : the #GtkAboutDialog in which the user activated a link.
 * @link         : the link, mail or web address, to open.
 * @user_data    : user data that was passed when the function was
 *                 registered with gtk_about_dialog_set_email_hook()
 *                 or gtk_about_dialog_set_url_hook(). This is currently
 *                 unused within the context of this function, so you
 *                 can safely pass %NULL when registering this hook
 *                 with #GtkAboutDialog.
 *
 * This is a convenience function, which can be registered with #GtkAboutDialog,
 * to have the <link linkend="exo-Opening-URLs">exo-url</link> module open links
 * clicked by the user in #GtkAboutDialog<!---->s.
 *
 * All you need to do is to register this hook with gtk_about_dialog_set_url_hook()
 * and gtk_about_dialog_set_email_hook(). This can be done prior to calling
 * gtk_show_about_dialog(), for example:
 *
 * <informalexample><programlisting>
 * static void show_about_dialog (void)
 * {
 *   gtk_about_dialog_set_email_hook (exo_url_about_dialog_hook, NULL, NULL);
 *   gtk_about_dialog_set_url_hook (exo_url_about_dialog_hook, NULL, NULL);
 *   gtk_show_about_dialog (.....);
 * }
 * </programlisting></informalexample>
 *
 * Since: 0.3.1.3
 **/
void
exo_url_about_dialog_hook (GtkAboutDialog *about_dialog,
                           const gchar    *link,
                           gpointer        user_data)
{
  GtkWidget *message;
  GdkScreen *screen;
  GError    *error = NULL;
  
  g_return_if_fail (GTK_IS_ABOUT_DIALOG (about_dialog));
  g_return_if_fail (link != NULL);

  /* determine the screen from the about dialog */
  screen = gtk_widget_get_screen (GTK_WIDGET (about_dialog));

  /* try to open the url on the given screen */
  if (!exo_url_show_on_screen (link, NULL, screen, &error))
    {
      /* make sure to initialize i18n support first,
       * so we'll see a translated message.
       */
      _exo_i18n_init ();

      /* display an error message to tell the user that we were unable to open the link */
      message = gtk_message_dialog_new (GTK_WINDOW (about_dialog),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
                                        _("Failed to open `%s'."), link);
      gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (message), "%s.", error->message);
      gtk_dialog_run (GTK_DIALOG (message));
      gtk_widget_destroy (message);
      g_error_free (error);
    }
}



#define __EXO_URL_C__
#include <exo/exo-aliasdef.c>
