package org.xfce.exo;

import org.gnu.glib.GObject;

public final class MimeInfo extends GObject {
  MimeInfo(int handle) {
    super(handle);
  }

  public String getComment() {
    return exo_mime_info_get_comment(getHandle());
  }

  public String getName() {
    return exo_mime_info_get_name(getHandle());
  }


  private static final native String exo_mime_info_get_comment(int handle);
  private static final native String exo_mime_info_get_name(int handle);
}
