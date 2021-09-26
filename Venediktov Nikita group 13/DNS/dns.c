/*************************************************************************
   LAB 1

    Edit this file ONLY!

*************************************************************************/



#include "dns.h"
#include <stdio.h>

typedef struct _Node   // element of hash-table
{
    char* domen;
    IPADDRESS ip;
    struct _Node* next_node;
}Node;
const int SIZE = 13000;   //size of array of pointers on nodes


DNSHandle InitDNS()
{
    DNSHandle hDNS = (DNSHandle)(Node**)calloc(SIZE, sizeof(Node*));
    if ((Node**)hDNS != NULL)  
        return hDNS;
    return (DNSHandle)0;  // case of OS does not provide with memory
}
unsigned int getHash(const char* domen)
{

    if (domen == NULL)
        return -1;
    unsigned int len = strlen(domen);
    if (len == 0)
        return -1;
    long sum = 0, mul = 1;
    for (int i = 0; i < len; ++i) {
        mul = (i % 4 == 0) ? 1 : mul * 256;
        sum += (domen[i]) * mul;
    }
    return (unsigned int)(abs(sum) % SIZE);
}
Node* NodeInicialisation(const char* domen, IPADDRESS ip, int domen_length) // "constructor" of node struct
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->domen = (char*)malloc((domen_length + 1) * sizeof(char));
    strcpy(node->domen, domen);
    node->ip = ip;
    return node;
}
void InsertToServer(DNSHandle hDNS, const char* domen, IPADDRESS ip)
{
    if ((Node**)hDNS == NULL || domen == NULL)
        return;
    unsigned int index = getHash(domen);
    if (index == -1)
        return;
    unsigned int domen_length = strlen(domen);
    if (((Node**)hDNS)[index] == NULL)  // cell is empty
    {
        ((Node**)hDNS)[index] = NodeInicialisation(domen, ip, domen_length);
        ((Node**)hDNS)[index]->next_node = NULL;
    }
    else if (((Node**)hDNS)[index] != NULL)    // cell is not empty
    {
        /* Node* temp = ((Node**)hDNS)[index];       // detour to find nides with the same domen and remove them
         while (temp != NULL)
         {
             if (strcmp(temp->domen, domen) == 0)
             {
                 temp->ip = ip;
                 return;
             }
             temp = temp->next_node;
         }*/
        Node* p = NodeInicialisation(domen, ip, domen_length);
        p->next_node = ((Node**)hDNS)[index];
        ((Node**)hDNS)[index] = p;
    }
}




void LoadHostsFile(DNSHandle hDNS, const char* hostsFilePath)  
{
    if ((Node**)hDNS == NULL || hostsFilePath == NULL)
        return;  // *
    FILE* file = NULL;
    file = fopen(hostsFilePath, "r");
    if (file == NULL)
        return;    //*there is an opportunity to use CEXCEPTION_T but we can't change main file
    unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
    char temp_domen[201];
    while (fscanf_s(file, "%d.%d.%d.%d %s", &ip1, &ip2, &ip3, &ip4, temp_domen, 200) != EOF)
    {
        IPADDRESS ip = (ip1 & 0xFF) << 24 |
            (ip2 & 0xFF) << 16 |
            (ip3 & 0xFF) << 8 |
            (ip4 & 0xFF);
        InsertToServer(hDNS, temp_domen, ip);
    }
    fclose(file);
}

IPADDRESS DnsLookUp(DNSHandle hDNS, const char* hostName)
{
    Node* temp = ((Node**)hDNS)[getHash(hostName)];
    while (temp != NULL)
    {
        if (strcmp(temp->domen, hostName) == 0)
            return temp->ip;
        temp = temp->next_node;
    }
    return INVALID_IP_ADDRESS;
}

void ShutdownDNS(DNSHandle hDNS)
{
    if((Node**)hDNS == NULL)
       return;
    for (unsigned int i = 0; i < SIZE; ++i)
    {
        Node* temp = ((Node**)hDNS)[i];
        //   printf("%d", i);  //  * helps to visualize hash-table
        while (temp != NULL)
        {
            //       printf(" element "); //*
            Node* temp_2 = temp->next_node;
            free(temp);
            temp = temp_2;
        }
        //   printf("\n"); //  *
    }
    free((Node**)hDNS);
}


