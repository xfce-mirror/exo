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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-execute.h>
#include <exo/exo-alias.h>

/**
 * SECTION: exo-execute
 * @title: Executing Applications
 * @short_description: Execute preferred applications
 * @include: exo/exo.h
 * @see_also: gtk_show_uri()
 *
 * This module provides functions to execute certain kinds of applications,
 * for which users can select their preferred ones. For example, whenever
 * you need to run a command in a terminal emulator from within your
 * application you should use exo_execute_terminal_shell() or
 * exo_execute_terminal_shell_on_screen() to make sure you run the user's
 * preferred terminal emulator.
 * On the other hand if you need to display an URL (i.e. you want to point
 * the user to the website of your application), you should use gtk_show_uri()
 * instead, as it will try to automatically determine the appropriate
 * viewer for a given URI.
 **/



/**
 * exo_execute_preferred_application:
 * @category          : the category of the preferred application to launch.
 * @parameter         : additional parameter to pass to the preferred application
 *                      (i.e. an URL to pass to the preferred browser) or %NULL
 *                      to pass no parameter.
 * @working_directory : path to the directory in which to execute the
 *                      preferred application for @category.
 * @envp              : child's environment, or %NULL to inherit parent's.
 * @error             : return location for errors or %NULL.
 *
 * Convenience wrapper to exo_execute_preferred_application_on_screen(), which
 * runs the preferred application for @category on the default #GdkScreen.
 *
 * Note that even if this method returns %TRUE there's no warranty that
 * the preferred application for @category was run successfully, because
 * of the way the helper framework is implemented. But you can be sure
 * that if the execution fails at a later stage, the library will popup
 * an error dialog to inform the user that the execution failed.
 *
 * Returns: %TRUE on success, else %FALSE.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_execute_preferred_application (const gchar *category,
                                   const gchar *parameter,
                                   const gchar *working_directory,
                                   gchar      **envp,
                                   GError     **error)
{
  g_return_val_if_fail (category != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return exo_execute_preferred_application_on_screen (category, parameter, working_directory, envp, gdk_screen_get_default (), error);
}

/* Set the DISPLAY variable, to be use by g_spawn_async. */
static void
set_environment (gchar *display)
{
  g_setenv ("DISPLAY", display, TRUE);
}

/**
 * exo_execute_preferred_application_on_screen:
 * @category          : the category of the preferred application to launch.
 * @parameter         : additional parameter to pass to the preferred application
 *                      (i.e. an URL to pass to the preferred browser) or %NULL
 *                      to pass no parameter.
 * @working_directory : path to the directory in which to execute the
 *                      preferred application for @category.
 * @envp              : child's environment, or %NULL to inherit parent's.
 * @screen            : the #GdkScreen on which to run the preferred
 *                      application for @category.
 * @error             : return location for errors or %NULL.
 *
 * Launches the preferred application for the given @category with the
 * @parameter on @screen in the specified @working_directory.
 *
 * libexo currently supports the following categories: %"WebBrowser",
 * %"MailReader" and %"TerminalEmulator". If you specify an invalid
 * @category here, the execution will fail at a later stage and the
 * user will be presented with an error dialog.
 *
 * Note that even if this method returns %TRUE there's no warranty that
 * the preferred application for @category was run successfully, because
 * of the way the helper framework is implemented. But you can be sure
 * that if the execution fails at a later stage, the library will popup
 * an error dialog to inform the user that the execution failed.
 *
 * Returns: %TRUE on success, else %FALSE.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_execute_preferred_application_on_screen (const gchar *category,
                                             const gchar *parameter,
                                             const gchar *working_directory,
                                             gchar      **envp,
                                             GdkScreen   *screen,
                                             GError     **error)
{
  GdkDisplay *display;
  gchar      *argv[5];
  gchar      *display_name;
  gchar      *helper;
  gint        argc = 0;
  gboolean    success;

  g_return_val_if_fail (category != NULL, FALSE);
  g_return_val_if_fail (GDK_IS_SCREEN (screen), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* generate the argument vector */

  helper = g_find_program_in_path ("xfce4-mime-helper");
  if (G_LIKELY(helper != NULL))
  {
    argv[argc++] = helper;
  }
  else
  {
    argv[argc++] = "xfce4-mime-helper";
  }

  argv[argc++] = "--launch";
  argv[argc++] = (gchar *) category;

  /* append parameter if given */
  if (G_LIKELY (parameter != NULL))
    argv[argc++] = (gchar *) parameter;

  /* null terminate the argument vector */
  argv[argc] = NULL;

  /* set the display environment variable */
  display = gdk_screen_get_display (screen);
  display_name = g_strdup (gdk_display_get_name (display));

  /* launch the command */
  success = g_spawn_async (working_directory,
    argv,
    envp,
    0,
    (GSpawnChildSetupFunc) set_environment,
    display_name,
    NULL,
    error);

  if (helper)
    g_free (helper);

  g_free (display_name);
  return success;
}



/**
 * exo_execute_terminal_shell:
 * @command_line      : shell command line to execute.
 * @working_directory : path to the directory in which to execute @command_line
 *                      or %NULL to use the current working directory.
 * @envp              : child's environment, or %NULL to inherit parent's.
 * @error             : return location for errors or %NULL.
 *
 * Convenience wrapper to exo_execute_terminal_shell_on_screen(), which
 * executes the @command_line on the default #GdkScreen.
 *
 * Note that even if this method returns %TRUE there's no warranty that
 * the @command_line was run successfully, because of the way the helper
 * framework is implemented. But you can be sure that if the execution
 * fails at a later stage, the library will popup an error dialog to
 * inform the user that the execution failed.
 *
 * Returns: %TRUE on success, else %FALSE.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_execute_terminal_shell (const gchar *command_line,
                            const gchar *working_directory,
                            gchar      **envp,
                            GError     **error)
{
  g_return_val_if_fail (command_line != NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return exo_execute_terminal_shell_on_screen (command_line, working_directory, envp, gdk_screen_get_default (), error);
}



/**
 * exo_execute_terminal_shell_on_screen:
 * @command_line      : shell command line to execute.
 * @working_directory : path to the directory in which to execute @command_line
 *                      or %NULL to use the current working directory.
 * @envp              : child's environment, or %NULL to inherit parent's.
 * @screen            : the #GdkScreen on which to run the @command_line.
 * @error             : return location for errors or %NULL.
 *
 * Executes @command_line in the default terminal emulator on the specified
 * @screen.
 *
 * If no preferred terminal emulator was chosen by the user so far and
 * no sane fallback could be located, the user will be presented with
 * the preferred application chooser dialog, which prompts to choose
 * a default terminal emulator, and the @command_line will be run
 * afterwards using the new default.
 *
 * Note that even if this method returns %TRUE there's no warranty that
 * the @command_line was run successfully, because of the way the helper
 * framework is implemented. But you can be sure that if the execution
 * fails at a later stage, the library will popup an error dialog to
 * inform the user that the execution failed.
 *
 * Returns: %TRUE on success, else %FALSE.
 *
 * Since: 0.3.1.3
 **/
gboolean
exo_execute_terminal_shell_on_screen (const gchar *command_line,
                                      const gchar *working_directory,
                                      gchar      **envp,
                                      GdkScreen   *screen,
                                      GError     **error)
{
  g_return_val_if_fail (command_line != NULL, FALSE);
  g_return_val_if_fail (GDK_IS_SCREEN (screen), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return exo_execute_preferred_application_on_screen ("TerminalEmulator", command_line, working_directory, envp, screen, error);
}



#define __EXO_EXECUTE_C__
#include <exo/exo-aliasdef.c>
