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

#ifndef __EXO_DIE_ICON_BUTTON_H__
#define __EXO_DIE_ICON_BUTTON_H__

#include <exo/exo.h>

G_BEGIN_DECLS;

typedef struct _ExoDieIconButtonClass ExoDieIconButtonClass;
typedef struct _ExoDieIconButton      ExoDieIconButton;

#define EXO_DIE_TYPE_ICON_BUTTON            (exo_die_icon_button_get_type ())
#define EXO_DIE_ICON_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXO_DIE_TYPE_ICON_BUTTON, ExoDieIconButton))
#define EXO_DIE_ICON_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXO_DIE_TYPE_ICON_BUTTON, ExoDieIconButtonClass))
#define EXO_DIE_IS_ICON_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXO_DIE_TYPE_ICON_BUTTON))
#define EXO_DIE_IS_ICON_BUTTON_CLASS(klass) (G_TYPE_CHECL_CLASS_TYPE ((klass), EXO_DIE_TYPE_ICON_BUTTON))
#define EXO_DIE_ICON_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXO_DIE_TYPE_ICON_BUTTON, ExoDieIconButtonClass))

GType        exo_die_icon_button_get_type (void) G_GNUC_CONST;

GtkWidget   *exo_die_icon_button_new      (void) G_GNUC_MALLOC;

const gchar *exo_die_icon_button_get_icon (ExoDieIconButton *icon_button);
void         exo_die_icon_button_set_icon (ExoDieIconButton *icon_button,
                                           const gchar      *icon);

G_END_DECLS;

#endif /* !__EXO_DIE_ICON_BUTTON_H__ */
