#include "Queue.h"

QueueNode *CreateQueueNode(DataType data)
{
    QueueNode *queueNode = (QueueNode *) malloc(1 * sizeof(QueueNode));
    if (queueNode != NULL)
    {
        queueNode->data = data;
        queueNode->next = NULL;

        return queueNode;
    }
    else
    {
        printf("In function \"QueueNode *CreateQueueNode(DataType data)\" occurred error!\n");
        printf("Failed to apply for memory.");

        exit(EXIT_FAILURE);
    }
}

void QueueInit(Queue *queue)
{
    if (queue == NULL) {
        return;
    }

    QueueNode *queueNode = CreateQueueNode(0);

    queue->head = queueNode;
    queue->tail = queueNode;
}

bool IsQueueEmpty(Queue *queue)
{
    if (queue == NULL) {
        return true;
    }

    return queue->head->next == NULL ? true : false;
}

int GetQueueLength(Queue *queue)
{
    int length = 0;

    if (queue == NULL) {
        return 0;
    }

    QueueNode *queueNode = queue->head;
    while (queueNode->next != NULL)
    {
        length++;
        queueNode = queueNode->next;
    }

    return length;
}

DataType GetQueueHeadElement(Queue *queue)
{
    if (queue == NULL) {
        return NULL;
    }
    if (IsQueueEmpty(queue)) {
        return NULL;
    }

    return queue->head->next->data;
}

DataType GetQueueTailElement(Queue *queue)
{
    if (queue == NULL) {
        return NULL;
    }
    if (IsQueueEmpty(queue)) {
        return NULL;
    }

    return queue->tail->data;
}

void Enqueue(Queue *queue, DataType data)
{
    if (queue == NULL) {
        return;
    }

    QueueNode *queueNode = CreateQueueNode(data);

    if (IsQueueEmpty(queue)) {
        queue->head->next = queueNode;
        queue->tail = queue->head;
    }
    queue->tail->next = queueNode;
    queue->tail = queueNode;
}

void Dequeue(Queue *queue)
{
    if (queue == NULL) {
        return;
    }

    if (queue->head->next == NULL) {
        return;
    }

    QueueNode *queueNode = queue->head->next;
    queue->head->next = queueNode->next;
    if (IsQueueEmpty(queue)) {
        queue->tail = queue->head;
    }
    free(queueNode);
}

void QueueDestroy(Queue *queue)
{
    if (queue == NULL) {
        return;
    }

    QueueNode *headNode = queue->head;
    while (headNode->next != NULL)
    {
        QueueNode *queueNode = headNode->next;
        headNode->next = queueNode->next;
        if (queueNode->data) {
            free(queueNode->data);
        }
        free(queueNode);
    }
    free(queue->head);
}