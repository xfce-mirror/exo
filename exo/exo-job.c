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

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <exo/exo-config.h>
#include <exo/exo-job.h>
#include <exo/exo-private.h>
#include <exo/exo-string.h>



#define EXO_JOB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_JOB, ExoJobPrivate))



/* Signal identifiers */
enum
{
  ERROR,
  FINISHED,
  INFO_MESSAGE,
  PERCENT,
  LAST_SIGNAL,
};



typedef struct _ExoJobSignalData ExoJobSignalData;



static void exo_job_class_init (ExoJobClass  *klass);
static void exo_job_init       (ExoJob       *job);
static void exo_job_finalize   (GObject      *object);
static void exo_job_error      (ExoJob       *job,
                                GError       *error);
static void exo_job_finished   (ExoJob       *job);



struct _ExoJobPrivate
{
  GIOSchedulerJob *scheduler_job;
  GCancellable    *cancellable;
  guint            running : 1;
};

struct _ExoJobSignalData
{
  gpointer instance;
  GQuark   signal_detail;
  guint    signal_id;
  va_list  var_args;
};



static GObjectClass *exo_job_parent_class = NULL;
static guint         job_signals[LAST_SIGNAL];



GType
exo_job_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = g_type_register_static_simple (G_TYPE_OBJECT, 
                                            "ExoJob",
                                            sizeof (ExoJobClass),
                                            (GClassInitFunc) exo_job_class_init,
                                            sizeof (ExoJob),
                                            (GInstanceInitFunc) exo_job_init,
                                            G_TYPE_FLAG_ABSTRACT);
    }

  return type;
}



