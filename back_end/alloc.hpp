#ifndef SJTU_ALLOC_HPP
#define SJTU_ALLOC_HPP
using pointer = long;
#define node_size 65536
#define new_node_num 8
class ALLOC{
    struct node{
        node *next, *prior;
        pointer loc[2];
        // int type;
        node(int l1 = 0, int l2 = 0, node *p = nullptr, node *n = nullptr):next(n), prior(p), loc(l1, l2){}
        size_t size(){
            return loc[1] - loc[0];
        }
    };
    node *head;
    pointer file_end;
    void remove(node *n){
        if (n->prior != nullptr) n->prior->next = n->next;
        else head = n->next;
        if (n->next != nullptr) n->next->prior = n->prior;
    }
    void new_mem(node *n){
        node *tmp = n;
        for (int i = 0;i < new_node_num;i++){
            tmp->next = new node(tmp->loc[1], tmp->loc[1] + node_size, tmp);
            tmp = tmp->next;
        }
        file_end += node_size * new_node_num;
    }
public:
    ALLOC(){}
    void initialize(){}
    void exit(){}
    pointer alloc(size_t s){
        node *temp = head;
        pointer p;
        if (s > node_size){
            p = file_end;
            file_end += s + 1;
            return p;
        }
        while(temp->next != nullptr && temp->size() <= s) temp = temp->next;
        if (temp->size() <= s){
            new_mem(temp);
            p = temp->next->loc[0];
            temp->next->loc[0] += s;
        }
        else{
            p = temp->head;
            temp->head += s;
            if (!temp->size) remove(temp);
        }
        return p;
    }
};
#endif