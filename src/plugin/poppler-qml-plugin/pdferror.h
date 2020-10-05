#ifndef PDFERROR_H
#define PDFERROR_H

#include <QObject>

class PopplerError : public QObject
{
    Q_OBJECT
    Q_ENUMS(Error)

public:
    enum Error {
        NoError                   = 0,
        FileNotFound              = 1,
        DocumentLocked            = 2
    };
};

#endif // PDFERROR_H
