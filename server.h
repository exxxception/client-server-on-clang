typedef struct ClientNode 
{
    int data;
    struct ClientNode* prev;
    struct ClinetNode* next;
    // char ip[16];
    // char name[31];
} ClientList;

ClientList* newNode(int socket) 
{
    ClientList *np = (ClientList*)malloc(sizeof(ClientList));
    np->data = socket;
    np->prev = NULL;
    np->next = NULL;
    return np;
}