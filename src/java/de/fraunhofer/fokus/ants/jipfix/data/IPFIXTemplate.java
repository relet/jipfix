package de.fraunhofer.fokus.ants.jipfix.data;

public class IPFIXTemplate {
  public final static int DATA_TEMPLATE=1;
  public final static int OPTION_TEMPLATE=2;
  
  public IPFIXTemplate next;
  public int type;
  public long tsend;
  
  public int tid;
  public int ndatafields;
  public int nscopefields;
  public int nfields;
  public IPFIXTemplateField[] fields;
  public int maxfields;
  
  public IPFIXTemplate(IPFIXTemplate n, int t, long ts, int ti, int nd, int ns, int nf, IPFIXTemplateField[] f, int m) {
    next=n;
    type=t;
    tsend=ts;
    tid=ti;
    ndatafields=nd;
    nscopefields=ns;
    nfields=nf;
    fields=f;
    maxfields=m;
  }
}
