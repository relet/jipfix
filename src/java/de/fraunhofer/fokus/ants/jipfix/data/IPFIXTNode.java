package de.fraunhofer.fokus.ants.jipfix.data;

public class IPFIXTNode {
  
  public IPFIXTNode next;
  public long expire_time;
  public IPFIXTemplate ipfixt;
  public String ident;
  
  public IPFIXTNode(IPFIXTNode n, long e, IPFIXTemplate t, String i) {
    next=n;
    expire_time=e;
    ipfixt=t;
    ident=i;
  }
}
