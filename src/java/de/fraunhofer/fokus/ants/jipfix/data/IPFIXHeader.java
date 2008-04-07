package de.fraunhofer.fokus.ants.jipfix.data;
import de.fraunhofer.fokus.ants.jipfix.jIPFIX;

public class IPFIXHeader {
  public int version;
  public int count; //aka length
  public long sysuptime;
  public long exporttime;
  public long seqno;
  public long sourceid;
  public IPFIXSNode source;
  
  public IPFIXHeader(int v, int c, long u, long e, long s, long sid) {
    version=v;
    count=c;
    sysuptime=u;
    exporttime=e;
    seqno=s;
    sourceid=sid;
  }
}