static void
exo_job_class_init (ExoJobClass *klass)
{
  GObjectClass *gobject_class;

  g_type_class_add_private (klass, sizeof (ExoJobPrivate));

  /* Determine the parent type class */
  exo_job_parent_class = g_type_class_peek_parent (klass);

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = exo_job_finalize; 

  klass->execute = NULL;
  klass->error = NULL;
  klass->finished = NULL;
  klass->info_message = NULL;
  klass->percent = NULL;

  /**
   * ExoJob::error:
   * @job   : an #ExoJob.
   * @error : a #GError describing the cause.
   *
   * Emitted whenever an error occurs while executing the @job.
   **/
  job_signals[ERROR] =
    g_signal_new (I_("error"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_NO_HOOKS, 
                  G_STRUCT_OFFSET (ExoJobClass, error), 
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  /**
   * ExoJob::finished:
   * @job : an #ExoJob.
   *
   * This signal will be automatically emitted once the
   * @job finishes its execution, no matter whether @job
   * completed successfully or was cancelled by the
   * user.
   **/
  job_signals[FINISHED] =
    g_signal_new (I_("finished"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_NO_HOOKS, 0, NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);

  /**
   * ExoJob::info-message:
   * @job     : an #ExoJob.
   * @message : information to be displayed about @job.
   *
   * This signal is emitted to display information about the
   * @job. Examples of messages are "Preparing..." or
   * "Cleaning up...".
   *
   * The @message is garanteed to contain valid UTF-8, so
   * it can be displayed by #GtkWidget<!---->s out of the
   * box.
   **/
  job_signals[INFO_MESSAGE] =
    g_signal_new (I_("info-message"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_NO_HOOKS, 0, NULL, NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE, 1, G_TYPE_STRING);

  /**
   * ExoJob::percent:
   * @job     : an #ExoJob.
   * @percent : the percentage of completeness.
   *
   * This signal is emitted to present the state of the overall 
   * progress. The @percent value is garantied to be in the 
   * range 0.0 to 100.0.
   **/
  job_signals[PERCENT] =
    g_signal_new (I_("percent"),
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_NO_HOOKS, 0, NULL, NULL,
                  g_cclosure_marshal_VOID__DOUBLE,
                  G_TYPE_NONE, 1, G_TYPE_DOUBLE);
}



static void
exo_job_init (ExoJob *job)
{
  job->priv = EXO_JOB_GET_PRIVATE (job);
  job->priv->cancellable = g_cancellable_new ();
  job->priv->running = FALSE;
  job->priv->scheduler_job = NULL;
}



static void
exo_job_finalize (GObject *object)
{
  ExoJob *job = EXO_JOB (object);

  exo_job_cancel (job);
  g_object_unref (job->priv->cancellable);

  (*G_OBJECT_CLASS (exo_job_parent_class)->finalize) (object);
}



static gboolean
exo_job_finish (ExoJob             *job,
                GSimpleAsyncResult *result,
                GError            **error)
{
  _exo_return_val_if_fail (EXO_IS_JOB (job), FALSE);
  _exo_return_val_if_fail (G_IS_SIMPLE_ASYNC_RESULT (result), FALSE);
  _exo_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  return !g_simple_async_result_propagate_error (result, error);
}



static void
exo_job_async_ready (GObject      *object,
                     GAsyncResult *result)
{
  ExoJob *job = EXO_JOB (object);
  GError *error = NULL;

  _exo_return_if_fail (EXO_IS_JOB (job));
  _exo_return_if_fail (G_IS_SIMPLE_ASYNC_RESULT (result));

  if (!exo_job_finish (job, G_SIMPLE_ASYNC_RESULT (result), &error))
    {
      g_assert (error != NULL);

      /* don't treat cancellation as an error for now */
      if (error->code != G_IO_ERROR_CANCELLED)
        exo_job_error (job, error);

      g_error_free (error);
    }

  exo_job_finished (job);
}



static gboolean
exo_job_scheduler_job_func (GIOSchedulerJob *scheduler_job,
                            GCancellable    *cancellable,
                            gpointer         user_data)
{
  GSimpleAsyncResult *result = user_data;
  ExoJob             *job;
  GError             *error = NULL;
  gboolean            success;

  job = g_simple_async_result_get_op_res_gpointer (result);
  job->priv->scheduler_job = scheduler_job;

  success = (*EXO_JOB_GET_CLASS (job)->execute) (job, &error);

  /* TODO why was this necessary again? */
  g_io_scheduler_job_send_to_mainloop (scheduler_job, (GSourceFunc) gtk_false, 
                                       NULL, NULL);

  if (!success)
    {
      g_simple_async_result_set_from_error (result, error);
      g_error_free (error);
    }

  g_simple_async_result_complete_in_idle (result);

  return FALSE;
}



static gboolean
exo_job_emit_valist_in_mainloop (gpointer user_data)
{
  ExoJobSignalData *data = user_data;

  g_signal_emit_valist (data->instance, data->signal_id, data->signal_detail, 
                        data->var_args);

  return FALSE;
}


static void
exo_job_emit_valist (ExoJob *job,
                     guint   signal_id,
                     GQuark  signal_detail,
                     va_list var_args)
{
  ExoJobSignalData data;

  _exo_return_if_fail (EXO_IS_JOB (job));
  _exo_return_if_fail (job->priv->scheduler_job != NULL);

  data.instance = job;
  data.signal_id = signal_id;
  data.signal_detail = signal_detail;
  
  /* copy the variable argument list */
  G_VA_COPY (data.var_args, var_args);

  /* emit the signal in the main loop */
  g_io_scheduler_job_send_to_mainloop (job->priv->scheduler_job,
                                       exo_job_emit_valist_in_mainloop,
                                       &data, NULL);
}



static void
exo_job_error (ExoJob *job,
               GError *error)
{
  _exo_return_if_fail (EXO_IS_JOB (job));
  _exo_return_if_fail (error != NULL);

  g_signal_emit (job, job_signals[ERROR], 0, error);
}



static void
exo_job_finished (ExoJob *job)
{
  _exo_return_if_fail (EXO_IS_JOB (job));
  g_signal_emit (job, job_signals[FINISHED], 0);
}



/**
 * exo_job_launch:
 * @job : an #ExoJob.
 *
 * This functions schedules @job to be run as soon as possible, in a 
 * separate thread.
 *
 * Return value: a pointer to @job.
 **/
ExoJob*
exo_job_launch (ExoJob *job)
{
  GSimpleAsyncResult *result;

  _exo_return_val_if_fail (EXO_IS_JOB (job), NULL);
  _exo_return_val_if_fail (!job->priv->running, NULL);
  _exo_return_val_if_fail (EXO_JOB_GET_CLASS (job)->execute != NULL, NULL);

  /* mark the job as running */
  job->priv->running = TRUE;

  result = g_simple_async_result_new (G_OBJECT (job),
                                      (GAsyncReadyCallback) exo_job_async_ready,
                                      NULL, exo_job_launch);

  g_simple_async_result_set_op_res_gpointer (result, g_object_ref (job),
                                             (GDestroyNotify) g_object_unref);

  g_io_scheduler_push_job (exo_job_scheduler_job_func, result,
                           (GDestroyNotify) g_object_unref,
                           G_PRIORITY_HIGH, job->priv->cancellable);

  return job;
}



/**
 * exo_job_cancel:
 * @job : a #ExoJob.
 *
 * Attempts to cancel the operation currently performed by @job. Even 
 * after the cancellation of @job, it may still emit signals, so you
 * must take care of disconnecting all handlers appropriately if you 
 * cannot handle signals after cancellation.
 **/
void
exo_job_cancel (ExoJob *job)
{
  _exo_return_if_fail (EXO_IS_JOB (job));
  g_cancellable_cancel (job->priv->cancellable);
}



/**
 * exo_job_is_cancelled:
 * @job : a #ExoJob.
 *
 * Checks whether @job was previously cancelled
 * by a call to exo_job_cancel().
 *
 * Return value: %TRUE if @job is cancelled.
 **/
gboolean
exo_job_is_cancelled (const ExoJob *job)
{
  _exo_return_val_if_fail (EXO_IS_JOB (job), FALSE);
  return g_cancellable_is_cancelled (job->priv->cancellable);
}



GCancellable *
exo_job_get_cancellable (const ExoJob *job)
{
  _exo_return_val_if_fail (EXO_IS_JOB (job), NULL);
  return job->priv->cancellable;
}



gboolean
exo_job_set_error_if_cancelled (ExoJob  *job,
                                GError **error)
{
  _exo_return_val_if_fail (EXO_IS_JOB (job), FALSE);
  return g_cancellable_set_error_if_cancelled (job->priv->cancellable, error);
}



void
exo_job_emit (ExoJob *job,
              guint   signal_id,
              GQuark  signal_detail,
              ...)
{
  va_list var_args;

  _exo_return_if_fail (EXO_IS_JOB (job));

  va_start (var_args, signal_detail);
  exo_job_emit_valist (job, signal_id, signal_detail, var_args);
  va_end (var_args);
}



void
exo_job_info_message (ExoJob      *job,
                      const gchar *format,
                      ...)
{
  va_list var_args;
  gchar  *message;

  _exo_return_if_fail (EXO_IS_JOB (job));
  _exo_return_if_fail (message != NULL);

  va_start (var_args, format);
  message = g_strdup_vprintf (format, var_args);

  exo_job_emit (job, job_signals[INFO_MESSAGE], 0, message);

  g_free (message);
  va_end (var_args);
}



void
exo_job_percent (ExoJob *job,
                 gdouble percent)
{
  _exo_return_if_fail (EXO_IS_JOB (job));

  percent = MAX (0.0, MIN (100.0, percent));
  exo_job_emit (job, job_signals[PERCENT], 0, percent);
}

