#include <iostream>
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <string>

using std::vector;
using std::list;
using std::string;

const int MAX_CHARS_PER_LINE = 10240;
const int MAX_TOKENS_PER_LINE = 1000;
const char* const DELIMITER = " ,\t";

int flag = 0;
char hash[]="#";
int addWireFlag = 0;
int gcount = 0;

struct Node
{
    char name[1000];
    float delay;
    int size;
    char dataType[1000];
    char nodeType[1000];
    char color[100];
    int id;
    float dist;
    struct Node *next;
    
};

struct Component
{
    char name[100];
    int size;
    float delay;
    
};

Component * comp = new Component[70];
vector<Node *> graph;

int CreateStructure(char inputArray[1000][1000], int &k, Node * pointer);
int WriteVerilog(char inputArray[1000][1000], int k, char** argv);
int CreateVerilog(char inputArray[1000][1000], int k, char** argv, Node * pointer);
int GetSizeType(char input[1000], int *size, char datatype[1000]);
void InitializeComp();
float GetDelay(char name[1000], int size);
//int addEdge(vector<Node *> &graph,int src,int dest);
int addEdge(vector<Node *> &graph,Node src,Node dest);
void print(int v,vector<Node *> &graph);
void longestpath (vector<Node *> &G, int v);
void topologicalsort(vector<Node *> &G, std::list<Node> &L);
int tsvisit(vector<Node *> &G,list<Node> &L,Node *temp);



int main(int argc, char** argv)
{
    if ( argc != 3 ){
        
        cout<<"USAGE: dpgen <InputFileName> <OutputFileName>\n";
        return 1;
    }
    else {
        
        //     cout<< "Filename:"<<argv[1]<<endl;
        
        ifstream fin ( argv[1] );
        
        if ( !fin.is_open() ) {
            cout<<"Could not open file\n";
            return 1;
        }
    }
    
    char inputArray[1000][1000] = {};
    Node * pointer = new Node[50];
    
    int k=0;
    
    
    ifstream fin;
    fin.open(argv[1]);
    if (!fin.good())
    {
        cout<<"Input file not found. Please place the input file in the current folder."<<endl;
        return 1;
    }
    
    
    while (!fin.eof())
    {
        
        char buf[MAX_CHARS_PER_LINE];
        fin.getline(buf, MAX_CHARS_PER_LINE);
        // cout << buf << endl;
        
        
        int n = 0;
        
        
        const char* token[MAX_TOKENS_PER_LINE] = {};
        
        
        token[0] = strtok(buf, " \t");
        if (token[0] != NULL)
        {
            for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
            {
                token[n] = strtok(0, DELIMITER);
                if (!token[n]) break;
            }
        }
        else
            continue;
        
        
        
        for (int i = 0; i<= n; i++)
        {
            if(token[i])
                strcpy (inputArray[k], token[i]);
            else
                strcpy (inputArray[k], hash);
            k++;
        }
    }
    
    
    //for (int m = 0; m<k; m++)
    //    cout << inputArray[m] << endl;
    
    
    
    CreateVerilog(inputArray,k,argv,pointer);
    
    
    if(!flag)
    {
        //cout <<"Graph Size is "<<gcount<<endl;
        //print(gcount,graph);
        
        longestpath (graph,gcount);
        
        //print(gcount,graph);
    }
    
}


int WriteVerilog(char outputArray[1000][1000], int k, char** argv)
{
    //for (int m = 0; m<k; m++)
    //    cout << outputArray[m] << endl;
    
    
    ofstream fout;
    fout.open(argv[2]);
    
    for (int m= 0; m<k; m++)
    {
        if (strcmp (outputArray[m],hash)!=0)
            fout << outputArray[m] <<" ";
        else
            fout<<endl;
    }
    fout.close();
    return 0;
    
}


