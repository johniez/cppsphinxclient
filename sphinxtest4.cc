/*
*
* C++ sphinx search client library
* Copyright (C) 2007  Seznam.cz, a.s.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Seznam.cz, a.s.
* Radlicka 2, Praha 5, 15000, Czech Republic
* http://www.seznam.cz, mailto:sphinxclient@firma.seznam.cz
*
* $Id: sphinxtest.cc 15 2009-06-17 06:40:44Z honkir $
*
* DESCRIPTION
* A sphinxclient sample and testing program.
*
* AUTHORS
* Jan Kirschner <jan.kirschner@firma.seznam.cz>
*
* HISTORY
* 2006-12-14  (kirschner)
*             Created.
*
* Quick compile:
* g++  -g sphinxtest.cc -Iinclude/  -Lsrc/.libs/ -lsphinxclient -o sphinxtest
*/

#include <sphinxclient/sphinxclient.h>
#include <stdio.h>

//------------------------------------------------------------------------------


int main()
{
    Sphinx::ConnectionConfig_t config (
        "localhost", // hostname
        3312,        // port
        true,        // keepalive
        2000,        // connect timeout
        20000,       // read timeout
        2000         // write timeout
    );

    Sphinx::Client_t connection(config);
    Sphinx::Response_t result;
    Sphinx::SearchConfig_t settings(Sphinx::VER_COMMAND_SEARCH_2_0_5);

    printf("starting.....\n");

    // search settings
    settings.setPaging(0, 20);
    settings.setMatchMode(Sphinx::SPH_MATCH_EXTENDED2);
    settings.setMaxMatches(1000);
    settings.setSearchedIndexes("*");

    try{
        //single query
        connection.query("100 najlepsich slovenskych vin sk 2005", settings, result);
        printf("query success.\n");
    }
    catch(Sphinx::Error_t e)
    {
        printf("query error:\n%s\n", e.errMsg.c_str());
        return 2;
    }

    printf("command version: 0x%X\n", result.commandVersion);

    printf("------------------------ result ---------------------\n");
    printf("field count:       %d\n", result.field.size());
    printf("attribute count:   %d\n", result.attribute.size());
    printf("match count:       %d\n", result.entry.size());
    printf("word count:        %d\n", result.word.size());

    printf("\nFields:\n");
    for (int i=0;i<result.field.size();i++)
    {
        printf("    %s\n", result.field[i].c_str());
    }//for

    printf("\nAttributes:\n");
    for (Sphinx::AttributeTypes_t::iterator it=result.attribute.begin() ;
         it!=result.attribute.end();it++)
    {
        if (it->second & Sphinx::SPH_ATTR_MULTI) {
            printf("    name: %s, multi type: %d\n", it->first.c_str(), (it->second ^ Sphinx::SPH_ATTR_MULTI));
        }//if
        else
            printf("    name: %s, type: %d\n", it->first.c_str(), it->second);
    }//for

    printf("\nWords:\n");
    for (std::map<std::string, Sphinx::WordStatistics_t>::iterator it
         = result.word.begin() ; it!=result.word.end() ; it++)
    {
        printf("    Word %s: %d docs / %d hits\n", it->first.c_str(), it->second.docsHit, it->second.totalHits);
    }//for

    printf("entries: %d\n", result.entriesGot);
    printf("documents: %d\n", result.entriesFound);
    printf("duration: %d\n", result.timeConsumed);
    printf("64bit ID: %d\n", result.use64bitId);

    // print results with attributes
    printf("\nResponse:\n");

    int i = 0; 
    for (std::vector<Sphinx::ResponseEntry_t>::const_iterator it = result.entry.begin();
         it != result.entry.end(); it++) {

        i++;
        printf("%d) id: %ld ", i, it->documentId);
        for (std::map<std::string, Sphinx::Value_t>::const_iterator j =
                            it->attribute.begin();
                j != it->attribute.end() ; j++)
        {
            switch(j->second.getValueType()) {
            case Sphinx::VALUETYPE_FLOAT:
                printf("%s:%.2f | ", j->first.c_str(), (float)(j->second));
                break;
            case Sphinx::VALUETYPE_VECTOR: {
                char comma[2] = {'\0', '\0'};
                std::vector<Sphinx::Value_t> vec = j->second;
                printf("%s:(", j->first.c_str());
                for (std::vector<Sphinx::Value_t>::iterator vi=vec.begin() ; vi!=vec.end() ; vi++) {
                    if(vi->getValueType() == Sphinx::VALUETYPE_FLOAT)
                        printf("%s%ff", comma, (float)(*vi));
                    else
                        printf("%s%dd", comma, (uint32_t)(*vi));
                    comma[0] = ',';
                }//for
                printf(") | ");
                break; }
            case Sphinx::VALUETYPE_UINT64:
                printf("%s:%lu | ", j->first.c_str(), (uint64_t)(j->second));
                break;
            case Sphinx::VALUETYPE_STRING:
                printf("%s:%s | ", j->first.c_str(), ((std::string)(j->second)).c_str());
                break;
            case Sphinx::VALUETYPE_UINT32:
            default:
                printf("%s:%d | ", j->first.c_str(), (uint32_t)(j->second));
                break;
            }//switch
        }
        printf("\n");
    }//for response entries

    printf("----------------------------- end ------------------------\n");

    return 0;
}//main

