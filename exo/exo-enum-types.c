
/* Generated data (by glib-mkenums) */

#undef GTK_DISABLE_DEPRECATED
#define GTK_ENABLE_BROKEN
#include <exo/exo.h>


/* enumerations from "exo-pango-extensions.h" */
GType
exo_pango_ellipsize_mode_get_type (void)
{
	static GType type = 0;
	if (type == 0) {
	static const GEnumValue values[] = {
	{ EXO_PANGO_ELLIPSIZE_NONE, "EXO_PANGO_ELLIPSIZE_NONE", "exo-pango-ellipsize-none" },
	{ EXO_PANGO_ELLIPSIZE_START, "EXO_PANGO_ELLIPSIZE_START", "exo-pango-ellipsize-start" },
	{ EXO_PANGO_ELLIPSIZE_MIDDLE, "EXO_PANGO_ELLIPSIZE_MIDDLE", "exo-pango-ellipsize-middle" },
	{ EXO_PANGO_ELLIPSIZE_END, "EXO_PANGO_ELLIPSIZE_END", "exo-pango-ellipsize-end" },
	{ 0, NULL, NULL }
	};
	type = g_enum_register_static ("ExoPangoEllipsizeMode", values);
  }
	return type;
}


/* Generated data ends here */

