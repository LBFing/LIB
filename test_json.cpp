
#include <stdio.h>          
#include <stdlib.h>                                                                                  
#include "cJSON.h"

char * makeJson()
{
    cJSON * pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "hello", "hello world");
    cJSON_AddNumberToObject(pJsonRoot, "number", 10010);
    cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
    cJSON * pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
        // create object faild, exit
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    cJSON_AddStringToObject(pSubJson, "subjsonobj", "a sub json string");
    cJSON_AddItemToObject(pJsonRoot, "subobj", pSubJson);

    char * p = cJSON_Print(pJsonRoot);
  // else use : 
    // char * p = cJSON_PrintUnformatted(pJsonRoot);
    if(NULL == p)
    {
        //convert json list to string faild, exit
        //because sub json pSubJson han been add to pJsonRoot, so just delete pJsonRoot, if you also delete pSubJson, it will coredump, and error is : double free
        cJSON_Delete(pJsonRoot);
        return NULL;
    }
    //free(p);
    
    cJSON_Delete(pJsonRoot);

    return p;
}

void parseJson(char * pMsg)
{
    if(NULL == pMsg)
    {
        return;
    }
    cJSON * pJson = cJSON_Parse(pMsg);
    if(NULL == pJson)                                                                                         
    {
        // parse faild, return
      return ;
    }

    // get string from json
    cJSON * pSub = cJSON_GetObjectItem(pJson, "hello");
    if(NULL == pSub)
    {
        //get object named "hello" faild
    }
    printf("obj_1 : %s\n", pSub->valuestring);

    // get number from json
    pSub = cJSON_GetObjectItem(pJson, "number");
    if(NULL == pSub)
    {
        //get number from json faild
    }
    printf("obj_2 : %d\n", pSub->valueint);

    // get bool from json
    pSub = cJSON_GetObjectItem(pJson, "bool");
    if(NULL == pSub)
    {
        // get bool from json faild
    }                                                                                                         
    printf("obj_3 : %d\n", pSub->valueint);

 // get sub object
    pSub = cJSON_GetObjectItem(pJson, "subobj");
    if(NULL == pSub)
    {
        // get sub object faild
    }
    cJSON * pSubSub = cJSON_GetObjectItem(pSub, "subjsonobj");
    if(NULL == pSubSub)
    {
        // get object from subject object faild
    }
    printf("sub_obj_1 : %s\n", pSubSub->valuestring);
    cJSON_Delete(pJson);
}

int TestJson()
{
    char * p = makeJson();
    if(NULL == p)
    {
        return 0;
    }
    printf("%s\n", p);
    parseJson(p);
	free(p);
    return 0;
}