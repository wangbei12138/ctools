#ifndef __QUEUE_H__
#define __QUEUE_H__
/**
 * @file queue.h
 * Coding: utf-8
 * Author   : liushuo
 * time     : 2023/12/13 11:00
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef void* DataType;

typedef struct QueueNode
{
    DataType data;
    struct QueueNode *next;
} QueueNode;

typedef struct Queue
{
    QueueNode *head;
    QueueNode *tail;
} Queue;

QueueNode *CreateQueueNode(DataType data);

void QueueInit(Queue *queue);

bool IsQueueEmpty(Queue *queue);

int GetQueueLength(Queue *queue);

DataType GetQueueHeadElement(Queue *queue);

DataType GetQueueTailElement(Queue *queue);

void Enqueue(Queue *queue, DataType data);

void Dequeue(Queue *queue);

void QueueDestroy(Queue *queue);
#endif