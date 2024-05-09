#include <stdio.h>
#include <stdlib.h>
typedef short bool;
#define true 1
#define false 0
struct Nodemem
{
    int memorysize,nodenumber;
    bool taken;
    struct Nodemem *left;
    struct Nodemem *right;
};

/**
 * @brief  Initialize the memory tree
 * 
 * @param memavailable  The total memory available
 * @param isLeft  A boolean to indicate if the current node is a left node
 * @return struct Nodemem* 
 */
struct Nodemem* InitialiseMemory(int memavailable,int nodenumber)
{
    if (memavailable < 1) { return NULL; }
    struct Nodemem *root = malloc(sizeof(struct Nodemem));
    root->nodenumber = nodenumber;
    root->memorysize = memavailable;
    root->taken = false;
    root->left = InitialiseMemory(memavailable / 2,2*nodenumber);
    root->right = InitialiseMemory(memavailable / 2,2*nodenumber+1);
    return root;
}


/**
 * @brief  Check if memory is available for a process
 * 
 * @param root  The root of the memory tree
 * @return true 
 * @return false 
 */
bool CheckMemoryAvailability(struct Nodemem* root){
    if (root == NULL) { return false; }
    if (root->taken) { return false; }
    if (root->left) { 
        if (root->left->taken || root->right->taken) { return false; }
        return CheckMemoryAvailability(root->left) && CheckMemoryAvailability(root->right);
    }
    return true;
}
/**
 * @brief  Clear the memory tree
 * 
 * @param root  The root of the memory tree
 */
void ClearMemory(struct Nodemem* root)
{
    if (root == NULL) { return; }
    ClearMemory(root->left);
    ClearMemory(root->right);
    free(root);
}

/**
 * @brief  Allocate memory for a process
 * 
 * @param root  The root of the memory tree
 * @param memrequired  The memory required by the process
 * @param p  The process that needs the memory
 * @param totalmemory  The total memory available
 * @param f  The file pointer to the log file
 * @return true 
 * @return false 
 */
struct Nodemem *randnode1 = NULL, *randnode2 = NULL;
bool AllocateMemory(struct Nodemem* root, int memrequired) {
    if (root == NULL) { return false; }
    if (root->taken) { return false; }
    if (root->memorysize == memrequired) {
        if (root->left) { if (root->left->taken || root->right->taken) { root->taken = true; return false; } }
        if (root->taken) { return false; }
        if (CheckMemoryAvailability(root)){
            root->taken = true;
            if (randnode1 == NULL) { randnode1 = root; }
            else { randnode2 = root; }
            return true;
        }
        return false;
    }
    if (root->memorysize > memrequired) {
        if (AllocateMemory(root->left, memrequired)) { return true; }
        else if (AllocateMemory(root->right, memrequired)) { return true; }
        else {
            if (CheckMemoryAvailability(root)){
                root->taken = true;
                if (randnode1 == NULL) { randnode1 = root; }
                else { randnode2 = root; }
                return true;
            }
        }
    }
    return false;
}
/**
 * @brief  Check if a node is on the left side of the tree
 * 
 * @param root  The root of the memory tree
 * @param node  The node to check
 * @param condition Condition that is initially true but set as false if the function starts checking in the right side of the root
 * @return true 
 * @return false 
 */
bool CheckifonLeftSide(struct Nodemem* root, struct Nodemem* node, bool condition) {
    if (root == NULL) { return false; }
    if (node == root) { return condition; }
    bool onLeft = CheckifonLeftSide(root->left, node, true);
    if (onLeft) { return true; }
    bool onRight = CheckifonLeftSide(root->right, node, false);
    return onRight;
}

void PrintMemory(struct Nodemem* root)
{
    if (root == NULL) { return; }
    printf("Memory size: %d, Taken: %d, Node number = %d\n", root->memorysize, root->taken, root->nodenumber);
    sleep(2);
    PrintMemory(root->left);
    PrintMemory(root->right);
}

void TreeToArray(struct Nodemem *root, struct Nodemem *array[]) {
    // Initialize queue for level order traversal
    struct Nodemem *queue[100];
    int front = -1, rear = -1;

    // Enqueue root node
    queue[++rear] = root;

    // Perform level order traversal
    while (front != rear) {
        // Dequeue a node from the queue
        struct Nodemem *node = queue[++front];
        
        // Store the node in the array
        array[++rear] = node;

        // Enqueue left child
        if (node->left)
            queue[++rear] = node->left;
        
        // Enqueue right child
        if (node->right)
            queue[++rear] = node->right;
    }
}

void a7aneek(){
    //struct Nodemem* root = malloc(sizeof(struct Nodemem));
    // root->memorysize = 16;
    // root->taken = false;
    // root->isLeft = true;
    // root->left = malloc(sizeof(struct Nodemem));
    // root->left->memorysize = 8;
    // root->left->taken = false;
    // root->left->isLeft = true;
    // root->left->left = malloc(sizeof(struct Nodemem));
    // root->left->left->memorysize = 4;
    // root->left->left->taken = false;
    // root->left->left->isLeft = true;
    // root->left->left->left = NULL;
    // root->left->left->right = NULL;
    // root->left->right = malloc(sizeof(struct Nodemem));
    // root->left->right->memorysize = 4;
    // root->left->right->taken = false;
    // root->left->right->isLeft = false;
    // root->left->right->left = NULL;
    // root->left->right->right = NULL;
    // root->right = malloc(sizeof(struct Nodemem));
    // root->right->memorysize = 8;
    // root->right->taken = false;
    // root->right->isLeft = false;
    // root->right->left = malloc(sizeof(struct Nodemem));
    // root->right->left->memorysize = 4;
    // root->right->left->taken = false;
    // root->right->left->isLeft = true;
    // root->right->left->left = NULL;
    // root->right->left->right = NULL;
    // root->right->right = malloc(sizeof(struct Nodemem));
    // root->right->right->memorysize = 4;
    // root->right->right->taken = false;
    // root->right->right->isLeft = false;
    // root->right->right->left = NULL;
    // root->right->right->right = NULL;
}

int main(){
    struct Nodemem* root = InitialiseMemory(1024,1);
    //bool x = AllocateMemory(root,4);
    //bool y = AllocateMemory(root,4);
    //bool z = AllocateMemory(root,10);
    PrintMemory(root);
    //printf("%d %d %d\n",x,randnode1->memorysize,CheckifonLeftSide(root,randnode1,true));
    //printf("%d %d %d\n",y,randnode2->memorysize,CheckifonLeftSide(root,randnode2,true));
    //printf("%d",z);
    ClearMemory(root);
}