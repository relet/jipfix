package de.fraunhofer.fokus.ants.jipfix.data;
import de.fraunhofer.fokus.ants.jipfix.jIPFIX;

public class IPFIXSNode {
  //public IPFIXSNode next; //may not always be set
  public long odid;
  public IPFIXInput input;
  public IPFIXTNode templates;
  public long last_seqno;
  
  public IPFIXSNode(IPFIXSNode n, long o, IPFIXInput i, IPFIXTNode t, String f, long l) { 
    //next=n;
    odid=o;
    input=i;
    templates=t;
    last_seqno=l;
  }
}
