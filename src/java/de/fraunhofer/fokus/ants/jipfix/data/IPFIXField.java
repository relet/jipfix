package de.fraunhofer.fokus.ants.jipfix.data;

public class IPFIXField {

  public IPFIXField next;
  public IPFIXFieldType ft;
  
  public IPFIXField(IPFIXField n, IPFIXFieldType t) {
    next=n;
    ft=t;
  }
}
