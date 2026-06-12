#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include "hashmap.h"
#include <math.h>

#include <math.h>
//
// Created by b3nov on 6/5/2026.
//

#define lockper (16*16*16)
#define START_SIZE 500
typedef struct {
    uint64_t key;
    uint64_t value;
    uint64_t lockPos;

}hashVal;


pthread_rwlock_t endlock=PTHREAD_RWLOCK_INITIALIZER;// I dont like this

typedef struct{
    hashVal * values;
    uint64_t size;
    _Atomic uint64_t opCount;
    pthread_rwlock_t *lock_t
}valueSet;

typedef struct {
    valueSet *curnt;
    valueSet *old;


    pthread_rwlock_t lock_t;
    pthread_rwlock_t swittch;

    int oldaloc;


}Table;

uint64_t gethash(uint64_t i,uint64_t l){
    return (i % ((uint64_t)((l/log(l))+1)))*log(l);
}

// warning if it hits the thing it does not unlock
hashVal* keyOeRempty(uint64_t key,int start,hashVal* hashes,uint64_t max,int read,pthread_rwlock_t *locks){
    int locked=false;
    for(int i=start;i<max;i++){
        if(read==true) {
            if (locked == false) {
                locked = true;
                pthread_rwlock_rdlock(&(locks[(hashes[i].lockPos)]));
            }
        }else if(locked==false){
            locked=true;
            pthread_rwlock_wrlock(&(locks[(hashes[i].lockPos)]));
        }
        if(key==hashes[i].key)return hashes+i;
        if(hashes[i].key==false)return hashes+i;
        if(i+1<max){
        if(hashes[i].lockPos!=hashes[i+1].lockPos){
            pthread_rwlock_unlock(&(locks[(hashes[i].lockPos)]));
            locked=false;
        }
        }else{
            pthread_rwlock_unlock(&(locks[(hashes[i].lockPos)]));
            locked=false;


    }}
    return NULL;
}



void copyData(Table* table){
    valueSet * vs = atomic_load(&(table->curnt));
    uint64_t  tableSizes =vs->size;
    hashVal * Cvlaues=vs->values;
    uint64_t lengthnew=(tableSizes*1.3*log(tableSizes))+7;
    uint64_t sizel=((lengthnew-(lengthnew%lockper))/lockper)+5;

    hashVal *  newvalues= malloc(sizeof (hashVal)*lengthnew);
    pthread_rwlock_t* locksnew= malloc(sizeof (pthread_rwlock_t)*sizel);
    for(int i =0;i<lengthnew;i++){
        //pthread_rwlock_init(&(locksnew[i]),NULL);
        newvalues[i].key=false;
        newvalues[i].lockPos= (i-(i%lockper))/lockper;
    }
    for(uint64_t i=0;i<sizel;i++)pthread_rwlock_init(&(locksnew[i]),NULL);
    uint64_t hash;
    hashVal  toput;

    for(int i=0;i<tableSizes;i++){

        toput =Cvlaues[i];
        if(toput.value!=false) {

            hash= gethash(toput.key,lengthnew);
            for(int n=hash;n<lengthnew;n++){
                if(newvalues[n].key==false){
                    newvalues[n].key=toput.key;
                    newvalues[n].value=toput.value;
                    break;
                }
            }
        }
    }
    valueSet* newVS= malloc(sizeof (valueSet));

    newVS->size =lengthnew;
    newVS->values=newvalues;
    newVS->lock_t=locksnew;

    //atomic_store(&table->deathwait,true);
    atomic_init(&(newVS->opCount),0);

    pthread_rwlock_wrlock(&(table->swittch));
    valueSet *vstemp= atomic_load(&(table->curnt));
    atomic_store(&(table->curnt),newVS);
    atomic_store(&(table->old), vstemp);
    pthread_rwlock_unlock(&(table->swittch));



    //pthread_rwlock_unlock(&(table->swittch));


}


void fraeer(void* arg){

   Table * tab =(Table *)arg;

    valueSet* vs =(tab->old);

    while (atomic_load(&(vs->opCount))!=0);
    pthread_rwlock_t *locks =vs->lock_t;
    uint64_t si=vs->size;
    si=((si-(si%lockper))/lockper)+5;
    for(uint64_t i=0;i<si;i++){
        pthread_rwlock_destroy(locks+i);
    }
    free (vs->values);
    free(vs->lock_t);
    free(vs);



}

