#ifndef IMAGEQUEUE_H
#define IMAGEQUEUE_H

#include <QObject>
#include <queue>
#include <windows.h>

class QImage;

class ImageQueue : public QObject
{
    Q_OBJECT
public:
    explicit ImageQueue(QObject *parent = 0, int capacity = 4);
    ~ImageQueue();
    void enqueue(QImage *item);
    QImage *dequeue();
    bool isEmpty();
private:
    int m_capacity;
    std::queue<QImage*> m_queue;
    HANDLE m_sem_queue;
    HANDLE m_sem_dequeue;
    HANDLE m_safe_modif;
};

#endif // IMAGEQUEUE_H
