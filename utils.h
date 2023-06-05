#ifndef UTILS_H
#define UTILS_H

#include <string>

using namespace std;

string getNameFromLocation(string str)
{
    string ret;
    int index;

    for(int i = int(str.size()) - 1; i >= 0; i--)
    {
        if(str[i] == '/')
        {
            index = i + 1;
            i = -1;
        }
    }

    while(index < int(str.size()))
    {
        ret.push_back(str[index]);
        index++;
    }

    return ret;
}

#endif // UTILS_H
