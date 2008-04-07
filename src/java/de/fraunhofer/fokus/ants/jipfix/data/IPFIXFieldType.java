package de.fraunhofer.fokus.ants.jipfix.data;

import java.io.*;
import com.thoughtworks.xstream.*;
import de.fraunhofer.fokus.ants.jipfix.*;

public class IPFIXFieldType {
  public int eno;                /* enterprise number or 0 */
  public int ftype;              /* field type */
  public int length;             /* field length (ssize_t actually!) */
  public int coding;
  public String name;
  public String documentation;
  
  public IPFIXFieldType(int eno, int ftype, int length, int coding, String name, String documentation) {
    this.eno=eno;
    this.ftype=ftype;
    this.length=length;
    this.coding=coding;
    this.name=name;
    this.documentation=documentation;
  }
  
  public static IPFIXFieldType[] readFile(String filename) {
    try {
      XStream xstream=new XStream();
      xstream.alias("field", IPFIXFieldType.class);
      IPFIXFieldType[] data = (IPFIXFieldType[])xstream.fromXML(new FileInputStream(filename));
      return data;
    } catch (Exception ex) {
      ex.printStackTrace();
    }
    return null;
  }
}