int CreateVerilog(char inputArray[1000][1000], int k, char** argv, Node * pointer)
{
    char outputArray[1000][1000] = {};
    int m = 0;
    
    
    strcpy(outputArray[m], "`timescale 1ns / 1ps");
    m++;
    strcpy(outputArray[m], "\n");
    m++;
    strcpy(outputArray[m], "module");
    m++;
    
    strcpy(outputArray[m], "circuit");
    strcat(outputArray[m], "(");
    m++;
    
    InitializeComp();
    
    CreateStructure(inputArray, k, pointer);
    
    
    if (flag)
        return 0;
    
    int count=0;
    
    for (int i=0;strcmp(pointer[i].name,"#")!=0;i++)
    {
        pointer[i].id=count;
        count=count+1;
    }
    //cout<<endl<<"Count is "<<count<<endl;
    
    
    
    for(int i=0;i<count;i++)
    {
        graph.push_back(NULL);
        
    }
    
    
    gcount = count;
    if (flag)
        return 0;
    
    
    for(int j= 0;j<k;j++)
    {
        strcpy(outputArray[m], pointer[j].name);
        if(strcmp(pointer[j+1].name, hash)!=0 && strcmp(pointer[j+1].nodeType, "wire")!=0 && strcmp(pointer[j+1].nodeType, "register")!=0 )
        {
            strcat(outputArray[m], ",");
            m++;
            
        }
        else
        {
            strcat(outputArray[m], ",");
            m++;
            strcat(outputArray[m], "clk,");
            m++;
            strcat(outputArray[m], "rst)");
            m++;
            strcat(outputArray[m], hash);
            m++;
            break;
        }
    }
    
    strcat(outputArray[m], hash);
    m++;
    
    strcat(outputArray[m], "input");
    m++;
    strcat(outputArray[m], "clk,");
    m++;
    strcat(outputArray[m], "rst;");
    m++;
    strcat(outputArray[m], hash);
    m++;
    
    
    
    int j=0;
    
    
    //Variable initialization
    for (j=0; j<k; j++)
    {
        
        int inputSize=0;
        char type[10];
        
        if(strcmp(inputArray[j], "input") == 0 || strcmp(inputArray[j], "output") == 0 || strcmp(inputArray[j], "wire") == 0 || strcmp(inputArray[j], "register") == 0)
        {
            while(strcmp(inputArray[j], hash)!=0)
            {
                if(strcmp(inputArray[j], "input") == 0 || strcmp(inputArray[j], "output") == 0 || strcmp(inputArray[j], "wire") == 0 || strcmp(inputArray[j], "register") == 0)
                {
                    if(strcmp(inputArray[j], "register") != 0)
                        strcpy(outputArray[m], inputArray[j]);
                    else
                        strcpy(outputArray[m], "wire");
                    m++;
                    j= j+2;
                    
                    for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                    {
                        if(strcmp(pointer[l].name, inputArray[j]) == 0)
                        {
                            inputSize= pointer[l].size;
                            strcpy(type, pointer[l].dataType);
                            break;
                        }
                    }
                    
                    if(strcmp(type, "Int") == 0)
                    {
                        strcat(outputArray[m], "signed");
                        m++;
                    }
                    
                    
                    char res[10];
                    int zero = 0;
                    
                    if(inputSize-1>0)
                    {
                        sprintf(res, "[%d:%d]", inputSize-1,zero);
                        strcat(outputArray[m], res);
                        m++;
                    }
                    
                }
                
                strcpy(outputArray[m], inputArray[j]);
                
                j++;
                m++;
                
                if (strcmp(inputArray[j], hash)!=0)
                    strcat(outputArray[m-1], ",");
                else
                {
                    strcat(outputArray[m-1], ";");
                    strcpy(outputArray[m], hash);
                    m++;
                    break;
                }
                
            }
            
        }
        else
            break;
    }
    
    
    if(addWireFlag)
    {
        for (int s=j;s<k;s++)
        {
            int inputSize=0;
            
            if(strcmp(inputArray[s], "wire") == 0 )
            {
                strcpy(outputArray[m], "wire");
                m++;
                s= s+2;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, inputArray[s]) == 0)
                    {
                        inputSize= pointer[l].size;
                        break;
                    }
                }
                
                char res[10];
                int zero = 0;
                
                if(inputSize-1>0)
                {
                    sprintf(res, "[%d:%d]", inputSize-1,zero);
                    strcat(outputArray[m], res);
                    m++;
                }
                
                
                
                strcpy(outputArray[m], inputArray[s]);
                
                strcat(outputArray[m], ";");
                m++;
                strcpy(outputArray[m], hash);
                m++;
                
            }
            
        }
        
    }
    
    
    strcat(outputArray[m], hash);
    m++;
    
    
    int cAdd = 1;
    int cSub = 1;
    int cMul = 1;
    int cDiv = 1;
    int cReg = 1;
    int cCompgt = 1;
    int cComplt = 1;
    int cCompeq = 1;
    int cMux = 1;
    int cShr = 1;
    int cShl = 1;
    int cMod = 1;
    
    
    
    for (int p=j; p<k; p++) {
        
        char operants[4][10] = {};
        int index=0;
        
        while(strcmp(inputArray[p], hash) != 0)
        {
            strcpy(operants[index], inputArray[p]);
            index++;
            
            if(strcmp(inputArray[p+1], hash) != 0)
                p=p+2;
            else
                break;
            
        }
        
        
        for (int p=0 ; p<index; p++) {
            
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++) {
                
                if(p==0)
                {
                    if(strcmp(pointer[l].name, operants[p]) == 0)
                        if(strcmp(pointer[l].nodeType,"input")==0)
                        {
                            flag = 1;
                            cout<<"result of an operation cannot be assigned to an input element. Verilog file creation failed"<<endl;
                            return 0;
                        }
                }
                else
                {
                    if(strcmp(pointer[l].name, operants[p]) == 0)
                        if(strcmp(pointer[l].nodeType,"output")==0)
                        {
                            flag = 1;
                            cout<<"Output element cannot be used as an input for an operation. Verilog file creation failed"<<endl;
                            return 0;
                        }
                    
                    
                }
                
            }
        }
        
    }
    
    
    
    for(int p=j;p<k;p++)
    {
        
        int outSize = 0;
        int inputsize = 0;
        int variablefinder = 0;
        Node destination = *new Node;
        
        
        if(strcmp(inputArray[p+3], "+") == 0)
        {
            strcpy(outputArray[m], "ADD");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "+")!=0)
                {
                    
                    strcpy(operants[index], inputArray[p]);
                    
                    index++;
                }
                
                p++;
                
            }
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    char type[] = "ADD";
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    
                    //  cout<<"struct- forloop"<<endl;
                    break;
                }
            }
            
            if(variablefinder==0)
            {
                flag = 1;
                //  cout<<"Hit ADD";
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "ADD_%d(", cAdd);
            strcpy(outputArray[m], res);
            
            // cout<<endl<<pointer[1].name <<endl;
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
                if(variablefinder==0)
                {
                    
                    flag = 1;
                    cout<<operants[i]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
            }
            
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            
            strcat(outputArray[m], hash);
            m++;
            
            cAdd++;
        }
        
        else if(strcmp(inputArray[p+3], "-") == 0)
        {
            strcpy(outputArray[m], "SUB");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "-")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    
                    char type[] = "SUB";
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    break;
                }
            }
            
            if(variablefinder==0)
            {
                flag = 1;
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "SUB_%d(", cSub);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                    
                    
                }
                
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[i]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cSub++;
        }
        
        else if(strcmp(inputArray[p+3], "*") == 0)
        {
            strcpy(outputArray[m], "MUL");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "*")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    char type[] = "MUL";
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    break;
                }
            }
            
            if(variablefinder==0)
            {
                flag = 1;
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "MUL_%d(", cMul);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                    
                }
                
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[i]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cMul++;
        }
        
        
        
        else if(strcmp(inputArray[p+3], "/") == 0)
        {
            // strcpy(outputArray[m], "DIV");
            // m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "/")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    
                    char type[] = "DIV";
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    break;
                }
            }
            
            
            if(variablefinder==0)
            {
                flag = 1;
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            
            
            
            
            
            //
            int isSigned=0;
            for (int v=1; v<index;v++)
            {
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[v]) == 0)
                    {
                        if(strcmp(pointer[l].dataType, "Int") == 0)
                        {
                            isSigned=1;
                            break;
                        }
                    }
                }
                if(isSigned)
                    break;
            }
            
            if (isSigned) {
                strcpy(outputArray[m], "SDIV");
                m++;
                
            }
            else
            {
                strcpy(outputArray[m], "DIV");
                m++;
                
            }
            
            //
            
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "DIV_%d(", cDiv);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l]);
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                    
                }
                
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[i]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cDiv++;
        }
        
        else if(strcmp(inputArray[p+3], "%") == 0)
        {
            strcpy(outputArray[m], "MOD");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "%")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    char type[] = "MOD";
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    break;
                }
            }
            
            if(variablefinder==0)
            {
                flag = 1;
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "MOD_%d(", cMod);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                    
                }
                
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cMod++;
        }
        
        
        else if(strcmp(inputArray[p+3], "<") == 0)
        {
            //strcpy(outputArray[m], "COMPLT");
            //m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "<")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            int sourceIndex=0;
            int isSigned =0 ;
            
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            sourceIndex = l;
                            
                        }
                        if(strcmp(pointer[l].dataType, "Int") == 0)
                            isSigned=1;
                        
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            char type[] = "COMPLT";
            pointer[sourceIndex].delay = GetDelay(type, outSize);
            destination=pointer[sourceIndex];
            
            if (isSigned) {
                strcpy(outputArray[m], "SCOMPLT");
                m++;
                
            }
            else
            {
                strcpy(outputArray[m], "COMPLT");
                m++;
                
            }
            
            
            
            
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "COMPLT_%d(", cComplt);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cComplt++;
        }
        
        else if(strcmp(inputArray[p+3], ">") == 0)
        {
            // strcpy(outputArray[m], "COMPGT");
            // m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], ">")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            int isSigned=0;
            
            int sourceIndex=0;
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            sourceIndex = l;
                            
                            //   char type[] = "COMPLT";
                            //   pointer[l].delay = GetDelay(type, pointer[l].size);
                            //   destination=pointer[l];
                        }
                        
                        if(strcmp(pointer[l].dataType, "Int") == 0)
                            isSigned=1;
                        
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            char type[] = "COMPGT";
            pointer[sourceIndex].delay = GetDelay(type, outSize);
            destination=pointer[sourceIndex];
            
            if (isSigned) {
                strcpy(outputArray[m], "SCOMPGT");
                m++;
                
            }
            else
            {
                strcpy(outputArray[m], "COMPLT");
                m++;
                
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "COMPGT_%d(", cCompgt);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    //addEdge(graph, destination, pointer[l] );
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, pointer[l], destination);
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cCompgt++;
        }
        
        else if(strcmp(inputArray[p+3], "==") == 0)
        {
            strcpy(outputArray[m], "COMPEQ");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "==")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            int sourceIndex =0;
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            sourceIndex = l;
                            
                            //   char type[] = "COMPLT";
                            //   pointer[l].delay = GetDelay(type, pointer[l].size);
                            //   destination=pointer[l];
                        }
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            char type[] = "COMPEQ";
            pointer[sourceIndex].delay = GetDelay(type, outSize);
            destination=pointer[sourceIndex];
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "COMPEQ_%d(", cCompeq);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cCompeq++;
        }
        
        else if(strcmp(inputArray[p+3], ">>") == 0)
        {
            //  strcpy(outputArray[m], "SHR");
            //  m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], ">>")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            int isSigned = 0;
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            char type[] = "SHR";
                            pointer[l].delay = GetDelay(type, pointer[l].size);
                            destination=pointer[l];
                        }
                        
                        if(strcmp(pointer[l].dataType, "Int") == 0)
                            isSigned=1;
                        
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            
            if (isSigned) {
                strcpy(outputArray[m], "SSHR");
                m++;
                
            }
            else
            {
                strcpy(outputArray[m], "SHR");
                m++;
                
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "SHR_%d(", cShr);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cShr++;
        }
        
        
        else if(strcmp(inputArray[p+3], "<<") == 0)
        {
            strcpy(outputArray[m], "SHL");
            m++;
            
            char operants[3][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "<<")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            char type[] = "SHL";
                            pointer[l].delay = GetDelay(type, pointer[l].size);
                            destination=pointer[l];
                        }
                        
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "SHL_%d(", cShl);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l] );
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cShl++;
        }
        
        
        
        else if(strcmp(inputArray[p+3], "?") == 0)
        {
            strcpy(outputArray[m], "MUX2x1");
            m++;
            
            char operants[4][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0 && strcmp(inputArray[p], "?")!=0 && strcmp(inputArray[p], ":")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            for (int d=0; d<index; d++)
            {
                variablefinder = 0;
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[d]) == 0)
                    {
                        if (d==0)
                        {
                            char type[] = "MUX2x1";
                            pointer[l].delay = GetDelay(type, pointer[l].size);
                            destination=pointer[l];
                        }
                        
                        variablefinder = 1;
                        if(outSize < pointer[l].size)
                        {
                            outSize = pointer[l].size;
                            break;
                        }
                    }
                }
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[d]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "MUX2x1_%d(", cMux);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l]);
                        if(i!=1)
                        {
                            variablefinder = 1;
                            inputsize= pointer[l].size;
                            
                            int pad = outSize - inputsize;
                            
                            if (strcmp(pointer[l].dataType, "Int") != 0)
                            {
                                if (pad > 1) {
                                    
                                    sprintf(res, "{%d'b", pad);
                                    strcat(outputArray[m], res);
                                    while(pad>0)
                                    {
                                        strcat(outputArray[m], "0");
                                        pad--;
                                    }
                                    strcat(outputArray[m], ",");
                                    strcat(outputArray[m], operants[i]);
                                    strcat(outputArray[m], "},");
                                    
                                }
                                
                                else
                                {
                                    strcat(outputArray[m], operants[i]);
                                    strcat(outputArray[m], ",");
                                }
                                break;
                            }
                            else
                            {
                                if (pad > 1) {
                                    
                                    if(inputsize>1)
                                        sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                    else
                                        sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                    
                                    strcat(outputArray[m], res);
                                    
                                }
                                
                                else
                                {
                                    strcat(outputArray[m], operants[i]);
                                    strcat(outputArray[m], ",");
                                }
                            }
                        }
                    }
                    
                }
                
            }
            
            strcat(outputArray[m], operants[1]);
            strcat(outputArray[m], ",");
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ");");
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cMux++;
        }
        
        else if(strcmp(inputArray[p], "wire") == 0)
        {
            while(strcmp(inputArray[p], hash) != 0)
                p++;
            
            p++;
            
            if(p==k)
                break;
        }
        
        
        
        else if (strcmp(inputArray[p+3], hash) == 0 && strcmp(inputArray[p+5], hash) != 0)
        {
            strcpy(outputArray[m], "REG");
            m++;
            
            char operants[2][10] = {};
            int index=0;
            
            while(strcmp(inputArray[p], hash) != 0)
            {
                
                if (strcmp(inputArray[p], "=")!=0)
                {
                    strcpy(operants[index], inputArray[p]);
                    index++;
                }
                
                p++;
                
            }
            
            
            variablefinder = 0;
            for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
            {
                if(strcmp(pointer[l].name, operants[0]) == 0)
                {
                    char type[] = "REG";
                    variablefinder = 1;
                    outSize= pointer[l].size;
                    pointer[l].delay = GetDelay(type, outSize);
                    destination=pointer[l];
                    break;
                }
            }
            
            if(variablefinder==0)
            {
                flag = 1;
                cout<<operants[0]<<" - is uninitialized. Verilog file creation failed."<<endl;
                return 0;
            }
            
            char res[20];
            
            sprintf(res, "#(.datawidth(%d))", outSize);
            
            strcpy(outputArray[m], res);
            m++;
            
            
            sprintf(res, "REG_%d(", cReg);
            strcpy(outputArray[m], res);
            
            
            for (int i=1; i<index; i++)
            {
                variablefinder = 0;
                for (int l=0; strcmp(pointer[l].name, hash)!=0; l++)
                {
                    if(strcmp(pointer[l].name, operants[i]) == 0)
                    {
                        addEdge(graph, destination, pointer[l]);
                        
                        
                        variablefinder = 1;
                        inputsize= pointer[l].size;
                        
                        int pad = outSize - inputsize;
                        
                        if (strcmp(pointer[l].dataType, "Int") != 0)
                        {
                            if (pad > 1) {
                                
                                sprintf(res, "{%d'b", pad);
                                strcat(outputArray[m], res);
                                while(pad>0)
                                {
                                    strcat(outputArray[m], "0");
                                    pad--;
                                }
                                strcat(outputArray[m], ",");
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], "},");
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                            break;
                        }
                        else
                        {
                            if (pad > 1) {
                                
                                if(inputsize>1)
                                    sprintf(res, "{{%d{%s[%d]}},%s},", pad, pointer[l].name, inputsize-1, pointer[l].name);
                                else
                                    sprintf(res, "{{%d{%s}},%s},", pad, pointer[l].name, pointer[l].name);
                                
                                strcat(outputArray[m], res);
                                
                            }
                            
                            else
                            {
                                strcat(outputArray[m], operants[i]);
                                strcat(outputArray[m], ",");
                            }
                        }
                        
                    }
                    
                    
                }
                
                if(variablefinder==0)
                {
                    flag = 1;
                    cout<<operants[i]<<" - is uninitialized. Verilog file creation failed."<<endl;
                    return 0;
                }
                
            }
            
            strcat(outputArray[m], operants[0]);
            strcat(outputArray[m], ",");
            strcat(outputArray[m], "clk,rst);");
            
            
            m++;
            
            strcat(outputArray[m], hash);
            m++;
            
            cReg++;
        }
        
        
        
        else
        {
            flag = 1;
            cout<<inputArray[p+3]<<" - unknown operator. Verilog file creation failed." <<endl;
            return 0;
        }
        
        
        
    }
    
    /* cout<<"Updatedstructure"<<endl;
     
     for (int l=0; strcmp(pointer[l].name, hash)!=0; l++) {
     cout<< pointer[l].name << " ";
     cout<< pointer[l].nodeType<<" ";
     cout<< pointer[l].dataType <<" ";
     cout<< pointer[l].delay <<" ";
     cout<< pointer[l].size <<endl;
     }*/
    
    
    
    
    strcat(outputArray[m], "endmodule");
    m++;
    
    for (int i=0; strcmp(pointer[i].name,hash)!=0; i++)
    {
        addEdge(graph,pointer[i], pointer [i]);
        
    }
    
    WriteVerilog(outputArray, m, argv);
    
    return 0;
}



