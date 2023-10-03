#include<iostream>
#include<cstdlib>
using namespace std;
struct HuffmanTreeNode
{
   char data;
   int frequency;
   struct HuffmanTreeNode* left;
   struct HuffmanTreeNode*  right;
};
struct minHeap
{
    int currentSize;
    int capacity;
    struct HuffmanTreeNode** array;
};
struct HuffmanTreeNode* Node(char data,int frequency)
{
    struct HuffmanTreeNode* tempNode=(struct HuffmanTreeNode*)malloc(sizeof(struct HuffmanTreeNode));
    tempNode->left=NULL;
    tempNode->right=NULL;
    tempNode->frequency=frequency;
    tempNode->data=data;
    return tempNode;
}
struct minHeap* createMinHeap(int capacity)
{
    struct minHeap* MinHeap=(struct minHeap*)malloc(sizeof(struct minHeap));
    MinHeap->currentSize=0;
    MinHeap->capacity=capacity;
    MinHeap->array=(struct HuffmanTreeNode**)malloc(MinHeap->capacity*sizeof(struct HuffmanTreeNode*));
    return MinHeap;
}
void SwapHuffmanTreeNode(struct HuffmanTreeNode** NodeA,struct HuffmanTreeNode** NodeB)
{
    struct HuffmanTreeNode* temporaryNode=*NodeA;
    *NodeA= *NodeB;
    *NodeB = temporaryNode;
}
void minHeapify(struct minHeap* MinHeap,int index)
{
    int small=index;
    int left=2*index+1;
    int right=2*index+2;
    if(left< MinHeap->currentSize &&MinHeap->array[left]->frequency<MinHeap->array[small]->frequency)
    {
        small=left;
    }
    if(right< MinHeap->currentSize &&MinHeap->array[right]->frequency<MinHeap->array[small]->frequency)
    {
        small=right;
    }
    if(small!=index)
    {
        SwapHuffmanTreeNode(&MinHeap->array[small],&MinHeap->array[index]);
        minHeapify(MinHeap,small);
    }
}
int isSizeOne(struct minHeap* minHeap)
{
    return(minHeap->currentSize==1);
}
struct HuffmanTreeNode* MinimumValueOfNode(struct minHeap* MinHeap)
{
    struct HuffmanTreeNode* temporaryNode=MinHeap->array[0];
    MinHeap->array[0]=MinHeap->array[MinHeap->currentSize-1];
    --MinHeap->currentSize;
    minHeapify(MinHeap,0);
}
void InsertNewNode(struct minHeap* MinHeap,struct HuffmanTreeNode* TreeNode)
{
    MinHeap->currentSize+=1;
    int i=MinHeap->currentSize-1;
    while(i&&TreeNode->frequency<MinHeap->array[(i-1)/2]->frequency)
    {
        MinHeap->array[i]=MinHeap->array[(i-1)/2];
        i=(i-1)/2;
    }
    MinHeap->array[i]=TreeNode;
}
void BuildMinHeap(struct minHeap* MinHeap)
{
    int n=MinHeap->currentSize-1;
    int i;
    for(int i=(n-1)/2;i>=0;--i)
    {
        minHeapify(MinHeap,i);
    }
}
void printArray(int array[])
{
    for(int i=0;i<sizeof(array);i++)
    {
        cout<<array[i];
    }
    cout<<endl;
}
int LeafNodeChecker(struct HuffmanTreeNode* root)
{
    return !(root->left)&&!root->right;
}
struct minHeap* CreateandBuildMinHeap(char data[],int frequency[],int size)
{
    struct minHeap* MinHeap=createMinHeap(size);
    for(int i=0;i<size;i++)
    {
        MinHeap->array[i]=Node(data[i],frequency[i]);
        
    }
}