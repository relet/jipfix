package de.fraunhofer.fokus.ants.jipfix.data;

public class IPFIXDataRecord {

	/**
	 * may contain one of the following data types, depending on the definition
	 * in the template:
	 * <ul>
	 * <li>java.math.BigInteger</li>
	 * <li>java.lang.Float</li>
	 * <li>java.lang.Double</li>
	 * <li>java.net.InetAddress</li>
	 * <li>java.lang.String</li>
	 * <li>java.util.Timestamp</li>
	 * <li>array of bytes</li>
	 * </ul>
	 */
	public Object[] addrs;
	public int maxfields;

	public IPFIXDataRecord(Object[] a, int m) {
		addrs = a;
		maxfields = m;
	}
}
