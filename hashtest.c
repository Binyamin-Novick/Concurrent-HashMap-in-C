//
// Created by b3nov on 6/8/2026.
//
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "hashmap.h"
#include "hashmapl.c"
#include <math.h>

pthread_mutex_t printlock=PTHREAD_MUTEX_INITIALIZER;


int creattable(){
    printf("new head");
    Table *out = newtable();
    printf("sizes  %d\n",out->curnt->size);
    assert(out->curnt->size==START_SIZE);

    for(int i=0;i<START_SIZE;i++) {
        assert((out->curnt->values->key == 00));
        assert((out->curnt->values->value == 00));
    }
    printf("header compleat");
    destroytable(&out);
    return true;
    }

int BasictestHappySinglethreaded1(){
    void *t = newtable();

    // basic put/get
    put(t, 1, 10);
    assert(get(t, 1) == 10);

    put(t, 2, 20);
    assert(get(t, 2) == 20);


    put(t, 1, 99);
    assert(get(t, 1) == 99);


    assert(get(t, 2) == 20);


    assert(get(t, 999) == 0);

    destroytable(t);


    printf("1 ALL TESTS PASSED\n");
    return 0;
}
int BasictestHappySinglethreaded2(){
    void *t = newtable();


    put(t, 1, 10);
    assert(get(t, 1) == 10);

    put(t, 1, 20);
    assert(get(t, 1) == 20);


    put(t, 1111111111, 11);
    assert(get(t, 1111111111) == 11);




    assert(get(t, 999) == 0);

    destroytable(&t);


    printf("2 ALL TESTS PASSED\n");
    return 0;
}


int BasictestHappySinglethreadedC3(){
    void *t = newtable();
    printf("t3 start");
    //test colide
    uint64_t type =  (uint64_t)(START_SIZE*0.45);
    uint64_t k1=  (type * 3)+9;
    uint64_t k2=  (type * 4)+9;
    uint64_t k3=  (type * 5)+9;
    // basic put/get
    put(t, k1, 10);
    assert(get(t, k1) == 10);
    assert(("missing key should return 0", get(t, k2) == 0));

    put(t, k2, 20);
    assert(("should retrun 20",get(t, k2) == 20));
    assert(get(t, k3) == 0);
    // overwrite value
    put(t, k3, 30);
    assert(get(t, k3) == 30);





    destroytable(&t);


    printf("c3 ALL TESTS PASSED\n");
    return 0;
}





int copytest(){

    printf("this is a non api func that being tested but is need for doing the next test");
    void *t = newtable();
    for(uint64_t i=0;i<20;i++){
        put(t,i,i+1);
        assert(get(t,i)==i+1);
    }
    printf("populted table \n");
    for(uint64_t i=0;i<((Table*)t)->curnt->size;i++){
        if(((Table*)t)->curnt->values[i].key!=false){
            printf("slot" " %"  PRIu64 " ",i);
            printf("key" " %"  PRIu64 " ",((Table*)t)->curnt->values[i].key);
            printf("value" " %"  PRIu64 "\n",((Table*)t)->curnt->values[i].value);
        }
    }
    copyData(t);
    printf("copy func ran\n");
    for(uint64_t i=0;i<((Table*)t)->curnt->size;i++){
        if(((Table*)t)->curnt->values[i].key!=false){
            printf("slot" " %"  PRIu64 " ",i);
            printf("key" " %"  PRIu64 " ",((Table*)t)->curnt->values[i].key);
            printf("value" " %"  PRIu64 "\n",((Table*)t)->curnt->values[i].value);
        }
    }


    for(uint64_t i=0;i<20;i++){
        assert(get(t,i)==i+1);
    }
    printf("values still there\n");
    for(uint64_t i=0;i<20;i++){
        put(t,i,i+2);
        assert(get(t,i)==i+2);
    }
    printf("new values set\n");

    fraeer(t);
    printf("fraer ran\n");
    for(uint64_t i=0;i<20;i++){
        assert(get(t,i)==i+2);
    }
    destroytable(&t);
    printf("copytest compleate\n");
    return 0;
}



int bigTable(){
    uint64_t n =START_SIZE*250*2;
    void *t=newtable();
    for(uint64_t i=1;i<n;i++){
        put(t,i,i+1);
    }
    printf("table populted \n");

   /* for(uint64_t i=1;i<((Table*)t)->tableSizes;i++){
        if(((Table*)t)->Cvlaues[i].key!=false){
            printf("slot" " %"  PRIu64 " ",i);
            printf("key" " %"  PRIu64 " ",((Table*)t)->Cvlaues[i].key);
            printf("value" " %"  PRIu64 "\n",((Table*)t)->Cvlaues[i].value);
        }
    }*/
    for(uint64_t i=1;i<n;i++){
        if(get(t,i)!=i+1){
            printf("key" " %"  PRIu64 " ",i);
            printf("value" " %"  PRIu64 "\n", get(t,i));
            printf("atslot" " %"  PRIu64 " ",(i % ((uint64_t)(((Table*)t)->curnt->size*0.45))+1)*2);
        } else
        assert(get(t,i)==i+1);
    }
    printf("big table complete\n");
    destroytable(&t);
    return 0;

}

