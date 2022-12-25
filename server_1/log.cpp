#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>

#define DEBUG

int main()
{

  int i = 0;
  openlog("test", LOG_PID, LOG_USER);

#ifdef DEBUG
  syslog(LOG_DEBUG, "try to sending 10 messages");
#endif

  for (i = 0; i < 10; i++)
  {
    syslog(LOG_INFO, "info message [i = %d] ", i);
  };

#ifdef DEBUG
  syslog(LOG_DEBUG, "try log to stderr");
#endif
  closelog();

  openlog("test_stderr", LOG_PERROR | LOG_PID, LOG_USER);
  syslog(LOG_INFO, "this is attempt to use stderr for syslog");
  closelog();

  return 0;
};