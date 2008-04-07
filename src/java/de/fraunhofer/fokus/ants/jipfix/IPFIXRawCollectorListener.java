package de.fraunhofer.fokus.ants.jipfix;

import de.fraunhofer.fokus.ants.jipfix.data.*;

interface IPFIXRawCollectorListener {
  public void sourceCollected(IPFIXSNode s);
  public void messageCollected(IPFIXHeader hdr);
  public void tRecordCollected(long sourceid, IPFIXTNode t);
  public void dRecordCollected(long sourceid, int templateid, IPFIXDataRecord data);
  public void cleanupCollector();
}