int singleTHreadedHappy(){
    creattable();
    BasictestHappySinglethreaded1();
    BasictestHappySinglethreaded2();
    BasictestHappySinglethreadedC3();
    copytest();
    bigTable();
    return 0;
}


double timetaken(void (*test)(uint64_t),uint64_t N){
    struct timespec start;

    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC,&start);
    test(N);
    clock_gettime(CLOCK_MONOTONIC,&end);
    return (end.tv_sec - start.tv_sec)*1000
           + (end.tv_nsec - start.tv_nsec) / 1e6;
}


int dcomp(const void *a, const void *b){
    double v1 = *( double *)a;
    double v2 = *(double *)b;
    if (v1 < v2) return -1;
    if (v1 > v2) return 1;
    return 0;
}
double  DoublingRatio(void (*test)(uint64_t),uint64_t start,uint64_t max,int robustness){
    double out=0;
    int raounds=0;
    double resolts[1000];
    double totalTime=0;
    double t1;
    double t2;
    int saize;
    for(int v=0;v<robustness;v++)
    for(uint64_t i=start;i<max;i=i*2){
        t2=timetaken(test,i*2);
        t1=timetaken(test,i);
        resolts[raounds] =t2 /t1;
        totalTime=t2+totalTime;
        raounds++;
        saize=i;
    }

    for(int i=0;i<raounds;i++){
        printf("doubling %f \n",resolts[i]);
    }
    printf("total time %f\n",totalTime);
    printf("lasttime %f for size %d\n",t2,saize*2);
    qsort(resolts,raounds, sizeof(double ),dcomp);
    return (resolts[(raounds/2)-1]+resolts[(raounds/2)+1] +resolts[raounds/2])/3;

}

void inserts(uint64_t count){
    void *t=newtable();
    for(uint64_t i=1;i<count;i++){
        put(t,i,i+1);
    }
    destroytable(&t);
}
typedef struct{
    void * T;
    int id;
    uint64_t end;
    pthread_t thr;
} inlet;

void *readerThread(void* max){

    inlet input =*((inlet*)max);
    uint64_t end =input.end;
    void * t=input.T;
    for(int i=1;i<end;i++){
        if(get(t,i)!=i+3){
            printf("real %d  expec %d\n",get(t,i),i+3);
            assert(get(t,i)==i+3);}
    }
    return NULL;

}


int mainyReaders(){
    void * t=newtable();
    for(int i=1;i<1000*200;i++){
        put(t,i,i+3);
    }
    printf("writen\n");
    inlet in[10];
    for(int i=0;i<10;i++){
        in[i].id=i;
        in[i].T=t;
        in[i].end= 20000*(i+1);

    }
    for(int i=9;i>=0;i--){
        pthread_create(&(in[i].thr),NULL,readerThread,in+i);
    }
    for(int i=0;i<10;i++){
        pthread_join(in[i].thr,NULL);
    }
    printf("mainy reads done\n");
    destroytable(&t);

}


void * writer(void *max){

    inlet input =*((inlet*)max);
    uint64_t end =input.end;
    void * t=input.T;
    int start= input.id+1;
    for(uint64_t i =(uint64_t)start;i<end;i=i+10){
        put(t,i,i+3);
    }

}

int mainyWriters(){
    void * t=newtable();
    inlet in[10];
    for(int i=0;i<10;i++){
        in[i].id=i;
        in[i].T=t;
        in[i].end= 1000*3000;

    }
    for(int i=0;i<10;i++){
        pthread_create(&(in[i].thr),NULL,writer,in+i);
    }
    for(int i=0;i<10;i++){
        pthread_join(in[i].thr,NULL);
    }
    printf("writer_complete\n");


    for(int i=9;i>=0;i--){
        pthread_create(&(in[i].thr),NULL,readerThread,in+i);
    }
    for(int i=0;i<10;i++){
        pthread_join(in[i].thr,NULL);
    }
    printf("mainy reads done\n");
    destroytable(&t);

}


void *readerThread2(void* max){

    inlet input =*((inlet*)max);
    uint64_t end =input.end;
    void * t=input.T;
    int start= input.id+1;
    for(uint64_t i=(uint64_t)start;i<end;i=i+10){
        uint64_t out =get(t,i);
        assert(out==i+3||out==i+300);
    }


}
void * writer2(void *max){

    inlet input =*((inlet*)max);
    uint64_t end =input.end;
    void * t=input.T;
    int start= input.id+1;
    for(uint64_t i =(uint64_t)start;i<end;i=i+10){
        put(t,i,i+300);
    }

}


