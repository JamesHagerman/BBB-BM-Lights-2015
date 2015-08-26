/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2010  Intel Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *   Emmanuele Bassi <ebassi@linux.intel.com>
 */

#ifndef __CLUTTER_SENSATRON_EFFECT_H__
#define __CLUTTER_SENSATRON_EFFECT_H__

#include <clutter/clutter.h>

G_BEGIN_DECLS

#define CLUTTER_TYPE_SENSATRON_EFFECT        (clutter_sensatron_effect_get_type ())
#define CLUTTER_SENSATRON_EFFECT(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_TYPE_SENSATRON_EFFECT, ClutterSensatronEffect))
#define CLUTTER_IS_SENSATRON_EFFECT(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_TYPE_SENSATRON_EFFECT))

/**
 * ClutterSensatronEffect:
 *
 * #ClutterSensatronEffect is an opaque structure
 * whose members cannot be accessed directly
 *
 * Since: 1.4
 */
typedef struct _ClutterSensatronEffect       ClutterSensatronEffect;
typedef struct _ClutterSensatronEffectClass  ClutterSensatronEffectClass;

GType clutter_sensatron_effect_get_type (void) G_GNUC_CONST;
ClutterEffect *clutter_sensatron_effect_new (void);

gboolean        clutter_shader_effect_set_shader_source (ClutterShaderEffect *effect,
                                                         const gchar         *source);

G_END_DECLS

#endif /* __CLUTTER_SENSATRON_EFFECT_H__ */