//
// Created by alik on 6/14/18.
//

#ifdef max
    #undef max
#endif

#ifdef min
    #undef min
#endif

#ifdef MAX
    #undef MAX
#endif
#define MAX max

#ifdef MIN
   #undef MIN
#endif
#define MIN min

#include <algorithm>
using std::max;
using std::min;