//this is less to see the acuricy as that cant be determind becase if you have a write and a read
//  at the same time it is har to see if post or pre write the read ocers
//  what can be sceen is whether bad behavore like ded lock or memory curuption has efects
//  rather it is to see if there a dead lock
int readAndWriteSameTime(){
    void * t=newtable();
    inlet in[10];
    inlet out[10];
    for(int i=0;i<10;i++){
        in[i].id=i;
        in[i].T=t;
        in[i].end= 1000*3000;
        out[i].id=i;
        out[i].T=t;
        out[i].end= 1000*3000;
    }
    for(int i=0;i<10;i++){
        pthread_create(&(in[i].thr),NULL,writer,in+i);
    }
    for(int i=0;i<10;i++){
        pthread_join(in[i].thr,NULL);
    }
    printf("writer_complete 1\n");

    for(int i=0;i<10;i++){
        pthread_create(&(in[i].thr),NULL,writer2,in+i);
    }

    for(int i=0;i<10;i++){
        pthread_create(&(out[i].thr),NULL,readerThread2,out+i);
    }
    for(int i=0;i<10;i++){
        pthread_join(in[i].thr,NULL);


    }
    for(int i=0;i<10;i++){

        pthread_join(out[i].thr,NULL);
    }
    printf("mainy reads and cuncurent writes done\n");
    destroytable(&t);




}






void *writer5(void *arg) {
    inlet *input = (inlet *)arg;

    void *t = input->T;
    uint64_t end = input->end;
    int id = input->id;

    // stride split: avoids collisions between threads
    for (uint64_t i = id + 1; i < end; i += 5) {
        put(t, i, i + 5);
    }

    return NULL;
}
void test5Writers(uint64_t N) {
    void *t = newtable();

    inlet in[5];

    for (int i = 0; i < 5; i++) {
        in[i].id = i;
        in[i].T = t;
        in[i].end = N;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < 5; i++) {
        pthread_create(&in[i].thr, NULL, writer5, &in[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(in[i].thr, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double ms =
            (end.tv_sec - start.tv_sec) * 1000.0 +
            (end.tv_nsec - start.tv_nsec) / 1e6;

    printf("5-thread write test: %f ms for N=%lu\n", ms, N);

    destroytable(&t);
}




int main(void) {
singleTHreadedHappy();

  //  DoublingRatio(inserts,100,131072,1);
    uint64_t size=1000*3310;
    double dr;// =timetaken(inserts,size);

    //mainyReaders();
    //mainyWriters();
    //readAndWriteSameTime();
    dr =timetaken(inserts, 500 * 1000);
    printf("single threaded time %f size %d\n", dr, 500 * 1000);
    test5Writers(500 * 1000);
    dr =timetaken(inserts, 1000 * 1000);
    printf("single threaded time %f size %d\n", dr, 1000 * 1000);
    test5Writers(1000 * 1000);
    dr =timetaken(inserts, 3*1000 * 1000);
    printf("single threaded time %f size %d\n", dr, 3*1000 * 1000);
    test5Writers(3 * 1000 * 1000);
    dr =timetaken(inserts,5 * 1000 * 1000);
    test5Writers(5 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,5 * 1000 * 1000);
    dr =timetaken(inserts,10 * 1000 * 1000);
    test5Writers(7 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,7 * 1000 * 1000);
    dr =timetaken(inserts,10 * 1000 * 1000);
    test5Writers(10 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,10 * 1000 * 1000);
     dr =timetaken(inserts,10 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,10 * 1000 * 1000);
    test5Writers(20 * 1000 * 1000);
    dr =timetaken(inserts,50 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,20 * 1000 * 1000);
    test5Writers(30 * 1000 * 1000);
    dr =timetaken(inserts,100 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,30 * 1000 * 1000);
    test5Writers(40 * 1000 * 1000);
    dr =timetaken(inserts,10 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,40 * 1000 * 1000);
    test5Writers(50 * 1000 * 1000);
    dr =timetaken(inserts,50 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,50 * 1000 * 1000);
    test5Writers(60 * 1000 * 1000);
    dr =timetaken(inserts,70 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,60 * 1000 * 1000);
    test5Writers(70 * 1000 * 1000);
    dr =timetaken(inserts,70 * 1000 * 1000);
    printf("single threaded time %f size %d\n", dr,70 * 1000 * 1000);
    test5Writers(80 * 1000 * 1000);
    dr =timetaken(inserts,80 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,80 * 1000 * 1000);
    test5Writers(90 * 1000 * 1000);
    dr =timetaken(inserts,90 * 1000 * 1000);
    printf("single threaded  time %f size %d\n", dr,90 * 1000 * 1000);

}