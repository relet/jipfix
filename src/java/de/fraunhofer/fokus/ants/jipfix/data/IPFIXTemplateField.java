package de.fraunhofer.fokus.ants.jipfix.data;

public class IPFIXTemplateField {

  public int flength;
  public int unknown_f;
  public int relay_f;
  public IPFIXField elem;
  
  public IPFIXTemplateField(int f, int u, int r, IPFIXField e) {
    flength=f;
    unknown_f=u;
    relay_f=r;
    elem=e;
  }
}
