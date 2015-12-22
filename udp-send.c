/*
	send udp messages
	This sends a sequence of messages (the # of messages is defined in MSGS)
        usage:  udp-send.exe on windows
        usage:  ./udp-send on linux
        Paul Mohr
*/
//==================================================== file = udpClient.c =====
//=  A message "client" program to demonstrate sockets programming            =
//=============================================================================
//=  Notes:                                                                   =
//=    1) This program conditionally compiles for Winsock and BSD sockets.    =
//=       Set the initial #define to WIN or BSD as appropriate.               =
//=    2) This program needs udpServer to be running on another host.         =
//=       Program udpServer must be started first.                            =
//=    3) This program assumes that the IP address of the host running        =
//=       udpServer is defined in "#define IP_ADDR"                           =
//=    4) The steps #'s correspond to lecture topics.                         =
//=---------------------------------------------------------------------------=
//=  Example execution: (udpServer and udpClient running on host 127.0.0.1)   =
//=    Received from server: This is a reply message from SERVER to CLIENT    =
//=---------------------------------------------------------------------------=
//=  Build: bcc32 ucpClient.c or cl udpClient.c wsock32.lib for Winsock       =
//=         gcc udpClient.c -lsocket -lnsl for BSD                            =
//=---------------------------------------------------------------------------=
//=  Execute: udpClient                                                       =
//=---------------------------------------------------------------------------=
//=  Original Author: Ken Christensen                                                  =
//=          University of South Florida                                      =
//=          WWW: http://www.csee.usf.edu/~christen                           =
//=          Email: christen@csee.usf.edu                                     =
//=---------------------------------------------------------------------------=
//=  History:  KJC (08/02/08) - Genesis (from client.c)                       =
//=            KJC (09/09/09) - Minor clean-up                                =
//=            KJC (09/22/13) - Minor clean-up to fix warnings                =
//=============================================================================
#define  LINUX                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#ifdef LINUX
#include <netdb.h>
#include <sys/socket.h>
#include "port.h"
#include "ifaddrs.h"
#include "netinet/in.h"
#include "arpa/inet.h"

#endif
#ifdef WIN
// #include <winsock.h>
#include <winsock2.h>
  #include <windows.h>      // Needed for all Winsock stuff
#endif
#ifdef BSD
  #include <sys/types.h>    // Needed for sockets stuff
  #include <netinet/in.h>   // Needed for sockets stuff
  #include <sys/socket.h>   // Needed for sockets stuff
  #include <arpa/inet.h>    // Needed for sockets stuff
  #include <fcntl.h>        // Needed for sockets stuff
  #include <netdb.h>        // Needed for sockets stuff
#endif

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM          49152 // was 1050 Port number used
#define  IP_ADDR      "192.168.1.12" // IP address of server1 (*** HARDWIRED ***)
#define  IP_ADDR2      "192.168.1.2" // IP address of server1 (*** HARDWIRED ***)

char addrp[INET6_ADDRSTRLEN];
char* lookip(){
    struct ifaddrs *iflist, *iface;
    char *fault="There is no IP as we got a fault";
    const void *addr;
    if (getifaddrs(&iflist) < 0) {
        perror("getifaddrs");
        return fault;
    }
    for (iface = iflist; iface; iface = iface->ifa_next) {
        int af = iface->ifa_addr->sa_family;
        switch (af) {
            case AF_INET:
                addr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
                break;
            case AF_INET6:
                addr = &((struct sockaddr_in6 *)iface->ifa_addr)->sin6_addr;
                break;
            default:
                addr = NULL;
        }

        if (addr) {
            if (inet_ntop(af, addr, addrp, sizeof addrp) == NULL) {
                perror("inet_ntop");
                continue;
            }

            if ( strstr(addrp,"192") == addrp){
//	      		printf("Interface 192 %s has address %s\n", iface->ifa_name, addrp);
            	freeifaddrs(iflist);return addrp;
            }
            if ( strstr(iface->ifa_name,"eth") == iface->ifa_name){
//			      printf("Interface eth %s has address %s\n", iface->ifa_name, addrp);
            	freeifaddrs(iflist);return addrp;
            }
        }
    }

    freeifaddrs(iflist);
    return addrp;
}

//===== Main program ==========================================================
int main(){
#ifdef WIN
  WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
  WSADATA wsaData;                              // Stuff for WSA functions
#endif
  int                  client_s;        // Client socket descriptor
  struct sockaddr_in   server_addr;     // Server Internet address
  int                  addr_len;        // Internet address length
  char                 out_buf[4096];   // Output buffer for data
  char                 in_buf[4096];    // Input buffer for data
  int                  retcode;         // Return code
#ifdef WIN
  // This stuff initializes winsock
  WSAStartup(wVersionRequested, &wsaData);
#endif
  // >>> Step #1 <<<  // Create a socket  //   - AF_INET is Address Family Internet and SOCK_DGRAM is datagram
  client_s = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_s < 0)  {
    printf("*** ERROR - socket() failed \n");    exit(-1);
  }
  // >>> Step #2 <<<  // Fill-in server1 socket's address information
  server_addr.sin_family = AF_INET;                 // Address family to use
  server_addr.sin_port = htons(PORT_NUM);           // Port num to use
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDR2); // IP address to use


  if (bind(client_s, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
          perror("bind failed");          exit(1);
      }
//  server_addr.sin_addr.s_addr = inet_addr(IP_ADDR2); // IP address to use
  // Assign a message to buffer out_buf
  sprintf(out_buf, "Test message from %s %d to %s %d or THIS %s?",IP_ADDR2,PORT_NUM,IP_ADDR,PORT_NUM,lookip());
  // >>> Step #3 <<<
  // Now send the message to server.  The "+ 1" is for the end-of-string
  // delimiter
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
  retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
    (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (retcode < 0)
  {
    printf("*** ERROR - sendto() failed \n");
    exit(-1);
  }

  // >>> Step #4 <<<
  // Wait to receive a message
  server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
  addr_len = sizeof(server_addr);
  retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0,
    (struct sockaddr *)&server_addr, &addr_len);
  if (retcode < 0)
  {
    printf("*** ERROR - recvfrom() failed \n");
    exit(-1);
  }

  // Output the received message
  printf("Received from server: %s \n", in_buf);

  // >>> Step #5 <<<
  // Close all open sockets
#ifdef WIN
  retcode = closesocket(client_s);
  if (retcode < 0)
  {
    printf("*** ERROR - closesocket() failed \n");
    exit(-1);
  }
#endif
#ifdef BSD
  retcode = close(client_s);
  if (retcode < 0)
  {
    printf("*** ERROR - close() failed \n");
    exit(-1);
  }
#endif

#ifdef WIN
  // This stuff cleans-up winsock
  WSACleanup();
#endif

  // Return zero and terminate
  return(0);
}
