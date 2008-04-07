package de.fraunhofer.fokus.ants.jipfix;

import java.io.File;
import java.util.Hashtable;
import de.fraunhofer.fokus.ants.jipfix.data.*;

/**
 * An utility wrapper intended to control the FOKUS libipfix library
 * from Java.  
 */

public class jIPFIX implements MPollLoopExitListener, IPFIXRawCollectorListener {
  static {
    System.loadLibrary("jipfix");
  }

/* PUBLIC DEFINITONS *********************************************************/  
  
  /* some useful consts */
  /** Network address family IPv4 */
  public static final int AF_INET = 2;

  /** IPFIX protocol identifiers */
  public static final int IPFIX_PROTO_TCP  = 6;
  public static final int IPFIX_PROTO_SCTP = 132;
  public static final int IPFIX_PROTO_UDP  = 17;

  /* public methods */
  /**
  * initialize an IPFIX Collector which invokes callback functions on 
  * interesting events.
  * @param  listen  an IPFIXCollectorListener implementing the callback 
  *                 functions. 
  */
  public void initCollector(IPFIXCollectorListener listen) {
    myInitCollector();
    myCollect(listen);
  }

  /**
  * initialize an IPFIX Collector which writes its information into files.
  * @param  directory a base directory to write information in. 
  */
  public void initFileCollector(File directory) {
    myInitCollector();
    myFileCollect(directory);
  }
  /**
  * start collecting IPFIX messages.
  * @param  proto     IPFIX protocol to use, e.g. IPFIX_PROTO_SCTP 
  * @param  port      Server port to listen on 
  * @param  family    Consider this network address family, e.g. AF_INET for IPv4 
  * @param  maxcon    Maximum number of accepted connections.
  * @param  duration  listen for this many seconds or -1 for infinite. 
  */
  public void startCollector(int proto, int port, int family, int maxcon, int duration) {
    myStartCollector(proto, port, family, maxcon, duration);
  }
  /**
  * stop collecting IPFIX messages.
  */
  public void stopCollector() {
    myCleanup();
  }

  /**
  * retreive available information for a specific source identifier
  * @param odid the source identifier
  * @return     a source (SNode) object
  */
  public IPFIXSNode getSourceByID(long odid) {
    Object o=sources.get(new Long(odid));
    return (o==null)?null:(IPFIXSNode)o;
  }
  /**
  * retreive available information for a specific template identifier
  * @param tid the template identifier
  * @return    a template (TNode) object
  */
  public IPFIXTNode getTemplateByID(int tid) {
    Object o=templates.get(new Integer(tid));
    return (o==null)?null:(IPFIXTNode)o;
  }
  
  /**
  * start a simple exampleCollecter that does nothing.
  */
  public static void main(String[] args) {
    new jIPFIX().exampleCollector();    
  }

/* PRIVATE DEFINITONS ********************************************************/  
  /* private fields */
  private Hashtable sources=new Hashtable();
  private Hashtable templates=new Hashtable();
  private IPFIXCollectorListener listener;
  
  private void exampleCollector() {
    myInitCollector();
    myCollect(new ExampleCollector()); //exampleListener needed
    myStartCollector(IPFIX_PROTO_TCP, 4739, AF_INET, 10, -1);
  }
  
  private void myInitCollector() { //throws jIPFIXException
    mlogSetVerbosityLevel(0);
    if (ipfixInit()<0)  {
      System.err.println("IPFIX Initialization failed,");
      System.exit(1);
    }
  }
  /* this is optional. 
    IPFIXFieldType[] elements = IPFIXFieldType.readFile("ipfix_fields_fokus.xml");
    if (ipfixAddVendorInformationElements(elements)<0) {
      System.err.println("IPFIX AddVIE failed.");
      System.exit(1);
    }
  */
  private void myCollect(IPFIXCollectorListener listen) {
    this.listener=listen;
    ipfixCollectorInitCallbackExport(this);
  }
  private void myFileCollect(File directory) {
    ipfixCollectorInitFileExport(directory.getName());
  }
  private void myStartCollector(int proto, int port, int family, int maxcon, int duration) {
    if (ipfixCollectorListen(proto, port, family, maxcon)<0) {
      System.err.println("IPFIX CollectorListen failed.");
      System.exit(1);
    }
    mpollLoop(duration, this);
    myCleanup();
  }
  private void myCleanup() {
    ipfixCollectorCleanup();
    ipfixCleanup();
  }
  
/* PUBLIC LISTENER METHODS - DO NOT CALL THESE ********************************/
  public void sourceCollected(IPFIXSNode s) {
    sources.put(new Long(s.odid), s);
    if (listener!=null) listener.sourceCollected(s);
  }
  public void messageCollected(IPFIXHeader hdr) {
    hdr.source=getSourceByID(hdr.sourceid);
    if (listener!=null) listener.messageCollected(hdr);
  }
  public void tRecordCollected(long sourceid, IPFIXTNode t) {
    templates.put(new Integer(t.ipfixt.tid),t);
    //todo: append t to source.templates;
    IPFIXSNode source=getSourceByID(sourceid);
    if (listener!=null) listener.templateRecordCollected(source, t);
  }
  public void dRecordCollected(long sourceid, int templateid, IPFIXDataRecord data) {
    IPFIXSNode source=getSourceByID(sourceid);
    IPFIXTNode template=getTemplateByID(templateid);
    if (listener!=null) listener.dataRecordCollected(source, template, data);
  }
  public void cleanupCollector() {
    if (listener!=null) listener.cleanupCollector(); //optional
  }
  
  public void mpollLoopExit() {
    myCleanup();
  }

/* NATIVE METHODS *************************************************************/
  
  //TODO: input checking for all native functions on java side!
  private native byte ipfixInit();
  private native byte ipfixAddVendorInformationElements(IPFIXFieldType[] elements);
  private native void ipfixCleanup();
  //Collector
  private native byte ipfixCollectorInitCallbackExport(IPFIXRawCollectorListener l);
  private native void ipfixCollectorInitFileExport(String datadir);
  private native byte ipfixCollectorListen(int protocol, int port, int family, int maxcon);
  private native void ipfixCollectorCleanup();
  //mlog/mpoll
  private native void mlogSetVerbosityLevel(int level);
  private native void mpollLoop(int timeout, MPollLoopExitListener l);

}
