#ifndef TIMETOOL_H
#define TIMETOOL_H

#include <QObject>

class timeTool : public QObject {
    Q_OBJECT
public:
    explicit timeTool(QObject *parent = nullptr);
    ~timeTool();
signals:

public slots:

private:

};

#endif // TIMETOOL_H
