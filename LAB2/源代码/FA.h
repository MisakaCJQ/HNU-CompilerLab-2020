//
// Created by cai on 2020/11/12.
// 自动机类
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <map>
#include <set>
using namespace std;

#ifndef PROJ1_FA_H
#define PROJ1_FA_H

struct Edge
{
    char c;//读入的字符
    int targetID;//目标状态的id
    Edge(char ac,int atargetID)
    {
        c=ac;
        targetID=atargetID;
    }
};

struct State
{
    //string name;//状态名
    int ID;//状态编号
    int stateType;//状态类型{0:start; 1:normal; 2:finish; 3 start&finish}
    int numEdge;
    vector<Edge> trans;//当前状态的转换表
    State(int aID,int astateType,int anumEdge)
    {
        ID=aID;
        stateType=astateType;
        numEdge=anumEdge;
        //trans.clear();
    }
    State()
    {
        ID=0;
        stateType=0;
        numEdge=0;
        trans.clear();
    }
    bool operator <(const State & a)const
    {
        return ID<a.ID;
    }
};

struct dState//用于NFA确定化
{
    int ID;
    set<int> NFAset;
    bool flag;
    dState(int aID,set<int> &aNFAset,bool aflag)
    {
        ID=aID;
        NFAset=aNFAset;
        flag=aflag;
    }
};

struct qnode//用于DFA最小化时划分子集使用
{
    int depth;
    set<int> DFAset;
    qnode(int adepth,set<int> &aDFAset)
    {
        depth=adepth;
        DFAset=aDFAset;
    }
};

class FA
{
    private:
        int FAType;//自动机的类型，0:NFA,1:DFA,2:MinDFA
        map<int,State> stateSet;//状态集合,可使用ID进行映射
        int chNum;//字符集中的字符个数
        string alphabet;//字符集
        int stateNum;//状态数
        int startStateID;//开始状态的ID
        vector<int> finishStateID;//接受状态的ID集合
    public:
        FA();
        //~FA();
        void read(ifstream &fin);//读取数据
        void print();//输出自动机数据
        set<int> getClosure(const set<int> &T);//得到T的ε-closure
        int unflagInDstates(const vector<dState> &Dstates) const;//判断Dstates中是否还有未标记的状态,有就返回其下标，否则返回-1
        set<int> getMove(const set<int> &T,char c);//状态集T输入字符c后得到的目标状态集合
        int isInDstates(const vector<dState> &dStates,const set<int> &U) const;//判断某个子集是否已经存在dstates中,存在则返回其下标，否则返回-1
        int getStatesType(const set<int> &T) const;//返回一个状态集的种类
        int DFAnextState(int stateID,char c);//返回DFA当中某一个状态输入字符c后得到的下一状态
        bool isFinishStateIDinSet(const set<int> &T) const;//判断集合中是否含有接受状态
        int stateInGroupNum(const set<set<int> >& Division,int stateID);//返回某状态在分划中的组号
        void NFAtoDFA();//NFA确定化
        void DFAminimize();//DFA最小化

};

#endif //PROJ1_FA_H
