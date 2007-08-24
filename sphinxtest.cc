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
* $Id$
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

//------------------------------------------------------------------------------


int main()
{
    Sphinx::ConnectionConfig_t config;
    config.host = "localhost";
    config.port = 3313;
    config.keepAlive = true;
    config.connectTimeout = 2000;
    config.readTimeout = 2000;
    config.writeTimeout = 2000;


    Sphinx::Client_t connection(config);
    Sphinx::Response_t result;
    Sphinx::SearchConfig_t settings;

    printf("starting.....\n");

    try{
        connection.connect();
        printf("connection success.\n");
    }
    catch(Sphinx::Error_t e)
    {
        printf("connection error:\n%s\n", e.errMsg.c_str());
        return 1;
    }

    // search settings
    settings.msgOffset = 0;
    settings.msgLimit = 20;
    settings.commandVersion = Sphinx::VER_COMMAND_SEARCH_0_9_7_1;
    settings.matchMode = Sphinx::SPH_MATCH_EXTENDED;
    settings.maxMatches = 1000;

    // preparation of enumeration filter
    Sphinx::IntArray_t ids;
    ids.push_back(847);
    ids.push_back(831);
    ids.push_back(875);
    ids.push_back(840);

    // initialization of range and enum filter
    settings.addRangeFilter("attr1", 1, 600, true);
    settings.addEnumFilter("attr2", ids, true);

    // sort settings
    settings.sortMode = Sphinx::SPH_SORT_EXTENDED;
    settings.sortBy = "vat_price asc";

    // group settings
    settings.groupFunction = Sphinx::SPH_GROUPBY_ATTR;
    settings.groupBy = "attr1";
    settings.groupSort = "attr2 asc";

    try{
        connection.query("@field1 hello | @field2 world", settings, result);
        printf("query success.\n");
    }
    catch(Sphinx::Error_t e)
    {
        printf("query error:\n%s\n", e.errMsg.c_str());
        return 2;
    }

    printf("command version: 0x%X\n", result.commandVersion);
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

    // print results with attributes
    printf("\nResponse:\n");

    int i = 0; 
    for (std::vector<Sphinx::ResponseEntry_t>::const_iterator it = result.entry.begin();
         it != result.entry.end(); it++) {

        i++;
        printf("%d) %d ", i, it->documentId);
        for (std::map<std::string, uint32_t>::const_iterator j =
                            it->attribute.begin();
                j != it->attribute.end();
                j++) {
            printf("--%s:%d-- ", j->first.c_str(), j->second);
        }
        printf("\n");
    }
    return 0;
}//main

