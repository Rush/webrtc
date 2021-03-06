extern "C" {
#include <ILibParsers.h>
#include <ILibAsyncSocket.h>
#include <ILibWebRTC.h>
#include <ILibWrapperWebRTC.h>
}

#include <initializer_list>

static void* chain;

void setupCtrlC() {
  signal(SIGPIPE, SIG_IGN); // Set a SIGNAL on Linux to listen for Ctrl-C

  // Shutdown on Ctrl + C
  signal(SIGINT, [](int s) {
    signal(SIGINT, SIG_IGN);	// To ignore any more ctrl c interrupts
  	ILibStopChain(chain);
  });
  {
    struct sigaction act; 
    act.sa_handler = SIG_IGN; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags = 0; 
    sigaction(SIGPIPE, &act, NULL);
  }
}

static const char* stunServerList[] = { "stun01.sipphone.com", "stun.ekiga.net", "stun.ideasip.com", "stun.schlund.de", "stunserver.org", "stun.softjoys.com", "stun.voiparound.com", "stun.voipbuster.com", "stun.voipstunt.com", "stun.voxgratia.org" };

int main()
{
  setupCtrlC();
  setbuf(stdout, NULL);
  setbuf(stdin, NULL);

  
  chain = ILibCreateChain();
  auto mConnectionFactory = ILibWrapper_WebRTC_ConnectionFactory_CreateConnectionFactory(chain, 0);
  
  auto mConnection = ILibWrapper_WebRTC_ConnectionFactory_CreateConnection(mConnectionFactory,
    [](ILibWrapper_WebRTC_Connection connection, int connected) {
    printf("GOT CONNECTION %i\n", connected);
    
    // ILibWrapper_WebRTC_DataChannel_Create(connection, "MyDataChannel", 13, [](ILibWrapper_WebRTC_DataChannel *dataChannel) {
    //   printf("DATA CHANNEL CREATED!");
    // });
    
  }, [](ILibWrapper_WebRTC_Connection connection, ILibWrapper_WebRTC_DataChannel *dataChannel) {
    printf("DATA CHANNEL OPEN\n");
    
    char* message = "MESSAGE FROM PASSIVE";
    ILibWrapper_WebRTC_DataChannel_SendString(dataChannel, message, strlen(message));
    
    
  }, [](ILibWrapper_WebRTC_Connection connection) {
    printf("CONNECTION OK!\n");
  });
  ILibWrapper_WebRTC_Connection_SetStunServers(mConnection, (char**)stunServerList, 9);
    
  
  char bodyBuffer[2048];
  fgets(bodyBuffer, sizeof(bodyBuffer), stdin);
  
  unsigned char* decodedOffer = nullptr;
  auto decodedOfferLen = ILibBase64Decode((unsigned char*)bodyBuffer, strlen(bodyBuffer), &decodedOffer);
  
  // We're freeing this, becuase we'll generate the offer in the candidate callback...
  // The best way, is to return this offer, and update the candidate incrementally, but that is for another sample
  ILibWrapper_WebRTC_Connection_SetUserData(mConnection, NULL, NULL, NULL);
  auto parsedOffer = ILibWrapper_WebRTC_Connection_SetOffer(mConnection, (char*)decodedOffer, decodedOfferLen, [](ILibWrapper_WebRTC_Connection connection, struct sockaddr_in6* candidate) {    
    char *sdp = ILibWrapper_WebRTC_Connection_AddServerReflexiveCandidateToLocalSDP(connection, candidate);
    
    unsigned char* encodedSdp = nullptr;
    auto encodedSdpLen = ILibBase64Encode((unsigned char*)sdp, strlen(sdp), &encodedSdp);
    printf("%s\n", encodedSdp);
    
    // ILibWrapper_WebRTC_Connection_GetUserData(connection, NULL, NULL, NULL);
    // auto offer = ILibWrapper_WebRTC_Connection_AddServerReflexiveCandidateToLocalSDP(connection, candidate);
    // 
    // char *encodedOffer = NULL;
    // auto encodedOfferLen = ILibBase64Encode((unsigned char*)offer, strlen(offer),(unsigned char**)&encodedOffer);
    // 
    // printf("GOT ENCODED OFFER: %s\n", encodedOffer);
    // 
    // char sdp[4096*4] = {0,};
    // fgets(sdp, sizeof(sdp), stdin);
    // auto sdpSize = strlen(sdp);
    // unsigned char* encodedSdp = NULL;
    // auto encodedSdpLength = ILibBase64Decode((unsigned char*)sdp, sdpSize, &encodedSdp);
    // 
    // printf("READ ANSWER: %s\n", encodedSdp);
    // 
    // // send answer
    // {
    //   free(ILibWrapper_WebRTC_Connection_SetOffer(connection, (char*)encodedSdp, encodedSdpLength, NULL));
    // }
    // 
    // free(encodedSdp);
    // free(offer);
  	// free(encodedOffer);
  });
  if(!parsedOffer) {
    printf("Something wrong with the parsedOffer\n");
  } else {
    free(parsedOffer);
  }
  
  ILibStartChain(chain);
}
