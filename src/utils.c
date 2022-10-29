/*
 * Copyright 2022 Lucas V. Araujo (https://github.com/lvmalware)
 *
 * This file is part of cspider.
 * RefFinder is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * RefFinder is distributed in the hope that it will be useful,but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with RefFinder. If not, see <https://www.gnu.org/licenses/>.
 */

#include "utils.h"

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t length = size * nmemb; // size is always equal to 1
    struct string_buffer_s *sb = (struct string_buffer_s *) userdata;
    string_buffer_concat(sb, ptr, length);
    return length;
}

size_t header_callback(char *ptr, size_t size, size_t nitems, void *userp) {
    if (strncasecmp(ptr, "content-type", nitems) == 0) {
        char *type = strchr(ptr, ' ');
        if (type && strncasecmp(type + 1, "text/html", 9) != 0) return 0;
    }
    return nitems * size;
}

int http_get(char * const url, struct string_buffer_s * const result) {
    CURL *ceh = curl_easy_init();
    if (!ceh) return 0;
    curl_easy_setopt(ceh, CURLOPT_URL, url);
    curl_easy_setopt(ceh, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(ceh, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(ceh, CURLOPT_WRITEDATA, result);
    CURLcode res = curl_easy_perform(ceh);
    curl_easy_cleanup(ceh);
    if (res != CURLE_OK) return 0;
    return 1;
}

size_t crawl_over(char * const url, struct list_head_s * const list) {
    struct string_buffer_s body;
    string_buffer_init(&body);
    if (http_get(url, &body) == 0) goto FINISH;
    htmlDocPtr html = htmlReadMemory(body.buff, body.size, url, NULL,
            HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR |\
            HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
    if(!html) goto FINISH;
    xmlChar *xpath = (xmlChar*) "//a/@href";
    xmlXPathContextPtr context = xmlXPathNewContext(html);
    xmlXPathObjectPtr result = xmlXPathEvalExpression(xpath, context);
    xmlXPathFreeContext(context);
    if(!result) goto FINISH;
    xmlNodeSetPtr nodeset = result->nodesetval;
    if(xmlXPathNodeSetIsEmpty(nodeset)) {
        xmlXPathFreeObject(result);
        goto FINISH;
    }
    size_t i;
    for(i = 0; i < nodeset->nodeNr; i++) {
        const xmlNode *node = nodeset->nodeTab[i]->xmlChildrenNode;
        xmlChar *href = xmlNodeListGetString(html, node, 1);
      	xmlChar *orig = href;
      	href = xmlBuildURI(href, (xmlChar *) url);
      	xmlFree(orig);
        char *link = (char *) href;
        if (link && strncmp(link, "http", 4) == 0) {
            list_head_push(list, strdup(link), strlen(link));
        }
        xmlFree(link);
    }
    xmlXPathFreeObject(result);

FINISH:
    string_buffer_finish(&body);
    return list->length;
}

