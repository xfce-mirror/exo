package org.xfce.exo;

import java.io.File;
import org.gnu.glib.GObject;

public final class MimeDatabase extends GObject {
  private MimeDatabase(int handle) {
    super(handle);
  }

  public static MimeDatabase getDefault() {
    return new MimeDatabase(exo_mime_database_get_default());
  }

  public MimeInfo getInfo(String name) {
    int handle = exo_mime_database_get_info(getHandle(), name);
    MimeInfo info = (MimeInfo)retrieveGObject(handle);
    if (info == null)
      info = new MimeInfo(handle);
    return info;
  }

  public MimeInfo getInfoForFile(File file) {
    int handle = exo_mime_database_get_info_for_file(getHandle(), file.getAbsolutePath());
    MimeInfo info = (MimeInfo)retrieveGObject(handle);
    if (info == null)
      info = new MimeInfo(handle);
    return info;
  }

  public MimeInfo getInfoFromFileName(String fileName) {
    int handle = exo_mime_database_get_info_from_file_name(getHandle(), fileName);
    MimeInfo info = (MimeInfo)retrieveGObject(handle);
    if (info == null)
      info = new MimeInfo(handle);
    return info;
  }


  private static final native int exo_mime_database_get_default();
  private static final native int exo_mime_database_get_info(int handle, String name);
  private static final native int exo_mime_database_get_info_for_file(int handle, String filePath);
  private static final native int exo_mime_database_get_info_from_file_name(int handle, String fileName);


  static {
    System.loadLibrary("exojava0.3");
  }
}
