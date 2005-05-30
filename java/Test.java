import org.xfce.exo.*;
import org.gnu.gtk.*;


public class Test {
  private static final String[] FILENAMES = { "Class.java", "script.sh", "library.so", "source.c", "class.C" };
  private static final String[] FILEPATHS = { "Test.class", "Test.java", "/dev/null", "/vmlinuz" };

  public static void main(String[] args) {
    Gtk.init(args);
    MimeDatabase db = MimeDatabase.getDefault();
    for (int n = 0; n < FILENAMES.length; ++n) {
      MimeInfo info = db.getInfoFromFileName(FILENAMES[n]);
      System.out.println("File name " + FILENAMES[n] + " has type " + info.getName() + " (" + info.getComment() + ")");
    }
    for (int n = 0; n < FILEPATHS.length; ++n) {
      MimeInfo info = db.getInfoForFile(new java.io.File(FILEPATHS[n]));
      System.out.println("File path " + FILEPATHS[n] + " has type " + info.getName() + " (" + info.getComment() + ")");
    }
  }
}
