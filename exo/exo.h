/* $Id: exo.h,v 1.2 2004/09/17 09:48:24 bmeurer Exp $ */
/*-
 * Copyright (c) 2004-2005 os-cillation e.K.
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

#ifndef __EXO_H__
#define __EXO_H__

#ifndef EXO_API_SUBJECT_TO_CHANGE
#error "Please define EXO_API_SUBJECT_TO_CHANGE to acknoledge your understanding that libexo hasn't reached 1.0 and is subject to API churn. See the README for a full explanation."
#endif

#define EXO_INSIDE_EXO_H

#include <libxfcegui4/libxfcegui4.h>

#include <exo/exo-config.h>

#include <exo/exo-binding.h>
#include <exo/exo-cell-renderer-ellipsized-text.h>
#include <exo/exo-ellipsized-label.h>
#include <exo/exo-enum-types.h>
#include <exo/exo-gdk-pixbuf-extensions.h>
#include <exo/exo-gtk-extensions.h>
#include <exo/exo-gobject-extensions.h>
#include <exo/exo-icon-bar.h>
#include <exo/exo-icon-view.h>
#include <exo/exo-md5.h>
#include <exo/exo-pango-extensions.h>
#include <exo/exo-string.h>
#include <exo/exo-toolbars-editor.h>
#include <exo/exo-toolbars-editor-dialog.h>
#include <exo/exo-toolbars-model.h>
#include <exo/exo-toolbars-view.h>
#include <exo/exo-xsession-client.h>

#undef EXO_INSIDE_EXO_H

#endif /* !__EXO_H__ */
