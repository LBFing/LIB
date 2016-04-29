
#include <stdio.h>                                                                                       
#include "cjson.h"
#include <stdlib.h>

char * makeJson()
{
    cJSON * pJsonRoot = NULL;

    pJsonRoot = cJSON_CreateObject();
    if(NULL == pJsonRoot)
    {
        //error happend here
        fputs ("cJSON_CreateObject1",stderr);
        return NULL;
    }
    cJSON_AddStringToObject(pJsonRoot, "hello", "hello world");
    cJSON_AddNumberToObject(pJsonRoot, "number", 10010);
    cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
    cJSON * pSubJson = NULL;
    pSubJson = cJSON_CreateObject();
    if(NULL == pSubJson)
    {
        fputs ("cJSON_CreateObject2",stderr);
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

class JSonParse
{
public:
    JSonParse();
    ~JSonParse();
	bool LoadFile(char *szFileName);
    bool Parse();
    cJSON *GetObjectItem(char* szkey);
    cJSON *GetObjectItemEx(cJSON *pJson,char* szkey);
private:
    char* m_filebuf;
    cJSON *m_pJson;
};

JSonParse::JSonParse()
{

}
JSonParse::~JSonParse()
{
    if (m_filebuf)
    {
        delete m_filebuf;
        m_filebuf = NULL;
    }
    if (m_pJson)
    {
        cJSON_Delete(m_pJson);
    }
}

bool JSonParse::LoadFile(char *szFileName)
{    
    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */ 
    FILE *pFile = fopen (szFileName, "rb" );
    if (pFile==NULL)
    {
        fputs ("File error",stderr);
        return false;
    }

    /* 获取文件大小 */
    fseek (pFile , 0 , SEEK_END);
    size_t lSize = ftell (pFile);
    rewind (pFile);

    /* 分配内存存储整个文件 */ 
    m_filebuf = new char [lSize+1];
    if (m_filebuf == NULL)
    {
        fputs ("Memory error",stderr); 
        return false;
    }

    /* 将文件拷贝到buffer中 */
    size_t result = fread (m_filebuf,lSize,1,pFile);
    if (result == 0)
    {
        printf("result :%d size:%d",result,lSize);
        fputs ("Reading error",stderr);
        return false;
    }
    /* 现在整个文件已经在buffer中，可由标准输出打印内容 */
    //printf("%s", m_filebuf); 
    /* 结束演示，关闭文件并释放内存 */
    fclose (pFile);
    return true;
}

bool JSonParse::Parse()
{
    if(NULL == m_filebuf)
    {
        printf("m_filebuf is null\n");
        return false;
    }
    m_pJson = cJSON_Parse(m_filebuf);
    if(NULL == m_pJson)                                                                                         
    {
        printf("cJSON_Parse is failed\n");
        return false;
    }
    return true;
}

cJSON *JSonParse::GetObjectItem(char* szkey)
{
    return cJSON_GetObjectItem(m_pJson, szkey);
}

int main()
{
    JSonParse dc;
    char szFileName[] = "test.json";
    dc.LoadFile(szFileName);
    dc.Parse();
    cJSON* pSub = dc.GetObjectItem("hello");
    printf("obj_1 : %s\n", pSub->valuestring);
    //parseJson(makeJson());
}