uint64_t get(void * tab, uint64_t key){
    pthread_rwlock_rdlock(&(endlock));


    if(tab==NULL){
        pthread_rwlock_unlock(&(endlock));
        return false;
    }
    Table * t=tab;
    valueSet *vsset;
    pthread_rwlock_rdlock(&(t->swittch));
    vsset=t->curnt;
    atomic_fetch_add(&(vsset->opCount),1);
    pthread_rwlock_unlock(&(t->swittch));


    hashVal* hashVal1= vsset->values;

    uint64_t length = vsset->size;
    pthread_rwlock_t *locks=vsset->lock_t;



    //
    //
    // pthread_rwlock_unlock(&(t->swittch));
    uint64_t hash = gethash(key,length);
    hashVal1=keyOeRempty(key,hash,hashVal1,length,true,locks);
    uint64_t out;
    if(hashVal1!=NULL){
     out =hashVal1->value;
    pthread_rwlock_unlock(&(locks[hashVal1->lockPos]));}else out=false;
    atomic_fetch_sub(&(vsset->opCount),1);
    pthread_rwlock_unlock(&(endlock));
    return out;
}
int put(void *tab,uint64_t key,uint64_t value){

    Table * table=(Table *)tab;
    hashVal* loc;


    pthread_rwlock_rdlock(&(endlock));
    if(tab==NULL){
        pthread_rwlock_unlock(&(endlock));
        return -1;
    }


    pthread_rwlock_rdlock(&table->lock_t);
    uint64_t tablesize =table->curnt->size;
    uint64_t hash = gethash(key,tablesize);
    pthread_rwlock_t *locks=table->curnt->lock_t;
    while((loc=keyOeRempty(key,hash,table->curnt->values,tablesize,false,locks))==NULL){
        pthread_rwlock_unlock(&table->lock_t);
        pthread_rwlock_wrlock(&table->lock_t);
        if(tablesize !=table->curnt->size){
            pthread_rwlock_unlock(&table->lock_t);

        } else{


            copyData(table);

            fraeer(table);
            pthread_rwlock_unlock(&table->lock_t);


        }

        pthread_rwlock_rdlock(&table->lock_t);
        tablesize =table->curnt->size;
        hash = gethash(key,tablesize);
        locks=table->curnt->lock_t;
    }

    loc->key=key;
    loc->value=value;
    pthread_rwlock_unlock(&(locks[loc->lockPos]));
    pthread_rwlock_unlock(&(table->lock_t));



    pthread_rwlock_unlock(&(endlock));
    return 0;

}
void * newtable(){
    Table *newtable= malloc(sizeof(Table));
    newtable->curnt= malloc(sizeof (valueSet));
    newtable->old= malloc(1);// later code often asumses that old alredy is malocked to insure safty i was one byte
    newtable->curnt->values = malloc(sizeof(hashVal)*START_SIZE);

    uint64_t loscksize=((START_SIZE-(START_SIZE%lockper))/lockper)+5;
    newtable->curnt->lock_t=malloc(sizeof(pthread_rwlock_t)*loscksize);
    for(int i=0;i<START_SIZE;i++){
        //pthread_rwlock_init(&((newtable->curnt->lock_t)[i]),NULL);
        (newtable->curnt->values)[i].key=false;
        (newtable->curnt->values)[i].value=false;
        (newtable->curnt->values)[i].lockPos=(i-(i%lockper))/lockper;
    }
    atomic_init( &newtable->curnt->opCount,0);
    atomic_init( &newtable->curnt->opCount,0);
    for(uint64_t i=0;i<loscksize;i++)pthread_rwlock_init(&((newtable->curnt->lock_t)[i]),NULL);

    pthread_rwlock_init(&(newtable->lock_t),NULL);
    pthread_rwlock_init(&(newtable->swittch),NULL);
    pthread_rwlock_init(&(endlock),NULL);
    newtable->curnt->size=START_SIZE;
    return newtable;
}
int destroytable(void **t){



    pthread_rwlock_wrlock(&(endlock));
    if(*t==NULL){
        pthread_rwlock_unlock(&(endlock));
        return -1;
    }
    Table*  table=*t;
    free(table->curnt->values);
    pthread_rwlock_t* lock=table->curnt->lock_t;
    uint64_t locksC=table->curnt->size;
    locksC=((locksC-(locksC%lockper))/lockper)+5;
    for(uint64_t i=0;i<locksC;i++)pthread_rwlock_destroy(lock+i);
    free(lock);
    if(table->oldaloc==true)free(table->old);
    pthread_rwlock_destroy(&table->lock_t);
    pthread_rwlock_destroy(&table->swittch);

    free(table);
    pthread_rwlock_unlock(&(endlock));
    return 0;
}