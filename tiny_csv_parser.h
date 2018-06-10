#pragma once
#include <exception>
#include <string>

#include "absl/strings/string_view.cc"

using absl::string_view;

namespace tinycsv{
using std::string;

// Search for c until an unescaped occurrence is found or end of string is reached.
size_t findFirstUnescaped(string_view str, char to_find, 
                            char escape='\\')
{
    size_t pos = 0;
    while(pos < str.size()){
        pos = str.find(to_find, pos);
        if(pos == 0) { 
            return 0; 
        }
        else if(str[pos - 1] == escape){
            ++pos;
            continue;
        }
        else return pos;
    }
    return string::npos;
}

//
// returns a string_view to the next valid csv token from position
//    position should stard as a quote
//    NOTE: MODIFIES FROM_POS as side effect
// @param str the string to parse
// @param from_pos the position to search from
//                 NOTE: THIS IS MODIFIED BY THE FN
//                 the string at from_pos should be the quoting character
//
string_view getQuotedString(string_view str, size_t* from_pos=0,
                            char quote='\"')
{
    string_view to_search(&str[*from_pos+1], str.size() - *from_pos - 1);
    size_t token_end = findFirstUnescaped(to_search, quote) + *from_pos;
    if (token_end == string::npos){
        throw std::runtime_error("No end on quoted string.");
    }
    // remove outer quotes from result
    string_view result(&str[*from_pos+1], token_end - *from_pos);
    // update location pointer
    // since search is in [pos, size) return next char 
    // after comma which is after quote so + 2
    *from_pos = token_end + 3;
    return result;
}


//
// returns a string_view to the next valid csv token from position
//    NOTE: MODIFIES FROM_POS as side effect
// @param line the csv line to parse
// @param from_pos the position to search from
//                 NOTE: THIS IS MODIFIED BY THE FN
// @param separatorThenEscaper a string of two chars:
//            first the separator (usually a comma) 
//            then the escape char (usually "\"")
//            ",\"" by default
//
string_view nextToken(string_view line, size_t* from_pos=0, 
                      string_view separatorThenEscaper=",\"")
{
    auto first_char = line[*from_pos];
    if(first_char == separatorThenEscaper[1]){
        return getQuotedString(line, from_pos);
    }
    if (first_char == separatorThenEscaper[0]){
        *from_pos += 1;
        return string_view();
    }
    // TODO: not quote in this find_first_of
    size_t next_stop = line.find_first_of(separatorThenEscaper, *from_pos + 1);
    string_view this_token;
    if (next_stop == string::npos){
        this_token = string_view(&line[*from_pos], line.size()- *from_pos);
    }
    else{
        this_token = string_view(&line[*from_pos], next_stop - *from_pos);
    }
    // update location pointer
    // since search is in [pos, size) return next char after a comma
    *from_pos = next_stop + 1;
    return this_token;
}

} // tinycsv