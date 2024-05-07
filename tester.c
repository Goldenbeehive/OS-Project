#include <stdio.h>
#include <stdlib.h>
typedef short bool;
#define true 1
#define false 0

struct Node
{
    int memorysize;
    bool taken;
    struct Node *left;
    struct Node *right;
};

struct Node* InitialiseMemory(int memavailable)
{
    if (memavailable < 2) { return NULL; }
    struct Node *root = malloc(sizeof(struct Node));
    root->memorysize = memavailable;
    root->taken = false;
    root->left = InitialiseMemory(memavailable / 2);
    root->right = InitialiseMemory(memavailable / 2);
    return root;
}

void ClearMemory(struct Node* root)
{
    if (root == NULL) { return; }
    ClearMemory(root->left);
    ClearMemory(root->right);
    free(root);
}

bool AllocateMemory(struct Node* root, int memrequired) {
    if (root == NULL) { return false; }
    if (root->taken || root->memorysize < memrequired) { return false; }
    if (root->left) { if (root->left->taken && root->right->taken) {return false; } }
    if (root->memorysize == memrequired) {
        if (root->taken) { return false; }
        root->taken = true;
        printf("Memory size: %d, Taken: %d\n", root->memorysize, root->taken);
        return true;
    }
    if (root->memorysize >= memrequired) {
        if (AllocateMemory(root->left, memrequired)) { return true; }
        else if (AllocateMemory(root->right, memrequired)) { return true; }
        else {
            root->taken = true;
            printf("Memory size: %d, Taken: %d\n", root->memorysize, root->taken);
            return true;
        }
    }
    return false;
}
int count = 0;
void PrintMemory(struct Node* root)
{
    if (root == NULL) { return; }
    //printf("Memory size: %d, Taken: %d\n", root->memorysize, root->taken);
    count++;
    PrintMemory(root->left);
    PrintMemory(root->right);
}

int main(){
    struct Node* root = InitialiseMemory(1024);
    PrintMemory(root);
    printf("Count: %d\n", count);
    ClearMemory(root);
}