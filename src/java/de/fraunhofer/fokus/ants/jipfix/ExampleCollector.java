package de.fraunhofer.fokus.ants.jipfix;

import de.fraunhofer.fokus.ants.jipfix.data.*;
import com.thoughtworks.xstream.*; 

public class ExampleCollector implements IPFIXCollectorListener {
  XStream xstream=new XStream();
  
/**
 * called when a new source is detected
 * @param s source 
 */
  public void sourceCollected(IPFIXSNode s) {
    System.out.println("--[[ SOURCE DETECTED ]]--");
    System.out.println(xstream.toXML(s));
    System.out.println("-------------------------");
  }
/**
 * called when a new message begins
 * @param hdr IPFIX header
 */
  public void messageCollected(IPFIXHeader hdr) {
    System.out.println("--[[ INCOMING MESSAGE ]]--");
    System.out.println(xstream.toXML(hdr));
    System.out.println("--------------------------");
  }
/**
 * called when a new template record is received
 * @param s source
 * @param t template
 */
  public void templateRecordCollected(IPFIXSNode s, IPFIXTNode t) {
    System.out.println("--[[ NEW TEMPLATE RECEIVED ]]--");
    System.out.println("Source ID: "+s.odid);
    System.out.println(xstream.toXML(t));
    System.out.println("-------------------------------");
  }
/**
 * called when a new data record is received
 * @param s source
 * @param t template
 * @param data data record
 */

  public void dataRecordCollected(IPFIXSNode s, IPFIXTNode t, IPFIXDataRecord data) {
    System.out.println("--[[ NEW DATA RECORD ]]--");
    System.out.println("Source ID: "+s.odid);
    System.out.println("Template ID: "+t.ipfixt.tid);
    System.out.println(xstream.toXML(data));
    System.out.println("-------------------------");
  }
/**
 * called before the collector closes down 
 */
  public void cleanupCollector() {
    System.out.println("--[[ COLLECTOR CLOSING ]]--");
  }
}




