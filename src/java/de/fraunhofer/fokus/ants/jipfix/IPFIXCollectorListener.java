package de.fraunhofer.fokus.ants.jipfix;

import de.fraunhofer.fokus.ants.jipfix.data.*;

/**
 * Implement these methods to be able to receive IPFIX information elements  
 */

public interface IPFIXCollectorListener {
/**
 * called when a new source is detected
 * @param s source 
 */
  public void sourceCollected(IPFIXSNode s);
/**
 * called when a new message begins
 * @param hdr IPFIX header
 */
  public void messageCollected(IPFIXHeader hdr);
/**
 * called when a new template record is received
 * @param s source
 * @param t template
 */
  public void templateRecordCollected(IPFIXSNode s, IPFIXTNode t);
/**
 * called when a new data record is received
 * @param s source
 * @param t template
 * @param data data record
 */
  public void dataRecordCollected(IPFIXSNode s, IPFIXTNode t, IPFIXDataRecord data);
/**
 * called before the collector closes down 
 */
  public void cleanupCollector();
}
