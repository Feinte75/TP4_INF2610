#include <QImage>
#include <QDebug>
#include <QQueue>

#include "imagequeue.h"
#include "simpletracer.h"

ImageQueue::ImageQueue(QObject *parent, int capacity) :
    QObject(parent),
    m_capacity(capacity),
    m_sem_queue(CreateSemaphore(NULL, capacity, capacity, NULL)),
    m_sem_dequeue(CreateSemaphore(NULL, 0, capacity, NULL)),
    m_safe_modif(CreateSemaphore(NULL, 1, 1, NULL))
{
}

ImageQueue::~ImageQueue()
{
}

void ImageQueue::enqueue(QImage *item)
{
    WaitForSingleObject(m_sem_queue, INFINITE);

    WaitForSingleObject(m_safe_modif, INFINITE);
    m_queue.push(item);
    ReleaseSemaphore(m_safe_modif, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    ReleaseSemaphore(m_sem_dequeue, 1, NULL);
}

QImage *ImageQueue::dequeue()
{
    WaitForSingleObject(m_sem_dequeue, INFINITE);

    WaitForSingleObject(m_safe_modif, INFINITE);
    QImage* image = m_queue.front();
    m_queue.pop();
    ReleaseSemaphore(m_safe_modif, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    ReleaseSemaphore(m_sem_queue, 1, NULL);
    return image;
}
