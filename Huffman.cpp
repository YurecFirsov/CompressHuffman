#include<iostream>
#include<fstream>
#include<vector>
#include <queue>
#include<string>
#include "Huffman.h"
#include <sstream>

using namespace std;

struct Node                                                                                            //структура узла дерева Хаффмана
{
    unsigned char character;
    long count;
    Node* left;
    Node* right;
    unsigned char LeafSize=255;
    Node(unsigned char character_=0,long count_=0,Node* left_=nullptr,Node* right_=nullptr)
    {
        character=character_;
        count=count_;
        left=left_;
        right=right_;
    }


};
void delete_Node(Node *tree) {
    if (!tree) return;
    delete_Node(tree->left);
    delete_Node(tree->right);
    delete tree;
}
void MinHeap(vector<Node*> &node,int min ,int length)
{
    int least=min;
    if(2*min+1<=length&&node[2*min+1]->count<node[min]->count)
    {
        least=2*min+1;
        if(2*min+2<=length&&node[2*min+2]->count<node[2*min+1]->count)
            least=2*min+2;
    }
    else if(2*min+2<=length&&node[2*min+2]->count<node[min]->count)
        least=2*min+2;
    if(least!=min)
    {
        swap(node[min],node[least]);
        MinHeap(node,least,length);
    }
}
Node* GetMinChar(vector<Node*> &node)
{
    if(node.size()<1)
        return nullptr;
    Node* minimal=node[0];
    node[0]=node.back();
    node.pop_back();
    MinHeap(node,0,node.size()-1);
    return minimal;
}
void Push_in_MinHeap(vector<Node*> &node,Node* element)
{
    node.push_back(element);
    int i=node.size()-1;
    while(i>0&&node[(i-1)/2]->count>node[i]->count)
    {
        swap(node[i],node[(i-1)/2]);
        i=(i-1)/2;
    }
}
void Build_Minheap(vector<Node*> &node,int length)
{
    for(int i=(length-1)/2;i>=0;--i)
    {
        MinHeap(node,i,length);
    }
}
void CodeTable(Node* Root,char onecode[],int index,vector<long long int> &Huffman_codemap)    //таблица кодов символов
{
    if(Root->right)
    {
        onecode[index]='1';
        CodeTable(Root->right,onecode,index+1,Huffman_codemap);
    }
    if(Root->left)
    {
        onecode[index]='0';
        CodeTable(Root->left,onecode,index+1,Huffman_codemap);

    }

    if(!Root->left&&!Root->left)
    {
        for(int i=0;i<index;++i)
        {
            if(onecode[i]=='1'){
                Huffman_codemap[Root->character]^=(1<<i);
            }
        }
        Huffman_codemap[Root->character]^=(1<<index);
    }
}
void WriteCodes(IOutputStream& input,Node* Root)                                                          //запись таблицы в файл
{
    if(!Root->left&&!Root->right)
    {
        input.Write('1');
        input.Write(Root->character);
    }
    else
    {
        input.Write('0');
        WriteCodes(input,Root->left);
        WriteCodes(input,Root->right);
    }
}
Node* Huffman(long long int Count[])                                                                //алгоритм Хаффмана
{
    vector<Node*>minheap;
    for(int i=0;i<256;++i) //256 - размер ascii
        if(Count[i]!=0)
            minheap.push_back(new Node(i,Count[i]));
    Build_Minheap(minheap,minheap.size()-1);
    while(minheap.size()!=1)
    {
        Node* tmp=new Node(-1,0,GetMinChar(minheap),GetMinChar(minheap));
        tmp->count=tmp->left->count+tmp->right->count;
        Push_in_MinHeap(minheap,tmp);
    }
    return(minheap[0]);
}
void Write_compressed(vector<byte> & input, IOutputStream& output,vector<long long int > &Huffman_codemap)    //Запись сжатых данных в файл
{
    unsigned char bits_8;
    long long int counter=0;
    //unsigned char ch;
    size_t size = input.size();
    size_t when_fit = (1<<15-1);
    int iterations = size/2;
    for (auto& ch:input)
    {
        if(iterations==0)
            break;
        --iterations;
        long long int temp=Huffman_codemap[ch];
        while(temp!=1)
        {
            bits_8<<=1;
            if((temp&1)!=0)
                bits_8|=1;
            temp=temp>>1;
            counter++;
            if(counter==8)
            {
                output.Write(bits_8);
                counter=bits_8=0;
            }
        }

    }
    input.erase(input.begin(),input.begin()+size/2);
    input.shrink_to_fit();
    for (auto& ch:input)
    {
        long long int temp=Huffman_codemap[ch];
        while(temp!=1)
        {
            bits_8<<=1;
            if((temp&1)!=0)
                bits_8|=1;
            temp=temp>>1;
            counter++;
            if(counter==8)
            {
                output.Write(bits_8);
                counter=bits_8=0;
            }
        }
    }
    input.clear();
    input.shrink_to_fit();
    while(counter!=8)
    {
        bits_8<<=1;
        counter++;
    }
    output.Write(bits_8);
    //input.push_back(bits_8);
    //input.erase(input.begin(),input.begin()+size);
    //input.shrink_to_fit();
    ///
    //input.erase(input.begin(),input.begin()+ost);
    //input.shrink_to_fit();
}

