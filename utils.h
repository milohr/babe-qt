#ifndef UTILS_H
#define UTILS_H

#include "string"

using namespace std;

string getNameFromLocation(string str)
{
    string ret;
    int index;

    for(int i = str.size() - 1; i >= 0; i--)
    {
        if(str[i] == '/')
        {
            index = i + 1;
            i = -1;
        }
    }

    for(; index < str.size(); index++)
    {
        ret.push_back(str[index]);
    }

    return ret;
}

#endif // UTILS_H
