#include <iostream>
#include <string>


#ifdef BROCCOLI
#include <broccoli.h>
#endif

using namespace std;

int main()
{
  cout << "Hello broception!" << endl;

  bro_init (NULL);

  string host_str = "localhost:47758";
  BroConn *bc;

  if (! (bc = bro_conn_new_str(host_str.c_str(), BRO_CFLAG_DONTCACHE)))
  {
    cerr << "Requesting new bro connection failed" << endl;
    exit (EXIT_FAILURE);
  }

  if (! bro_conn_connect(bc))
  {
    cerr << "Error connecting to remote bro instance: \"" << host_str << "\"" << endl;
    exit(EXIT_FAILURE);
  }
  return 0;
}

