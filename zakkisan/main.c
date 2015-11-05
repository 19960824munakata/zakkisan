#include <stdio.h>
#include <math.h>

typedef struct neuron{
    int     num;                // ノード番号
    int     leafnum;            // 自身の子ノードの葉ノードの数
    float   weight;
    struct  neuron *child;
    struct  neuron *brother;
    struct  neuron *parent;
}Neuron;

void    copy(Neuron *n,float e);
int     is_leaf(Neuron *n);
float   distance(float e,float weight);
void    test(float kyori,float e,Neuron *winner);
void    update(Neuron *winner,float e);
void    connect_node(Neuron *n,Neuron *winner);

int j = 0;
int e[4] = {1,2,3,4};

int main(void){
    int i;
    float kyori;
    Neuron N[100],*winner;
    copy(&N[0],(float)e[0]);
    N[0].child   = NULL;
    N[0].brother = NULL;
    N[0].parent  = NULL;
    for(i=1,j=1;i<4;i++) {
//      e[i]とN[0]の重みの距離を代入
        kyori = distance((float)e[i], N[0].weight);
//      とりあえずwinnerにN[0]のポインタをもたせておく
        winner = &N[0];
//      winnerのもつ子ノードと比較しe[i]との距離が最も近いものをwinnerにする
        test(kyori,(float)e[i], winner);
//      winnerが葉ノードであったとき
        if(is_leaf(winner)) {
            copy(&N[j], (float)e[i]);
            connect_node(&N[j], winner);
            j++;
        }
        copy(&N[j], (float)e[i]);
        connect_node(&N[j], winner);
        j++;
        update(winner, (float)e[i]);
    }
    printf("end");
    return 0;
}

void copy(Neuron *n,float e){
    n->num      = j;
    n->weight   = e;
    n->leafnum  = 1;
}

int is_leaf(Neuron *ptr){
    if(ptr->child == NULL) {
        return 1;
    } else {
        return 0;
    }
}

float distance(float e, float weight){
    return fabsf(e-weight);
}

void test(float kyori, float e, Neuron *winner){
    Neuron *ptr;
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
        test(kyori, e, winner);
    }
}

void update(Neuron *winner, float e){
    float omomi = winner->weight;
    while(winner != NULL){
        winner->weight = omomi+(e-omomi)/(float)(winner->leafnum+1);
        winner->leafnum++;
        winner = winner->parent;
    }
}

void connect_node(Neuron *n,Neuron *winner){
    n->child = NULL;
    n->brother = NULL;
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
}