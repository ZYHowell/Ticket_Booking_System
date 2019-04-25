#ifndef SJTU_ALLOC_HPP
#define SJTU_ALLOC_HPP
#include <stdio.h>
#include "exceptions.h"
using pointer = long;
class ALLOC{
    struct node{
        node *next, *prior;
        pointer loc[2];
        // int type;
        node(int l1 = 0, int l2 = 0, node *p = nullptr, node *n = nullptr):next(n), prior(p){
            loc[0] = l1, loc[1] = l2;
        }
        size_t size(){
            return loc[1] - loc[0];
        }
    };
    node *head;
    pointer file_end;
    const int new_node_num = 8;
    const int node_size = 65536;
    void remove(node *n){
        if (n->prior != nullptr) n->prior->next = n->next;
        else head = n->next;
        if (n->next != nullptr) n->next->prior = n->prior;
    }
    //alloc new_node_num nodes sized node_size at the end of the file
    void new_mem(node *n){
        node *tmp = n;
        tmp->next = new node(file_end, file_end + node_size, tmp);
        tmp = tmp->next;
        for (int i = 1;i < new_node_num;i++){
            tmp->next = new node(tmp->loc[1], tmp->loc[1] + node_size, tmp);
            tmp = tmp->next;
        }
        file_end += node_size * new_node_num;
    }
    void clear_node(node *n){
        if (n != nullptr){
            clear_node(n->next);
            delete n;
        }
    }
public:
    ALLOC(){}
    ~ALLOC(){
        clear_node(head);
    }
    void refill(const char *filename){
        file_end = 0;
        head = new node;
        new_mem(head);
        head = head->next;
        delete head->prior;
        head->prior = nullptr;
        save(filename);
    }
    void initialize(const char *filename){
        FILE *file;
        file = fopen(filename, "rb");
        if (!file) {
            refill(filename);
            return;
        }
        if (!fread(&file_end, sizeof(pointer), 1, file)){
            refill(filename);
            return;
        }
        node *temp = head = new node;
        while(!feof(file)){
            fread(temp->loc, sizeof(pointer), 2, file);
            // printf("%d %d ",temp->loc[0], temp->loc[1]);
            temp->next = new node;
            temp->next->prior = temp;
            temp = temp->next;
        }
        temp = temp->prior;delete temp->next;temp->next = nullptr;
        temp = temp->prior;delete temp->next;temp->next = nullptr;
        fclose(file);
    }
    void save(const char *filename){
        FILE *file;
        file = fopen(filename, "wb");
        if (!file) throw(runtime_error());
        fwrite(&file_end, sizeof(pointer), 1, file);
        node *temp = head;
        while(temp != nullptr){
            fwrite(temp->loc, sizeof(pointer), 2, file);
            // printf("%d %d ",temp->loc[0], temp->loc[1]);
            temp = temp->next;
        }
        fclose(file);
    }
    pointer alloc(size_t s){
        node *temp = head;
        pointer p;
        if (s > node_size){
            p = file_end;
            file_end += s + 1;
            return p;
        }
        if (temp == nullptr){
            temp = new node;
            new_mem(temp);
            temp = head = temp->next;
            delete temp->prior;
        }
        while(temp->next != nullptr && temp->size() <= s) temp = temp->next;
        if (temp->size() <= s){
            new_mem(temp);
            p = temp->next->loc[0];
            temp->next->loc[0] += s;
        }
        else{
            p = temp->loc[0];
            temp->loc[0] += s;
            if (!(temp->size())) remove(temp);
        }
        return p;
    }
    void free(pointer p, size_t s){
        node *temp = head;
        for (;temp != nullptr;temp = temp->next){
            if (temp->loc[1] >= p + s){
                if (temp->loc[0] == p + s) temp->loc[0] = p;
                else{
                    node *n = new node(p, p + s, temp->prior, temp);
                    if (temp->prior == nullptr) head = n;
                    else {
                        if (temp->prior->loc[1] == p) {
                            temp->prior->loc[1] = p + s;
                            delete n;
                            return;
                        }
                        else temp->prior->next = n;
                    }
                    temp->prior = n;
                }
            break;
            }
        }
    }
};
#endif