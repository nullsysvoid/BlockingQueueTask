# BlockingQueueTask

It’s required to write a universal processor of several queues that meets the following requirements:
    • Insertion in the queue can occur from multiple producers (from different threads)
    • Available to subscribe / unsubscribe to parsing the queue by one fixed consumer (one queue - one consumer)
    • Increasing the number of queues and consumers does not lead to a significant drop in productivity

The handler should be thread-safe, as efficient as possible and easily extensible (in terms of code). By convention, your task is to create a universal "library" (set of classes) for working with queues. Choose the most logical and optimal option on the part of the user of this "library". Keep in mind that you, as a library developer, do not know the use cases and Key / Value types. Consider also convenience, flexibility and ease of use on the part of the user.