int CreateStructure(char inputArray[1000][1000], int &k, Node * pointer)
{
    int size = 0;
    char datatype[5];
    
    int index = 0;
    
    int i=0;
    
    for (i=0; strcmp(inputArray[i+1], "=") != 0;i++)
    {
        if(strcmp(inputArray[i], "input") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "input");
                index++;
                i++;
            }
        }
        
        else if(strcmp(inputArray[i], "output") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" - is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "output");
                index++;
                i++;
            }
            
            
        }
        
        else if(strcmp(inputArray[i], "wire") == 0)
        {
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" - is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "wire");
                index++;
                i++;
            }
        }
        
        else if(strcmp(inputArray[i], "register") == 0)
        {
            
            i++;
            
            GetSizeType(inputArray[i], &size, datatype);
            
            if (flag)
                return 0;
            
            i++;
            
            while (strcmp(inputArray[i], hash)!=0)
            {
                for (int d=0; d<index; d++) {
                    if(strcmp(inputArray[i],pointer[d].name)==0)
                    {
                        flag=1;
                        cout <<inputArray[i]<<" - is initialized more than once. Verilog file creation failed"<<endl;
                        return 0;
                    }
                }
                
                
                strcpy(pointer[index].dataType, datatype);
                strcpy(pointer[index].name, inputArray[i]);
                pointer[index].size = size;
                pointer[index].delay = 0;
                strcpy(pointer[index].nodeType, "register");
                index++;
                i++;
            }
        }
        
        else
        {
            flag = 1;
            cout<<inputArray[i]<<" - unknown Data Input type. Verilog File creation failed"<<endl;
            return 0;
        }
    }
    
    int newindex = index;
    
    
    for (int m = i; m<k; m++) {
        
        if(strcmp(inputArray[m], "input") == 0 || strcmp(inputArray[m], "output") == 0 || strcmp(inputArray[m], "wire") == 0 || strcmp(inputArray[m], "register") == 0)
        {
            flag =1;
            cout<< "Signal declaration after component initialization is not allowed. Verilog file creation failed."<<endl;
            return 0;
        }
        
    }
    
    
    for (int l = 0; l< index; l++) {
        
        if(strcmp(pointer[l].nodeType, "output")== 0)
        {
            int m= i;
            
            for (m = i; m < k; m++)
            {
                if (strcmp(pointer[l].name, inputArray[m])== 0 && strcmp(inputArray[m+3], hash)!= 0 ) {
                    break;
                }
            }
            
            if (m!=k)
            {
                addWireFlag = 1;
                char newWire[10];
                char newDatatype[10];
                
                strcpy(inputArray[k++],inputArray[m]);
                strcpy(inputArray[k++],"=");
                strcpy(newWire, inputArray[m]);
                strcat(newWire,"_new");
                strcpy(inputArray[k++],newWire);
                strcpy(inputArray[k++],hash);
                
                strcpy(inputArray[m],newWire);
                
                strcpy(inputArray[k++], "wire");
                sprintf(newDatatype, "%s%d", pointer[l].dataType,pointer[l].size);
                strcpy(inputArray[k++], newDatatype);
                
                strcpy(inputArray[k++],newWire);
                
                strcpy(inputArray[k++],hash);
                
                
                strcpy(pointer[newindex].dataType, pointer[l].dataType);
                strcpy(pointer[newindex].name, newWire);
                pointer[newindex].size = pointer[l].size;
                pointer[newindex].delay = 0;
                strcpy(pointer[newindex].nodeType, "wire");
                newindex++;
                
                
                
            }
        }
    }
    
    index= newindex;
    strcpy(pointer[index].name, hash);
    
    
    /*cout<<endl <<"new input array"<<endl;
     
     for (int m = 0; m<k; m++)
     cout << inputArray[m] << endl;
     
     cout<<"structure"<<endl;
     
     for (int l=0; l<index; l++) {
     cout<< pointer[l].name << " ";
     cout<< pointer[l].nodeType<<" ";
     cout<< pointer[l].dataType <<" ";
     cout<< pointer[l].size <<endl;
     }*/
    
    // strcpy(pointer[index].name, hash);
    
    
    return 0;
}


