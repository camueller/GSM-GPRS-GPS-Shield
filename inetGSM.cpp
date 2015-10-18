#include "inetGSM.h"
#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

int InetGSM::httpGET(const char* server, int port, const char* path, char* result, int resultlength)
{
  boolean connected=false;
  int n_of_at=0;
  int length_write;
  char end_c[2];
  end_c[0]=0x1a;
  end_c[1]='\0';

  /*
  Status = ATTACHED.
  if(gsm.getStatus()!=GSM::ATTACHED)
    return 0;
  */
  while(n_of_at<3){
	  if(!connectTCP(server, port)){
	  	#ifdef DEBUG_ON
			Serial.println("DB:NOT CONN");
		#endif	
	    	n_of_at++;
	  }
	  else{
		connected=true;
		n_of_at=3;
	}
  }

  if(!connected) return 0;
	
  gsm.SimpleWrite("GET ");
  gsm.SimpleWrite(path);
  gsm.SimpleWrite(" HTTP/1.0\nHost: ");
  gsm.SimpleWrite(server);
  gsm.SimpleWrite("\n");
  gsm.SimpleWrite("User-Agent: Arduino");
  gsm.SimpleWrite("\n\n");
  gsm.SimpleWrite(end_c);

  switch(gsm.WaitResp(10000, 10, "SEND OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  
  
 delay(50);
  	#ifdef DEBUG_ON
		Serial.println("DB:SENT");
	#endif	
  int res= gsm.read(result, resultlength);

  //gsm.disconnectTCP();
  
  //int res=1;
  return res;
}


int InetGSM::attachGPRS(char* domain, char* dom1, char* dom2)
{
   int i=0;
   delay(5000);
   
  //gsm._tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.
  gsm.WaitResp(50, 50);
  gsm.SimpleWriteln("AT+CIFSR");
  if(gsm.WaitResp(5000, 50, "ERROR")!=RX_FINISHED_STR_RECV){
  	#ifdef DEBUG_ON
		Serial.println("DB:ALREADY HAVE AN IP");
	#endif
	  gsm.SimpleWriteln("AT+CIPCLOSE");
	gsm.WaitResp(5000, 50, "ERROR");
	delay(2000);
	gsm.SimpleWriteln("AT+CIPSERVER=0");
	gsm.WaitResp(5000, 50, "ERROR");
	return 1;
  }
  else{

	#ifdef DEBUG_ON
		Serial.println("DB:STARTING NEW CONNECTION");
	#endif
  
  gsm.SimpleWriteln("AT+CIPSHUT");
  
  switch(gsm.WaitResp(500, 50, "SHUT OK")){

	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
	#ifdef DEBUG_ON
		Serial.println("DB:SHUTTED OK");
	#endif
	 delay(1000);
	 
  gsm.SimpleWrite("AT+CSTT=\"");
  gsm.SimpleWrite(domain);
  gsm.SimpleWrite("\",\"");
  gsm.SimpleWrite(dom1);
  gsm.SimpleWrite("\",\"");
  gsm.SimpleWrite(dom2);
  gsm.SimpleWrite("\"\r");  

  
  switch(gsm.WaitResp(500, 50, "OK")){

	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
	#ifdef DEBUG_ON
		Serial.println("DB:APN OK");
	#endif
	 delay(5000);
	  
	gsm.SimpleWriteln("AT+CIICR");  

  switch(gsm.WaitResp(10000, 50, "OK")){
	case RX_TMOUT_ERR: 
		return 0; 
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  	#ifdef DEBUG_ON
		Serial.println("DB:CONNECTION OK");
	#endif

  delay(1000);


 gsm.SimpleWriteln("AT+CIFSR");
 if(gsm.WaitResp(5000, 50, "ERROR")!=RX_FINISHED_STR_RECV){
	#ifdef DEBUG_ON
		Serial.println("DB:ASSIGNED AN IP");
	#endif
	gsm.setStatus(gsm.ATTACHED);
	return 1;
}
	#ifdef DEBUG_ON
		Serial.println("DB:NO IP AFTER CONNECTION");
	#endif
 return 0;
 }
}

int InetGSM::connectTCP(const char* server, int port)
{
  //gsm._tf.setTimeout(_TCP_CONNECTION_TOUT_);

  //Status = ATTACHED.
  //if (getStatus()!=ATTACHED)
    //return 0;

  //_cell.flush();
  
  //Visit the remote TCP server.
   gsm.SimpleWrite("AT+CIPSTART=\"TCP\",\"");
   gsm.SimpleWrite(server);
   gsm.SimpleWrite("\",");
   gsm.SimpleWriteln(port);
  
  switch(gsm.WaitResp(1000, 200, "OK")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }
  #ifdef DEBUG_ON
	Serial.println("DB:RECVD CMD");
  #endif	
  if (!gsm.IsStringReceived("CONNECT OK")) {
    switch(gsm.WaitResp(15000, 200, "OK")) {
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
    }
  }

  #ifdef DEBUG_ON
	Serial.println("DB:OK TCP");
  #endif

  delay(3000);
  gsm.SimpleWriteln("AT+CIPSEND");
  switch(gsm.WaitResp(5000, 200, ">")){
	case RX_TMOUT_ERR: 
		return 0;
	break;
	case RX_FINISHED_STR_NOT_RECV: 
		return 0; 
	break;
  }

  #ifdef DEBUG_ON
	Serial.println("DB:>");
  #endif
  delay(4000);
  return 1;
}