Node* Make_Huffman_tree(IInputStream &input)                                                        //Создание дерева Хаффмана из сжатого файла
{
    unsigned char ch;
    input.Read(ch);
    if(ch=='1')
    {
        input.Read(ch);
        return (new Node(ch));
    }
    else
    {
        Node* left=Make_Huffman_tree(input);
        Node* right=Make_Huffman_tree(input);
        return(new Node(-1,0,left,right));
    }
}
void decompress(IInputStream &input, IOutputStream &output,Node* Root,long long int Total_Freq)              //Decode each binary symbol according to the tree
{
    bool eof_flag = false;
    unsigned char bits_8;
    Node* pointer = Root;
    while(input.Read(bits_8))
    {
        int counter=7;
        while(counter>=0)
        {
            if(!pointer->left&&!pointer->right)
            {
                output.Write(pointer->character);
                Total_Freq--;
                if(!Total_Freq)
                {
                    eof_flag=true;
                    break;
                }
                pointer=Root;
                continue;
            }
            if((bits_8&(1<<counter)))
            {
                pointer=pointer->right;
                counter--;
            }
            else
            {
                pointer=pointer->left;
                counter--;
            }
        }
        if(eof_flag)
            break;
    }
}


void Encode(IInputStream& original, IOutputStream& compressed)
{
    vector<long long int> Huffman_codemap;                                                         //Вектор для хранения кодов символов
    Huffman_codemap.resize(256);
    long long int Count[256]={0};                                                             //массив частот символов ascii

    vector<byte> original_vec;
    unsigned char ch;
    while(original.Read(ch)) {
        Count[ch]++;
        original_vec.push_back(ch);
    }
    if(original_vec.size()==0)
        return;
    Node* tree=Huffman(Count);                                                                 //create Huffman tree

    //new
    std::stringstream ss;
    ss<<tree->count;
    std::string str = ss.str();
    for(auto symbol: str)
        compressed.Write(symbol);
    compressed.Write(',');
    //

    WriteCodes(compressed,tree);
    compressed.Write(' ');
    char single_code[32];
    CodeTable(tree,single_code,0,Huffman_codemap);
    Write_compressed(original_vec,compressed,Huffman_codemap);
    delete_Node(tree);
    //size_t size=original_vec.size();
    //size_t when_fit = 1<<13-1;
    //for(int i=0;i<size;++i)
    //for (auto symbol:original_vec)
    //{
        //compressed.Write(symbol);
        //compressed.Write(*original_vec.begin());
        //original_vec.erase(original_vec.begin());
        //if((i&when_fit)==0)
        //    original_vec.shrink_to_fit();
    //}


}


void Decode(IInputStream& compressed, IOutputStream& original)
{

    long long int Total_freq=0;
    unsigned char ch;
    while(compressed.Read(ch))                                                                  //read Original total frequency from file
    {
        if(ch==',')
            break;
       Total_freq*=10;
       Total_freq+=ch-'0';
    }
    if (Total_freq==0)
        return;
    Node* Huffman_tree=Make_Huffman_tree(compressed);                                         //Remake Huffman tree from file
    compressed.Read(ch);                                                                       //Read Extra space between compressed data and tree from file
    decompress(compressed,original,Huffman_tree,Total_freq);
    delete_Node(Huffman_tree);
}