int GetSizeType(char input[10], int *size, char datatype[5])
{
    if(strcmp(input, "Int1") == 0)
    {
        *size = 1;
        strcpy(datatype, "Int");
    }
    
    else if(strcmp(input, "Int8") == 0)
    {
        *size = 8;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int16") == 0)
    {
        *size = 16;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int32") == 0)
    {
        *size = 32;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "Int64") == 0)
    {
        *size = 64;
        strcpy(datatype, "Int");
    }
    else if(strcmp(input, "UInt1") == 0)
    {
        *size = 1;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt8") == 0)
    {
        *size = 8;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt16") == 0)
    {
        *size = 16;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt32") == 0)
    {
        *size = 32;
        strcpy(datatype, "UInt");
    }
    else if(strcmp(input, "UInt64") == 0)
    {
        *size = 64;
        strcpy(datatype, "UInt");
    }
    
    else
    {
        flag = 1;
        cout<<input<<" - datatype is not supported. Verilog file creation failed"<<endl;
        
    }
    
    return 0;
}


void InitializeComp()
{
    
    strcpy(comp[0].name, "REG");
    comp[0].size=1;
    comp[0].delay=0.754;
    
    strcpy(comp[1].name, "REG");
    comp[1].size=8;
    comp[1].delay=0.776;
    
    strcpy(comp[2].name, "REG");
    comp[2].size=16;
    comp[2].delay=0.800;
    
    strcpy(comp[3].name, "REG");
    comp[3].size=32;
    comp[3].delay=0.825;
    
    strcpy(comp[4].name, "REG");
    comp[4].size=64;
    comp[4].delay=0.828;
    
    
    
    
    strcpy(comp[5].name, "ADD");
    comp[5].size=1;
    comp[5].delay=0.718;
    
    strcpy(comp[6].name, "ADD");
    comp[6].size=8;
    comp[6].delay=1.178;
    
    strcpy(comp[7].name, "ADD");
    comp[7].size=16;
    comp[7].delay=1.293;
    
    strcpy(comp[8].name, "ADD");
    comp[8].size=32;
    comp[8].delay=1.523;
    
    strcpy(comp[9].name, "ADD");
    comp[9].size=64;
    comp[9].delay=1.983;
    
    
    
    
    
    strcpy(comp[10].name, "SUB");
    comp[10].size=1;
    comp[10].delay=0.718;
    
    strcpy(comp[11].name, "SUB");
    comp[11].size=8;
    comp[11].delay=1.584;
    
    strcpy(comp[12].name, "SUB");
    comp[12].size=16;
    comp[12].delay=1.749;
    
    strcpy(comp[13].name, "SUB");
    comp[13].size=32;
    comp[13].delay=1.939;
    
    strcpy(comp[14].name, "SUB");
    comp[14].size=64;
    comp[14].delay=2.460;
    
    
    
    
    
    
    strcpy(comp[15].name, "MUL");
    comp[15].size=1;
    comp[15].delay=0.689;
    
    strcpy(comp[16].name, "MUL");
    comp[16].size=8;
    comp[16].delay=3.131;
    
    strcpy(comp[17].name, "MUL");
    comp[17].size=16;
    comp[17].delay=3.362;
    
    strcpy(comp[18].name, "MUL");
    comp[18].size=32;
    comp[18].delay=5.717;
    
    strcpy(comp[19].name, "MUL");
    comp[19].size=64;
    comp[19].delay=8.169;
    
    
    
    strcpy(comp[20].name, "COMPGT");
    comp[20].size=1;
    comp[20].delay=0.712;
    
    strcpy(comp[21].name, "COMPGT");
    comp[21].size=8;
    comp[21].delay=1.997;
    
    strcpy(comp[22].name, "COMPGT");
    comp[22].size=16;
    comp[22].delay=1.572;
    
    strcpy(comp[23].name, "COMPGT");
    comp[23].size=32;
    comp[23].delay=1.684;
    
    strcpy(comp[24].name, "COMPGT");
    comp[24].size=64;
    comp[24].delay=1.908;
    
    
    
    
    strcpy(comp[25].name, "COMPLT");
    comp[25].size=1;
    comp[25].delay=0.712;
    
    strcpy(comp[26].name, "COMPLT");
    comp[26].size=8;
    comp[26].delay=1.901;
    
    strcpy(comp[27].name, "COMPLT");
    comp[27].size=16;
    comp[27].delay=1.529;
    
    strcpy(comp[28].name, "COMPLT");
    comp[28].size=32;
    comp[28].delay=1.641;
    
    strcpy(comp[29].name, "COMPLT");
    comp[29].size=64;
    comp[29].delay=1.865;
    
    
    
    
    strcpy(comp[30].name, "COMPEQ");
    comp[30].size=1;
    comp[30].delay=0.712;
    
    strcpy(comp[31].name, "COMPEQ");
    comp[31].size=8;
    comp[31].delay=1.024;
    
    strcpy(comp[32].name, "COMPEQ");
    comp[32].size=16;
    comp[32].delay=1.336;
    
    strcpy(comp[33].name, "COMPEQ");
    comp[33].size=32;
    comp[33].delay=1.408;
    
    strcpy(comp[34].name, "COMPEQ");
    comp[34].size=64;
    comp[34].delay=1.562;
    
    
    
    strcpy(comp[35].name, "MUX2x1");
    comp[35].size=1;
    comp[35].delay=0.768;
    
    strcpy(comp[36].name, "MUX2x1");
    comp[36].size=8;
    comp[36].delay=0.792;
    
    strcpy(comp[37].name, "MUX2x1");
    comp[37].size=16;
    comp[37].delay=0.819;
    
    strcpy(comp[38].name, "MUX2x1");
    comp[38].size=32;
    comp[38].delay=0.848;
    
    strcpy(comp[39].name, "MUX2x1");
    comp[39].size=64;
    comp[39].delay=0.852;
    
    
    strcpy(comp[40].name, "SHR");
    comp[40].size=1;
    comp[40].delay=0.712;
    
    strcpy(comp[41].name, "SHR");
    comp[41].size=8;
    comp[41].delay=1.211;
    
    strcpy(comp[42].name, "SHR");
    comp[42].size=16;
    comp[42].delay=1.440;
    
    strcpy(comp[43].name, "SHR");
    comp[43].size=32;
    comp[43].delay=1.701;
    
    strcpy(comp[44].name, "SHR");
    comp[44].size=64;
    comp[44].delay=1.705;
    
    
    strcpy(comp[45].name, "SHL");
    comp[45].size=1;
    comp[45].delay=0.712;
    
    strcpy(comp[46].name, "SHL");
    comp[46].size=8;
    comp[46].delay=1.223;
    
    strcpy(comp[47].name, "SHL");
    comp[47].size=16;
    comp[47].delay=1.374;
    
    strcpy(comp[48].name, "SHL");
    comp[48].size=32;
    comp[48].delay=1.696;
    
    strcpy(comp[49].name, "SHL");
    comp[49].size=64;
    comp[49].delay=1.674;
    
    
    strcpy(comp[50].name, "DIV");
    comp[50].size=1;
    comp[50].delay=0.693;
    
    strcpy(comp[51].name, "DIV");
    comp[51].size=8;
    comp[51].delay=9.713;
    
    strcpy(comp[52].name, "DIV");
    comp[52].size=16;
    comp[52].delay=22.916;
    
    strcpy(comp[53].name, "DIV");
    comp[53].size=32;
    comp[53].delay=52.048;
    
    strcpy(comp[54].name, "DIV");
    comp[54].size=64;
    comp[54].delay=115.463;
    
    
    
    
    strcpy(comp[55].name, "MOD");
    comp[55].size=1;
    comp[55].delay=0.928;
    
    strcpy(comp[56].name, "MOD");
    comp[56].size=8;
    comp[56].delay=10.565;
    
    strcpy(comp[57].name, "MOD");
    comp[57].size=16;
    comp[57].delay=23.337;
    
    strcpy(comp[58].name, "MOD");
    comp[58].size=32;
    comp[58].delay=52.461;
    
    strcpy(comp[59].name, "MOD");
    comp[59].size=64;
    comp[59].delay=115.939;
    
    
}

float GetDelay(char name[10], int size)
{
    for(int i=0;i<60; i++)
    {
        if(strcmp(comp[i].name, name)==0 && comp[i].size==size)
        {
            return comp[i].delay;
        }
    }
    return 0;
}
int addEdge(vector<Node *> &graph,Node src,Node dest)
{
    struct Node *temp=(struct Node *)malloc(sizeof(struct Node));
    
    temp->id=src.id;
    
    temp->delay=src.delay;
    
    strcpy(temp->color,"white");
    
    strcpy(temp->dataType,src.dataType);
    
    strcpy(temp->nodeType,src.nodeType);
    
    temp->dist=0;
    
    temp->size=src.size;
    
    strcpy(temp->name,src.name);
    
    temp->next=graph[dest.id];
    
    graph[dest.id]=temp;
    
    return 0;
}


void print(int v, vector<Node *> &graph)
{
    int i;
    struct Node *temp;
    for(i=0;i<v;i++)
    {
        temp=graph[i];
        while(temp!=NULL)
        {
            temp=temp->next;
        }
        
        
    }
    for(i=0;i<v;i++)
    {
        temp=graph[i];
        printf("node %d ",i);
        while(temp!=NULL)
        {
            printf(" -> %d %s %f %s %s %s %f %d",temp->id,temp->name,temp->delay,temp->color,temp->dataType,temp->nodeType,temp->dist,temp->size);
            cout<<endl;
            temp=temp->next;
        }
        printf("\n");
    }
}

int  tsvisit(vector<Node *> &G, std::list<Node> &L, Node *temp)
{
    struct Node *v;
    struct Node *adjtemp;
    
    strcpy(temp->color,"gray");
    
    struct Node *source;
    source = temp;
    
    while(temp->next!=NULL)
    {
        adjtemp=temp->next;
        v=G[adjtemp->id];
        
        if (strcmp(v->color,"white")==0 && strcmp(v->nodeType,"register")!=0)
        {
            tsvisit(G,L,v);
            
        }
        else if (strcmp(v->color,"white")==0 && (strcmp(v->nodeType,"register")==0) )
        {
            strcpy(v->color,"black");
            L.push_front(*v);
        }
        
        temp=temp->next;
    }
    
    strcpy(source->color,"black");
    L.push_front(*source);
    
    return 0;
}
void topologicalsort(vector<Node *> &G, std::list<Node> &L, Node *u, int v)
{
    struct Node *temp;
    
    for(int i=0;i<v;i++)
    {
        temp=G[i];
        while(temp!=NULL)
        {
            strcpy(temp->color,"white");
            temp->dist=0;
            temp=temp->next;
        }
    }
    
    temp=u;
    
    tsvisit(G,L,temp);
}



void longestpath (vector<Node *> &G, int v)
{
    float max=0.0;
    
    list<Node> L;
    struct Node *temp;
    struct Node *source;
    
    for(int i=0;i<v;i++)
    {
        temp=G[i];
        while (temp!=NULL)
        {
            if (strcmp (temp->nodeType,"input")==0 || strcmp (temp->nodeType,"output")==0)
            {
                strcpy(temp->nodeType,"register");
                
            }
            temp=temp->next;
        }
    }
    
    for(int i=0;i<v;i++)
    {
        
        temp=G[i];
        if ( strcmp (temp->nodeType,"register")==0 )
        {
            topologicalsort(G,L,temp,v);
        }
        
        if (L.empty())
        {
            // cout<<endl<<"List is empty";
        }
        
        //else cout<<endl<<"List is filled";
        
        /*       for(std::list<Node>::iterator list_iter = L.begin(); list_iter != L.end(); list_iter++)
         {
         std::cout<<endl<<list_iter->name;
         
         }
         cout<<endl;
         */
        Node n;
        
        //cout<<"List Print function completed"<<endl;
        source=G[i];
        struct Node* vertex;
        struct Node* tempnext;
        
        for(std::list<Node>::iterator list_iter = L.begin(); list_iter != L.end(); list_iter++)
        {
            n = *list_iter;
            
            vertex=G[list_iter->id];
            
            if ((strcmp(vertex->nodeType,"register")==0 && vertex==source) || strcmp(vertex->nodeType,"register")!=0  )
            {
                tempnext=vertex->next;
                while (tempnext!=NULL)
                {
                    for(std::list<Node>::iterator list_iter = L.begin(); list_iter != L.end(); list_iter++)
                    {
                        if (tempnext->id == list_iter->id)
                        {
                            if ((n.dist + list_iter->delay) > (list_iter->dist))
                            {
                                list_iter->dist = (n.dist + list_iter->delay);
                            }
                        }
                    }
                    tempnext=tempnext->next;
                    
                }
            }
        }
        
        //cout<<"Distance calculation completed"<<endl;
        
        /*      for(std::list<Node>::iterator list_iter = L.begin(); list_iter != L.end(); list_iter++)
         {
         std::cout<<endl<<list_iter->name<<" "<<list_iter->dist;
         
         }
         
         cout<<endl;
         */
        for(std::list<Node>::iterator list_iter = L.begin(); list_iter != L.end(); list_iter++)
        {
            if ( (strcmp(list_iter->nodeType,"register")==0 ) && list_iter->dist > max)
            {
                max = list_iter->dist;
            }
            
        }
        
        //cout<<"Exiting max calculate function and max is "<<max<<endl;
        
        L.clear();
    }
    
    cout<<"Critical path : "<<max<<"ns"<<endl;
    
}






