#include <stdio.h>
#include <math.h>

// issue:rootノードが存在しない
// issue:子ノードを作るときの処理が間違っている

typedef struct neuron{
    int     num;                // ノード番号
    int     leafnum;            // 自身の子ノードの葉ノードの数
    int     result;             // 0 or 1
    float   weight[2];
    struct  neuron *child;
    struct  neuron *brother;
    struct  neuron *parent;
}Neuron;

void    copy(Neuron *n, float *e, int out, int num);
int     is_leaf(Neuron *n);
float   distance(float *e,float *weight);
Neuron*    test(float kyori,float *e,Neuron *winner);
void    update(Neuron *winner);
void    connect_node(Neuron *n,Neuron *winner);
Neuron* get_nearest_node(Neuron*, float*);
void show(Neuron *);
void hyouji(Neuron *);

int main(void){
    int i;
    float e[4][2] = {{0,0},{0,1},{1,0},{1,1}};
    int out[4] = {0, 1, 1, 0};
    int num = 0;                // ノード番号
    float dis;
    Neuron N[100],*winner,*answer;
    //    N[0]にノード番号num,葉ノードの数1,重みe[0]をコピー
    copy(&N[0],e[0],out[0],num);
    N[0].child   = NULL;
    N[0].brother = NULL;
    N[0].parent  = NULL;
    for (i=1;i<4;i++) {
        
        //      e[num]とN[0]の重みの距離を代入
        dis = distance(e[i], N[0].weight);
        //      winnerにN[0](root)のポインタをもたせておく
        winner = &N[0];
        //      winnerのもつ子ノードと比較しe[i]との距離が最も近いものをwinnerにする
        winner = test(dis,e[i], winner);
        //      winnerが葉ノードであったとき
        if(is_leaf(winner)) {
            num++;
            copy(&N[num], winner->weight, winner->result, num);
            connect_node(&N[num], winner);
        }
        num++;
        copy(&N[num], e[i], out[i], num);
        connect_node(&N[num], winner);
        update(winner);
    }
    show(&N[0]);
    
    float data[2] = {0.5,0.5};
    
    answer = get_nearest_node(N, data);
    
    printf("最も近いのは%dです。\n",answer->result);
    
    printf("end");
    return 0;
}

void copy(Neuron *n,float *e,int out, int num){
    n->num      = num;
    n->weight[0]= e[0];
    n->weight[1]=e[1];
    n->result = out;
    n->leafnum  = 1;
    return;
}

int is_leaf(Neuron *ptr){
    if(ptr->child == NULL) {
        return 1;
    } else {
        return 0;
    }
}

float distance(float *e,float *weight){
    float dis;
    double x,y;
    x=(double)e[0]-(double)weight[0];
    y=(double)e[1]-(double)weight[1];
    dis=(float)sqrt(x*x+y*y);
    return fabsf(dis);
}


// winnerを最も距離の短いノードに更新する
Neuron* test(float kyori, float *e, Neuron *winner){
    Neuron *ptr,*n;
    n = winner;
    float dis;
    ptr = winner->child;
    if(ptr != NULL){
        do{
            dis = distance(e, ptr->weight);
            if(dis < kyori) {
                kyori = dis;
                winner = ptr;
            }
            ptr = ptr->brother;
        }while(ptr != NULL);
        if(winner != n){
            winner = test(kyori, e, winner);
        }
    }
    return winner;
}

void update(Neuron *winner){
    Neuron *ptr;
    int leafnum;
    float omomi[2];
    
    do {
        leafnum = omomi[0]=omomi[1] = 0;       // 初期化
        
        // leafnum,weightを更新
        
        ptr = winner->child;
        do {
            leafnum += ptr->leafnum;
            omomi[0] += (ptr->weight[0] * ptr->leafnum);
            omomi[1] += (ptr->weight[1] * ptr->leafnum);
            ptr = ptr->brother;
        }while(ptr != NULL);
        
        winner->leafnum = leafnum;
        winner->weight[0] = omomi[0]/leafnum;
        winner->weight[1] = omomi[1]/leafnum;
        
        //      winnerの親ノードで同じ処理を行う
        
        winner = winner->parent;
    } while(winner != NULL);
    
    return;
}

void connect_node(Neuron *n,Neuron *winner){
    n->parent = winner;
    
    if (winner->child == NULL) {
        winner->child = n;
    } else if (winner->child->brother == NULL) {
        winner->child->brother = n;
    } else if (winner->child->brother->brother == NULL) {
        winner->child->brother->brother = n;
    } else if (winner->child->brother->brother->brother == NULL) {
        winner->child->brother->brother->brother = n;
    }
    
    //    上のif文の塊はこんな感じのwhileを使えば書き直せそうなきがする(書いてあるのは未完成)
    /***
     Neuron *ptr;
     ptr = winner->child;
     while(1) {
     if (ptr == NULL) {
     ptr = n;
     break;
     }
     ptr = ptr->brother;
     }
     ***/
    
    return;
}

void show(Neuron *n){
    Neuron *ptr;
    ptr=n->child;
    if(ptr!=NULL){
        while(ptr!=n){
            while(ptr->child!=NULL){
                ptr=ptr->child;      //子供がいる間、子供のアドレスへ移動
            }
            hyouji(ptr);
            while(ptr->brother==NULL){
                ptr=ptr->parent;
                hyouji(ptr);
                if(ptr == n){
                    break;
                }
            }
            if(ptr != n){
                ptr = ptr->brother;
            }
        }
    }
}

// detaと最も近い葉ノードを返す
Neuron* get_nearest_node(Neuron *n, float *data){
    Neuron *ptr, *nearest;
    float dis;
    float min_distance = 100;
    ptr = n->child;
    if(ptr != NULL){
        while(ptr != n){
            while(ptr->child != NULL){
                ptr = ptr->child;      //子供がいる間、子供のアドレスへ移動
            }
            
            //            ここの式の重複を直したい
            dis = distance(data, ptr->weight);
            if (min_distance > dis) {
                min_distance = dis;
                nearest = ptr;
            }
            
            while(ptr->brother == NULL){
                ptr=ptr->parent;
                if(ptr == n){
                    break;
                }
                
            }
            if(ptr!=n){
                ptr=ptr->brother;
            }
        }
    }
    return nearest;
}

void hyouji(Neuron *n){
    printf("num=%d,leafnum=%d,weight(%.2f,%.2f)\n",n->num,n->leafnum,n->weight[0],n->weight[1]);
    if(n->parent!=NULL){
        printf("parent=%d\n",n->parent->num);
    }
    if(n->brother!=NULL){
        printf("brother=%d\n",n->brother->num);
    }
    if(n->child!=NULL){
        printf("child=%d\n",n->child->num);
    }
}