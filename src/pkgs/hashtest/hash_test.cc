#if 0

#include "util/hash.h"
#include "util/strings.h"
#include <iostream.h>

typedef hash_array<fstring, int, string_hasher<fstring> > htype;

void print_stats(htype& h)
{

  {
    for(htype::iterator itr = h.begin(), end = h.end();
        itr != end;
        ++itr)
      {
        cout << itr->key << ' ' << itr->value << endl;
      }
  }

  cout << "size == " << h.size() << endl;
  cout << "empty == " << h.empty() << endl;
  cout << "bucket_count == " << h.bucket_count() << endl;
  cout << "load_factor == " << h.load_factor() << endl;
  cout << "longest_chain == " << h.longest_chain() << endl << endl;
}

int foo() {
  htype myhash;

  myhash.insert("Hello", 2);
  myhash.insert("World", 3);
  myhash.insert("Hello1", 4);
  myhash.insert("World2", 5);
  myhash.insert("Hello3", 6);
  myhash.insert("World4", 7);
  myhash.insert("Hello5", 8);
  myhash.insert("World6", 9);

  print_stats(myhash);

  myhash.clear();
  print_stats(myhash);
}

#endif

