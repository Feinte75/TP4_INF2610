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
    // Attendre qu il y ai de la place dans la queue
    WaitForSingleObject(m_sem_queue, INFINITE);

    // Verrouiller la queue pour ajouter l image
    WaitForSingleObject(m_safe_modif, INFINITE);
    m_queue.push(item);
    ReleaseSemaphore(m_safe_modif, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    // Signaler qu une image peut être retire de la queue
    ReleaseSemaphore(m_sem_dequeue, 1, NULL);
}

QImage *ImageQueue::dequeue()
{
    // Attendre qu il y ai une image a recuperer
    WaitForSingleObject(m_sem_dequeue, INFINITE);

    // Verrouiller la queue pour enlever une image
    WaitForSingleObject(m_safe_modif, INFINITE);
    QImage* image = m_queue.front();
    m_queue.pop();
    ReleaseSemaphore(m_safe_modif, 1, NULL);

    // tracer la taille de la file lorsqu'elle change
    SimpleTracer::writeEvent(this, 0);

    // Signaler qu une image peut être ajoute a la queue
    ReleaseSemaphore(m_sem_queue, 1, NULL);
    return image;
}
