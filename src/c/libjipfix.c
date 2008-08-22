#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>

#include <jni.h>
#include <ipfix.h>
#include "ipfix_col.h"
#include <de_fraunhofer_fokus_ants_jipfix_jIPFIX.h>
#include <mlog.h>
#include <mpoll.h>

#ifndef NTOHLL
#define HTONLL(val)  ((uint64_t)(htonl((uint32_t)((val)>>32))) | \
                          (((uint64_t)htonl((uint32_t)((val)&0xFFFFFFFF)))<<32))
#define NTOHLL(val)  ((uint64_t)(ntohl((uint32_t)((val)>>32))) | \
                          (((uint64_t)ntohl((uint32_t)((val)&0xFFFFFFFF)))<<32))
#endif

void checkException (JNIEnv * env) {
  if ((*env)->ExceptionOccurred(env)) {
    (*env)->ExceptionDescribe(env);
  }
}

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixInit
 * Signature: ()B
 */
JNIEXPORT jbyte JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixInit(JNIEnv * env, jobject self) {
  return ipfix_init();
  checkException(env);
}

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixCleanup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixCleanup(JNIEnv * env, jobject self) {
  ipfix_cleanup();
  checkException(env);
}

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixAddVendorInformationElements
 * Signature: ([Lde/fraunhofer/fokus/ants/jipfix/ipfixFieldType;)B
 */
JNIEXPORT jbyte JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixAddVendorInformationElements(
        JNIEnv * env, jobject self, jobjectArray elements)
{
    jsize len = (*env)->GetArrayLength(env, elements);
    ipfix_field_type_t *ipfix_fts = calloc(len+1, sizeof(ipfix_field_type_t));
    int i;

    jclass classFT = (*env)->FindClass(env, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXFieldType");
    jfieldID feno    = (*env)->GetFieldID(env, classFT, "eno", "I");
    jfieldID fftype  = (*env)->GetFieldID(env, classFT, "ftype", "I");
    jfieldID flength = (*env)->GetFieldID(env, classFT, "length", "I");
    jfieldID fcoding = (*env)->GetFieldID(env, classFT, "coding", "I");
    jfieldID fname   = (*env)->GetFieldID(env, classFT, "name", "Ljava/lang/String;");
    jfieldID fdoc    = (*env)->GetFieldID(env, classFT, "documentation", "Ljava/lang/String;");

    for (i=0; i<len; i++) {
        jobject element = (*env)->GetObjectArrayElement(env, elements,i);
        ipfix_fts[i].eno    = (*env)->GetIntField(env,element, feno);
        ipfix_fts[i].ftype  = (*env)->GetIntField(env,element, fftype);
        ipfix_fts[i].length = (*env)->GetIntField(env,element, flength);
        ipfix_fts[i].coding = (*env)->GetIntField(env,element, fcoding);

        jstring sname = (*env)->GetObjectField(env, element, fname);
        ipfix_fts[i].name = (char*)((*env)->GetStringUTFChars(env, sname, NULL));

        jstring sdoc = (*env)->GetObjectField(env, element, fdoc);
        ipfix_fts[i].documentation = (char*)((*env)->GetStringUTFChars(env, sdoc, NULL));
    }

    ipfix_fts[i].eno = 0;
    ipfix_fts[i].ftype = 0;
    ipfix_fts[i].length = 0;
    ipfix_fts[i].coding = 0;
    ipfix_fts[i].name = NULL;
    ipfix_fts[i].documentation = NULL;

    checkException(env);
    //return ipfix_add_vendor_information_elements( ipfix_ft_fokus );
    return ipfix_add_vendor_information_elements( ipfix_fts );
}

/** COLLECTOR *****************************************************************/

static JNIEnv *senv;
static jobject oCollectorListener;
static jmethodID mNewSource, mNewMessage, mTRecord, mDRecord, mCleanup;

static int        *tcp_s=NULL, ntcp_s=0;       /* socket */
static ipfix_col_info_t *g_colinfo =NULL;

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixCollectorInitCallbackExport
 * Signature: (Lde/fraunhofer/fokus/ants/jipfix/IPFIXRawCollectorListener;)V
 */

jobject buildInput(ipfix_input_t *i) {

   jint type=i->type;
   jstring ident= (*senv)->NewStringUTF(senv, ipfix_col_input_get_ident( i ));

   jclass cInput=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXInput");
   jmethodID mCon=(*senv)->GetMethodID(senv, cInput, "<init>", "(ILjava/net/InetAddress;Ljava/lang/String;)V");
   jobject oInput = (*senv)->NewObject(senv, cInput, mCon, type, 0, ident);
   return oInput;
 }

 jobject buildFieldType(ipfix_field_type_t *ft) {

   jint eno=ft->eno;
   jint ftype=ft->ftype;
   jint length=ft->length;
   jint coding=ft->coding;
   jstring name=(*senv)->NewStringUTF(senv, ft->name);
   jstring documentation=(*senv)->NewStringUTF(senv, ft->documentation);

   jclass cFT=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXFieldType");
   jmethodID mCon=(*senv)->GetMethodID(senv, cFT, "<init>", "(IIIILjava/lang/String;Ljava/lang/String;)V");
   jobject oFT = (*senv)->NewObject(senv, cFT, mCon, eno, ftype, length, coding, name, documentation);
   return oFT;
 }

 jobject buildField(ipfix_field_t *f) {
   if (f==0) return 0;

   jobject next=0; //buildField(f->next);
   jobject ft=buildFieldType(f->ft);

   jclass cF=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXField");
   jmethodID mCon=(*senv)->GetMethodID(senv, cF, "<init>", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXField;Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXFieldType;)V");
   jobject oF= (*senv)->NewObject(senv, cF, mCon, next, ft);
   return oF;
 }

 jobject buildTemplateField(ipfix_template_field_t *f) {

   jint flength=f->flength;
   jint unknown_f=f->unknown_f;
   jint relay_f=f->relay_f;
   jobject elem=buildField(f->elem);

   jclass cTF=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplateField");
   jmethodID mCon=(*senv)->GetMethodID(senv, cTF, "<init>", "(IIILde/fraunhofer/fokus/ants/jipfix/data/IPFIXField;)V");
   jobject oTF = (*senv)->NewObject(senv, cTF, mCon, flength, unknown_f, relay_f, elem);
   return oTF;
 }

 jobject buildTemplate(ipfix_template_t *t) {
   if (t==0) return 0;

   jobject next=buildTemplate(t->next);
   jint type=t->type;
   jlong tsend=t->tsend;

   jint tid=t->tid;
   jint ndatafields=t->ndatafields;
   jint nscopefields=t->nscopefields;
   jint nfields=t->nfields;

   jclass cField = (*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplateField");
   jobjectArray aFields=(*senv)->NewObjectArray(senv, nfields, cField, NULL);
   int i;
   for (i=0;i<nfields;i++) {
     jobject field=buildTemplateField(&(t->fields[i]));
     (*senv)->SetObjectArrayElement(senv, aFields, i, field);
   }
   jint maxfields=t->maxfields;

   jclass cTemplate=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplate");
   jmethodID mCon=(*senv)->GetMethodID(senv, cTemplate, "<init>", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplate;IJIIII[Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplateField;I)V");
   jobject oTemplate = (*senv)->NewObject(senv, cTemplate, mCon, next, type, tsend, tid, ndatafields, nscopefields, nfields, aFields, maxfields);
   return oTemplate;
 }

 jobject buildTNode(ipfixt_node_t *t) {
   if (t==0) return 0;

   jobject next=buildTNode(t->next);
   jlong expire_time=t->expire_time;
   jobject ipfixt = buildTemplate(t->ipfixt);
   jstring ident = (*senv)->NewStringUTF(senv, t->ident);

   jclass cTNode=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXTNode");
   jmethodID mCon=(*senv)->GetMethodID(senv, cTNode, "<init>", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXTNode;JLde/fraunhofer/fokus/ants/jipfix/data/IPFIXTemplate;Ljava/lang/String;)V");
   jobject oTNode = (*senv)->NewObject(senv, cTNode, mCon, next, expire_time, ipfixt, ident);
   return oTNode;
 }

 jobject buildSNode(ipfixs_node_t *s) {
   if (s==0) return 0;

   jobject next=buildSNode(s->next);
   jlong odid=s->odid;
   jobject input=buildInput(s->input);
   jobject templates=buildTNode(s->templates);
   jstring fname=(*senv)->NewStringUTF(senv, s->fname);
   jlong last_seqno=s->last_seqno;

   jclass cSNode=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXSNode");
   jmethodID mCon=(*senv)->GetMethodID(senv, cSNode, "<init>", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXSNode;JLde/fraunhofer/fokus/ants/jipfix/data/IPFIXInput;Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXTNode;Ljava/lang/String;J)V");
   jobject oSNode = (*senv)->NewObject(senv, cSNode, mCon, next, odid, input, templates, fname, last_seqno);
   return oSNode;
 }

 jobject buildHeader(ipfix_hdr_t *h) {
   jint version=h->version;
   jint count;
   jlong sysuptime=-1;
   jlong exporttime;
   if (version==IPFIX_VERSION_NF9) {
     count=h->u.nf9.count;
     sysuptime=h->u.nf9.sysuptime;
     exporttime=h->u.nf9.unixtime;
   } else {
     count=h->u.ipfix.length;
     exporttime=h->u.ipfix.exporttime;
   }
   jlong seqno=h->seqno;
   jlong sourceid=h->sourceid;

   jclass cHeader=(*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXHeader");
   jmethodID mCon=(*senv)->GetMethodID(senv, cHeader, "<init>", "(IIJJJJ)V");
   jobject oHeader = (*senv)->NewObject(senv, cHeader, mCon, version, count, sysuptime, exporttime, seqno, sourceid);
   return oHeader;
 }

 jobject buildDataRecord(ipfix_datarecord_t *d, ipfixt_node_t *t) {
     jclass cObject = (*senv)->FindClass(senv, "java/lang/Object");
     int nfields = t->ipfixt->nfields;
     jobjectArray addrs = (*senv)->NewObjectArray(senv, nfields, cObject, NULL);
     int i;
     for (i=0; i<nfields; i++) {
         int len = d->lens[i];
         int coding = t->ipfixt->fields[i].elem->ft->coding;
         jobject record;
         jbyteArray value = (*senv)->NewByteArray(senv, len);
         (*senv)->SetByteArrayRegion(senv, value, 0, len, d->addrs[i]);

         switch (coding) {
         case IPFIX_CODING_INT:
         case IPFIX_CODING_NTP:    //todo: convert into java/util/Timestamp
         case IPFIX_CODING_UINT: { //todo: make unsigned (increase [B len by 1?)
             jclass cBigInteger = (*senv)->FindClass(senv, "java/math/BigInteger");
             jmethodID mCon = (*senv)->GetMethodID(senv, cBigInteger, "<init>", "([B)V");
             record = (*senv)->NewObject(senv, cBigInteger, mCon, value);
             break;
         }
         case IPFIX_CODING_IPADDR: {
             jclass cInetAddress = (*senv)->FindClass(senv, "java/net/InetAddress");
             jmethodID mCon = (*senv)->GetStaticMethodID(senv, cInetAddress, "getByAddress", "([B)Ljava/net/InetAddress;");
             record = (*senv)->CallStaticObjectMethod(senv, cInetAddress, mCon, value);
             break;
         }
         case IPFIX_CODING_STRING: {
             jclass cString = (*senv)->FindClass(senv, "java/lang/String");
             jmethodID mCon = (*senv)->GetMethodID(senv, cString, "<init>", "([B)V");
             record = (*senv)->NewObject(senv, cString, mCon, value);
             break;
         }
         case IPFIX_CODING_FLOAT: {
             uint32_t tmp32;
             uint64_t tmp64;
             float tmpfloat;
             double tmpdouble;
             switch ( len ) {
               case 4:
                   memcpy( &tmp32, d->addrs[i], 8);
                   tmp32 = htonl( tmp32);
                   memcpy( &tmpfloat, &tmp32, 8);
                   jclass cFloat = (*senv)->FindClass(senv, "java/lang/Float");
                   jmethodID mCon = (*senv)->GetMethodID(senv, cFloat, "<init>", "(F)V");
                   record = (*senv)->NewObject(senv, cFloat, mCon, tmpfloat);
               case 8: {
                   memcpy( &tmp64, d->addrs[i], 8);
                   tmp64 = HTONLL( tmp64);
                   memcpy( &tmpdouble, &tmp64, 8);
                   jclass cDouble = (*senv)->FindClass(senv, "java/lang/Double");
                   jmethodID mCon = (*senv)->GetMethodID(senv, cDouble, "<init>", "(D)V");
                   record = (*senv)->NewObject(senv, cDouble, mCon, tmpdouble);
               }
             }
//             jclass cBigDecimal = (*senv)->FindClass(senv, "java/math/BigDecimal");
//             jmethodID mCon = (*senv)->GetMethodID(senv, cBigDecimal, "<init>", "([B)V");
//             record = (*senv)->NewObject(senv, cBigDecimal, mCon, value);
             break;
         }
         case IPFIX_CODING_BYTES:
         default: {
             record = value;
             break;
         }
         }

         (*senv)->SetObjectArrayElement(senv, addrs, i, record);
     }
     jint maxfields = d->maxfields;

     jclass cDR = (*senv)->FindClass(senv, "de/fraunhofer/fokus/ants/jipfix/data/IPFIXDataRecord");
     jmethodID mCon = (*senv)->GetMethodID(senv, cDR, "<init>", "([Ljava/lang/Object;I)V");
     jobject oDR = (*senv)->NewObject(senv, cDR, mCon, addrs, maxfields);
     return oDR;
 }

 int export_newsource_cb( ipfixs_node_t *s, void *arg ) {
   jobject js = buildSNode(s);
   (*senv)-> CallVoidMethod(senv, oCollectorListener, mNewSource, js);
   (*senv)->ExceptionDescribe(senv); //we've just called a callback! the user might do anything!
   return 0;
 }
 int export_newmsg_cb( ipfixs_node_t *s, ipfix_hdr_t *hdr, void *arg ) {
   jobject jhdr = buildHeader(hdr);
   (*senv)-> CallVoidMethod(senv, oCollectorListener, mNewMessage, jhdr);
   (*senv)->ExceptionDescribe(senv); //we've just called a callback! the user might do anything!
   return 0;
 }
 int export_trecord_cb( ipfixs_node_t *s, ipfixt_node_t *t, void *arg ) {
   jlong js = s->odid;
   jobject jt = buildTNode(t);
   (*senv)-> CallVoidMethod(senv, oCollectorListener, mTRecord, js, jt);
   (*senv)->ExceptionDescribe(senv); //we've just called a callback! the user might do anything!
   return 0;
 }
 int export_drecord_cb( ipfixs_node_t      *s,
                                ipfixt_node_t      *t,
                                ipfix_datarecord_t *data,
                                void               *arg ) {
   jlong js = s->odid;
   jint jt = t->ipfixt->tid;
   jobject jdata = buildDataRecord(data, t);
   (*senv)-> CallVoidMethod(senv, oCollectorListener, mDRecord, js, jt, jdata);
   (*senv)->ExceptionDescribe(senv); //we've just called a callback! the user might do anything!
   return 0;
 }
 void export_cleanup_cb( void *arg ) {
   (*senv)-> CallVoidMethod(senv, oCollectorListener, mCleanup);
   (*senv)->ExceptionDescribe(senv); //we've just called a callback! the user might do anything!
   (*senv)->DeleteGlobalRef(senv, oCollectorListener);
 }

JNIEXPORT jbyte JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixCollectorInitCallbackExport(JNIEnv *env, jobject self, jobject listener) {
  senv=env;
  oCollectorListener= (*env)->NewGlobalRef(env, listener);

  jclass cListener = (*env)-> GetObjectClass(env, listener);

  mNewSource = (*env)->GetMethodID(env, cListener, "sourceCollected", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXSNode;)V");
  mNewMessage = (*env)->GetMethodID(env, cListener, "messageCollected", "(Lde/fraunhofer/fokus/ants/jipfix/data/IPFIXHeader;)V");
  mTRecord = (*env)->GetMethodID(env, cListener, "tRecordCollected", "(JLde/fraunhofer/fokus/ants/jipfix/data/IPFIXTNode;)V");
  mDRecord = (*env)->GetMethodID(env, cListener, "dRecordCollected", "(JILde/fraunhofer/fokus/ants/jipfix/data/IPFIXDataRecord;)V");
  mCleanup = (*env)->GetMethodID(env, cListener, "cleanupCollector", "()V");

  if ( (g_colinfo=calloc( 1, sizeof(ipfix_col_info_t))) ==NULL) {
    checkException(env);
    return -1;
  }

  g_colinfo->export_newsource = export_newsource_cb;
  g_colinfo->export_newmsg    = export_newmsg_cb;
  g_colinfo->export_trecord   = export_trecord_cb;
  g_colinfo->export_drecord   = export_drecord_cb;
  g_colinfo->export_cleanup   = export_cleanup_cb;
  g_colinfo->data = NULL;

  checkException(env);
  return ipfix_col_register_export( g_colinfo );
}


/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixCollectorInitFileExport
 * Signature: (Ljava/io/File;)V
 */
JNIEXPORT void JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixCollectorInitFileExport(JNIEnv *env, jobject self, jstring dir) {
  char * datadir = (char*)((*env) -> GetStringUTFChars(env, dir, NULL));
  ipfix_col_init_fileexport( datadir );
  checkException(env);
}


/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixCollectorListen
 * Signature: (IIII)V
 */
JNIEXPORT jbyte JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixCollectorListen(JNIEnv *env, jobject self, jint protocol, jint port, jint family, jint maxcon) {
  if ( ipfix_col_listen( &ntcp_s, &tcp_s, protocol, port, family, 10 ) <0 ) {
    checkException(env);
    return -1;
  }
  checkException(env);
  return 0;
}

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    ipfixCollectorCleanup
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_ipfixCollectorCleanup(JNIEnv *env, jobject self) {
   checkException(env);
   if ( tcp_s ) {
    int i;
    for( i=0; i<ntcp_s; i++ ) {
      mpoll_fdrm( tcp_s[i] );
      close( tcp_s[i] );
    }
  }
  ipfix_col_cleanup();
  checkException(env);
}

/** MPOLL / MLOG **************************************************************/
static JNIEnv *menv;

static jobject oExitListener;
static jmethodID mExit;

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    mpollLoop
 * Signature: (I)V
 */
void mpoll_exit_func(int signo) {
  (*menv)-> CallVoidMethod(menv, oExitListener, mExit);
  (*menv)-> DeleteGlobalRef(menv, oCollectorListener);
  exit( 1 );
}

JNIEXPORT void JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_mpollLoop(JNIEnv *env, jobject self, jint timeout, jobject listener) {
  menv=env;
  oExitListener= (*env)->NewGlobalRef(env, listener);

  jclass cListener = (*env)-> GetObjectClass(env, listener);
  mExit = (*env)->GetMethodID(env, cListener, "mpollLoopExit", "()V");

  if (mExit >0 ) {
    signal( SIGKILL, mpoll_exit_func );
    signal( SIGTERM, mpoll_exit_func );
    signal( SIGINT,  mpoll_exit_func );

    mpoll_loop(timeout);
  }
  checkException(env);
}

/*
 * Class:     de_fraunhofer_fokus_ants_jipfix_jIPFIX
 * Method:    mlogSetVerbosityLevel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_de_fraunhofer_fokus_ants_jipfix_jIPFIX_mlogSetVerbosityLevel(JNIEnv *env, jobject self, jint level) {
  mlog_set_vlevel(level);
  checkException(env);
}




