
#ifndef ___exo_marshal_MARSHAL_H__
#define ___exo_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,OBJECT (./exo-marshal.list:1) */
extern void _exo_marshal_VOID__OBJECT_OBJECT (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* BOOLEAN:VOID (./exo-marshal.list:2) */
extern void _exo_marshal_BOOLEAN__VOID (GClosure     *closure,
                                        GValue       *return_value,
                                        guint         n_param_values,
                                        const GValue *param_values,
                                        gpointer      invocation_hint,
                                        gpointer      marshal_data);

/* BOOLEAN:ENUM,INT (./exo-marshal.list:3) */
extern void _exo_marshal_BOOLEAN__ENUM_INT (GClosure     *closure,
                                            GValue       *return_value,
                                            guint         n_param_values,
                                            const GValue *param_values,
                                            gpointer      invocation_hint,
                                            gpointer      marshal_data);

G_END_DECLS

#endif /* ___exo_marshal_MARSHAL_H__ */

