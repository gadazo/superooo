/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"
#include <iostream>

#define NUM_REG 32

// Structs
typedef struct {
  int dep1;
  int dep2;
  int prev_time;
  int current_time;
} Node;

typedef struct {
  unsigned int num_of_inst;
  Node** node_array;
  ///Node* node_array;
  bool* exit_node;
} Tree;

//intialize the tree
void intializeTree(Tree* tree, unsigned int numOfInsts){
  tree->num_of_inst = numOfInsts;
  tree->node_array = new Node* [numOfInsts];
 // tree->node_array = new Node [numOfInsts];
  tree->exit_node= new bool [numOfInsts];
  for (unsigned int i =0 ; i< numOfInsts ; i ++){
    tree->exit_node[i] = false;
  }
  for (unsigned int i=0 ; i<numOfInsts ; i++){
    //intialize dep to -1
    tree->node_array[i]= new Node;
    tree->node_array[i]->dep1 = -1;
    tree->node_array[i]->dep2 = -1;
    tree->node_array[i]->prev_time = 0;
    tree->node_array[i]->current_time = 0;
  }
  return;
}


ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
  try{
    Tree* tree = new Tree;
    intializeTree(tree, numOfInsts);
    int reg_file[NUM_REG];
    for (int i = 0 ; i< NUM_REG ; i++){
    reg_file[i] = -1;
    }

    for (unsigned int i =0; i < numOfInsts ; i++){
      InstInfo current_inst = progTrace[i];
      tree->node_array[i]->dep1 = reg_file[current_inst.src1Idx];
      tree->node_array[i]->dep2 = reg_file[current_inst.src2Idx];

      int latency1 = 0;
      int latency2 = 0;

      int dep1 =tree->node_array[i]->dep1;
      int dep2 =tree->node_array[i]->dep2;
      if (dep1 != -1){
        latency1 = tree->node_array[dep1]->prev_time + tree->node_array[dep1]->current_time;
        tree->exit_node[dep1] = false;
      }
      if (dep2 != -1){
        latency2 = tree->node_array[dep2]->prev_time + tree->node_array[dep2]->current_time;
        tree->exit_node[dep2] = false;
      }

      tree->node_array[i]->current_time = opsLatency[current_inst.opcode];
      tree->node_array[i]->prev_time = (latency1 > latency2) ? latency1 : latency2;

      reg_file[current_inst.dstIdx] = i;
      tree->exit_node[i] = true;
    }
    return tree;
  }
  catch (std::bad_alloc& e){
    return PROG_CTX_NULL;
  }
}

void freeProgCtx(ProgCtx ctx) {
  Tree* tree = (Tree*)ctx;
  for (unsigned int i =0 ; i< tree->num_of_inst ; i++ ){
    delete tree->node_array[i];
  }
  delete[] tree->node_array;
  delete[] tree->exit_node;
  delete tree;
  return;
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
  Tree* tree = (Tree*)ctx;
  if ((tree->num_of_inst <= theInst) || (theInst < 0))
    return -1;
  return tree->node_array[theInst]->prev_time;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
  Tree* tree = (Tree*)ctx;
  if ((tree->num_of_inst <= theInst) || (theInst < 0))
    return -1;

  *src1DepInst = tree->node_array[theInst]->dep1;
  *src2DepInst = tree->node_array[theInst]->dep2;
  return 0;
}

int getProgDepth(ProgCtx ctx) {
  Tree* tree = (Tree*)ctx;
  int max_latency = 0;
  for (unsigned int i = 0; i< tree->num_of_inst ; i++){
    if (tree->exit_node[i]){
      int current_latency =tree->node_array[i]->prev_time +tree->node_array[i]->current_time;
      max_latency = (max_latency < current_latency)? current_latency : max_latency ;
    }
  }
  return max_latency;
}


