/*
    Client
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

// Constants
#define SERVER1_PORT 5001;
#define SERVER2_PORT 5002;

void printMenu()
{
  printf("What do you want to see?\n");
  printf("1. Size of the server process window\n");
  printf("2. Number of monitors\n");
  printf("3. Number of server process threads\n");
  printf("4. number of processes on the server\n");
  printf("5. Exit\n");
  printf(">");
}

int getVariant(int count)
{
  int variant;
  char s[100];    // строка для считывания введённых данных
  scanf("%s", s); // считываем строку

  // пока ввод некорректен, сообщаем об этом и просим повторить его
  while (sscanf(s, "%d", &variant) != 1 || variant < 1 || variant > count)
  {
    printf("Incorrect input. Try again: "); // выводим сообщение об ошибке
    scanf("%s", s);                         // считываем строку повторно
  }

  return variant;
}

void connect(int port, int variant)
{
  int socket_desc;
  struct sockaddr_in client_addr;
  char buffer[256];

  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  client_addr.sin_family = AF_INET;
  client_addr.sin_addr.s_addr = INADDR_ANY;
  // client_addr.sin_addr.s_addr = htonl(0xac150002);
  client_addr.sin_port = htons(port);

  if (connect(socket_desc, (struct sockaddr *)&client_addr, sizeof(client_addr)) == 0)
    printf("\nConnected to Server on port %d\n", port);
  else
    printf("\nConnection to server failed !\n");

  bzero(buffer, 256);
  sprintf(buffer, "%d", variant);
  write(socket_desc, buffer, strlen(buffer));
  bzero(buffer, 256);
  read(socket_desc, buffer, 255);
  printf("Response from Server: %s\n\n", buffer);
  close(socket_desc);
}

int main()
{
  int variant;
  do
  {
    printMenu();
    variant = getVariant(5);
    if (variant == 1 || variant == 2)
    {
      connect(5001, variant);
    }
    else if (variant == 3 || variant == 4)
    {
      connect(5002, variant);
    }
  } while (variant != 5);
  return 0;
}