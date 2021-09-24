
/*************************************************************************
   LAB 1                                                                

    Edit this file ONLY!

*************************************************************************/



#include "dns.h"
#include <stdio.h>

typedef struct _Node
{
    char* domen;
    IPADDRESS ip;
    struct _Node* next_node;
}Node;
int SIZE = 15000;
int node_amount = 0;
DNSHandle InitDNS()
{
    DNSHandle hDNS = (DNSHandle)(Node**)calloc(SIZE, sizeof(Node*));
    if ((Node**)hDNS != NULL)
        return hDNS;
    return (DNSHandle)0;
}
int getHash(const char* domen)
{
    unsigned int len = strlen(domen);
    unsigned int hash = 3000 + len * len;
    const char* temp = domen;
    while (*temp != '\0')
    {
        hash = ((hash << 4) + (unsigned int)(*temp)) % SIZE;
        ++temp;
    }
    hash ^= hash >> 3;
    if(len > 4)
    hash *=  domen[1] + domen[4];
    return hash % SIZE;
}
Node* NodeInicialisation( const char* domen, IPADDRESS ip, int domen_length)
{
    Node* node = (Node*)malloc(sizeof(Node));
    node->domen = (char*)malloc(sizeof(char) * (domen_length + 1));
    strcpy(node->domen, domen);
    node->ip = ip;
    return node;
}
//void TableResizing(DNSHandle hDNS)
//{
//    int size = SIZE;
//    SIZE *= 2;
//    DNSHandle new_hDNS = InitDNS();
//    for (unsigned int i = 0; i < size; ++i)
//    {
//        Node* temp = ((Node**)hDNS)[i];
//        while (temp != NULL)
//        {
//            InsertToServer(new_hDNS, temp->domen, (IPADDRESS)(temp->ip));
//            Node* temp_2 = temp->next_node;
//            free(temp);
//            temp = temp_2;
//        }
//
//    }
//    free(hDNS);
//    (Node**)hDNS = (Node**)new_hDNS;
//}

void InsertToServer(DNSHandle hDNS,const char* domen, IPADDRESS ip)
{
    if ((Node**)hDNS == NULL || domen == NULL)
        return;
    unsigned int index = getHash(domen);
    unsigned int domen_length = strlen(domen);
    if (((Node**)hDNS)[index] == NULL)
    {
        ((Node**)hDNS)[index] = NodeInicialisation(domen, ip, domen_length);
        ((Node**)hDNS)[index]->next_node = NULL;
    }
    else if (((Node**)hDNS)[index] != NULL)
    {
       /* Node* temp = ((Node**)hDNS)[index];
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
  // if ((++node_amount) / SIZE >= 0.7)
    //     hDNS = TableResizing(hDNS);
}

void LoadHostsFile( DNSHandle hDNS, const char* hostsFilePath )
{
    FILE* file = NULL;
    file = fopen(hostsFilePath, "r");
    if (file == NULL)
        return;
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

void ShutdownDNS( DNSHandle hDNS )
{
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
