#include<stdio.h>
#include<stdlib.h>
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
    return temporaryNode;
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
void printArray(int array[],int n)
{
    /*for(int i=0;i<n;i++)
    {
        cout<<array[i];
    }
    cout<<endl;*/
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
    MinHeap->currentSize=size;
    BuildMinHeap(MinHeap);
    return MinHeap;
}
struct HuffmanTreeNode* BuildHuffmanTree(char data[],int frequency[],int size)
{
    struct HuffmanTreeNode* left;
    struct HuffmanTreeNode* right;
    struct HuffmanTreeNode* top;
    struct minHeap* MinHeap=CreateandBuildMinHeap(data,frequency,size);
    while(!isSizeOne(MinHeap))
    {
        left=MinimumValueOfNode(MinHeap);
        right=MinimumValueOfNode(MinHeap);
        top=Node('$',left->frequency+right->frequency);
        top->left=left;
        top->right=right;
        InsertNewNode(MinHeap,top);
    }
    return MinimumValueOfNode(MinHeap);
}
void printCodes(struct HuffmanTreeNode* root,int array[],int top)
{
    if(root->left)
    {
        array[top]=0;
        printCodes(root->left,array,top+1);
    }
    if(root->right)
    {
        array[top]=1;
        printCodes(root->right,array,top+1);
    }
    if(LeafNodeChecker(root))
    {
        //cout<<root->data<<" : ";
        printArray(array,top);
    }
}
void HuffmanCodes(char data[],int frequency[],int size)
{
    struct HuffmanTreeNode* root=BuildHuffmanTree(data,frequency,size);
    int array[100],top=0;
    printCodes(root,array,top);
}
int main() {
    //freopen("input.txt","r",stdin);
    //freopen("output.txt","w",stdout);
    int size;
   // printf("Enter the size of the character and frequency arrays: ");
    scanf("%d", &size);
    char arr[size];
    int freq[size];
 //   printf("Enter characters and frequencies:\n");
    for (int i = 0; i < size; i++) {
     //   printf("Character %d: ", i + 1);
        scanf(" %c", &arr[i]); 
      //  printf("Frequency %d: ", i + 1);
        scanf("%d", &freq[i]);
    }
    HuffmanCodes(arr, freq, size);
    return 0;
}
