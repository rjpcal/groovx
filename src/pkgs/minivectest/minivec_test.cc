#if 0

#include "util/minivec.h"
typedef minivec<int> Vec;

#include <iostream.h>

void print(Vec& vec)
{
  cout << "capacity == " << vec.capacity() << '\n';
  cout << "size == " << vec.size() << '\n';

  for (Vec::iterator itr = vec.begin(), end = vec.end();
       itr != end;
       ++itr)
    {
      cout << *itr << ' ' ;
    }
  cout << endl << endl;
}

int foo()
{
  Vec vec;

  print(vec);

  vec.resize(5, 0);
  vec[3] = 3;
  vec[4] = 49;

  print(vec);

  vec.resize(10, 0);
  vec[9] = 345;

  print(vec);

  vec.push_back(-7);
  vec.push_back(-10);

  print(vec);

  vec.pop_back();
  vec.pop_back();
  vec.pop_back();

  print(vec);

  return 0;
}

#endif
