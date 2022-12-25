#include <iostream> // input output
#include <string>   // strlen string
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <pthread.h> // multi threading
#include <errno.h>
#include <signal.h>
#include <cerrno>

#include <sys/ioctl.h>
#include <fstream>
#include <syslog.h>

using namespace std;

#define ADDRESS "0.0.0.0"         // server address
#define PORT 5002                 // port number
#define CONCURRENT_CONNECTION 3   // maximum concurrent connections
#define QUEUE_CONNECTION 20       // maximum connection requests queued
#define BUFFER_SIZE 1024          // buffer size 1KB
#define THREAD_STACK_SIZE 65536   // Thread stack size 64KB
int connection = 0;               // current connections counter
void *connection_handler(void *); // connection handler function

int main(int argc, char *argv[])
{
  openlog("server2", LOG_PID, LOG_USER);
  pthread_t thread_id; // thread identifier
  pthread_attr_t attr; // thread attribute

  // ???
  if (pthread_attr_init(&attr) != 0)
  {
    std::cout << "[SERVER_2][ERROR][THREAD][INIT] " << strerror(errno) << "\n";
    return -1;
  }

  // stack size 1MB
  if (pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) != 0)
  {
    std::cout << "[SERVER_2][ERROR][THREAD][STACK] " << strerror(errno) << "\n";
    return -1;
  }

  if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
  {
    std::cout << "[SERVER_2][ERROR][THREAD][DETACH] " << strerror(errno) << "\n";
    return -1;
  }
  // ???

  int master_socket, conn_id;
  struct sockaddr_in server, client;

  memset(&server, 0, sizeof(server));
  memset(&client, 0, sizeof(client));

  signal(SIGPIPE, SIG_IGN);

  // creating master socket
  if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    std::cout << "[SERVER_2][ERROR] Can't create socket\n";
    return -1;
  }
  else
  {
    syslog(LOG_INFO, "[SERVER_2][ACCESS] Socket created log");
    std::cout << "[SERVER_2][ACCESS] Socket created\n";
  }

  // Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(ADDRESS);
  server.sin_port = htons(PORT);

  socklen_t addrlen = sizeof(struct sockaddr_in);

  // binding address and port
  if (bind(master_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
  {
    std::cout << "[SERVER_2][ERROR] " << strerror(errno) << "\n";
    return -1;
  }

  // Listen on the socket, with 20 max connection requests queued
  if (listen(master_socket, QUEUE_CONNECTION) == -1)
  {
    std::cout << "[SERVER_2][ERROR] " << strerror(errno) << "\n";
    return -1;
  }
  else
  {
    syslog(LOG_INFO, "[SERVER_2][INFO] Waiting for incoming connections...");
    std::cout << "[SERVER_2][INFO] Waiting for incoming connections...\n";
  }

  while (true)
  {
    // accept new connections
    conn_id = accept(master_socket, (struct sockaddr *)&client, (socklen_t *)&addrlen);

    // if connection acception failed
    if (conn_id == -1)
      std::cout << "[SERVER_2][WARNING] Can't accept connection\n";
    else
    {
      // if connection limit reached
      if (connection >= CONCURRENT_CONNECTION)
      {
        std::cout << "[SERVER_2][WARNING] Connection Limit\n";
        close(conn_id); // close connection
      }
      else
      {
        syslog(LOG_INFO, "[SERVER_2][ACCESS] New connection accepted from %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
        std::cout << "[SERVER_2][ACCESS] New connection accepted from " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\n";
        if (pthread_create(&thread_id, &attr, connection_handler, new int(conn_id)) == -1) // create new thread for new connection
        {
          std::cout << "[SERVER_2][WARNING] Can't create new thread\n";
          close(conn_id);
        }
        else
          connection++; // increase connection count
      }
    }
  }
  return 0;
}

// This will handle connection for each client
void *connection_handler(void *sock_fd)
{
  // Print Thread ID
  syslog(LOG_INFO, "[SERVER_2][INFO] THREAD ID = %ld", pthread_self());
  printf("[SERVER_2][INFO] THREAD ID = %ld\n", pthread_self());

  int read_byte = 0;                // byte size
  int conn_id = *(int *)sock_fd;    // Get the socket descriptor
  char buffer[BUFFER_SIZE] = {0};   // request data
  char response[BUFFER_SIZE] = {0}; // response data

  struct winsize w; // Size Struct
  char windowHeight[100], windowWidth[100];

  char *fileBuffer = new char[1024];
  int lineCounter = 0;

  // read response continue
  while ((read_byte = recv(conn_id, buffer, BUFFER_SIZE, 0)) > 0)
  {
    syslog(LOG_INFO, "[SERVER_2][RECEIVED] %s", buffer);
    std::cout << "[SERVER_2][RECEIVED] " << buffer << "\n";

    // ------------------- Обработка данных
    if (strcmp(buffer, "3") == 0)
    {
      sprintf(response, "%d", connection);
    }
    else if (strcmp(buffer, "4") == 0)
    {
      system("ps -A > file.txt");
      ifstream file("file.txt");
      while (!file.eof())
      {
        file.getline(fileBuffer, 1024, '\n');
        lineCounter++;
      }
      file.close();
      sprintf(response, "%d", lineCounter - 2);
    }
    else
      sprintf(response, "%s", "Enter a number in the range from 3 to 4");

    memset(buffer, 0, BUFFER_SIZE); // clear buffer data

    // Send response
    if (send(conn_id, response, strlen(response), 0) > 0)
    {
      syslog(LOG_INFO, "[SERVER_2][SEND] %s", response);
      std::cout << "[SERVER_2][SEND] " << response << "\n";
    }

    else
      std::cout << "[SERVER_2][WARNING] " << strerror(errno) << "\n";
  }

  sleep(1);       // Sleep
  close(conn_id); // terminate connection
  syslog(LOG_INFO, "[SERVER_2][INFO] CONNECTION CLOSED");
  std::cout << "[SERVER_2][INFO] CONNECTION CLOSED\n"; // Close Connection
  connection--;                                        // decrease connection counts
  delete (int *)sock_fd;                               //
  std::cout << "\n";                                   // \n
  pthread_exit(NULL);                                  // Terminate Thread
}