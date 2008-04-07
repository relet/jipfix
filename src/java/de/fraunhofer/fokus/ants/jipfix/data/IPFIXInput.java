package de.fraunhofer.fokus.ants.jipfix.data;

import java.net.InetAddress;

public class IPFIXInput {
  public static final int IPFIX_INPUT_FILE = 1;
  public static final int IPFIX_INPUT_IPCON = 2;
  
  public int type;
  public InetAddress address; //sockaddr?
  public String name;
  
  public IPFIXInput (String n) {
    this(IPFIX_INPUT_FILE, null, n);
  }
  public IPFIXInput (InetAddress a) {
    this(IPFIX_INPUT_IPCON, a, null);
  }
  public IPFIXInput (int t, InetAddress a, String n) {
    type=t;
    address=a;
    name=n;
  }
}
