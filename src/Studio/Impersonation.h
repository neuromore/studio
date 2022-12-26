#include <QObject>
#include <Core/Json.h>
#include <Core/String.h>

#ifndef __NEUROMORE_IMPERSONATION_H
#define __NEUROMORE_IMPERSONATION_H

class Impersonation : public QObject
{
    Q_OBJECT

public:
    Impersonation(QObject* parent = nullptr);

public:
    void checkUserExistence(const Core::String& uuid);
    void createUser();

public slots:
    void handleOnImpersonation(const QString& msg);

private:
    Core::Json		mJson;
};

#endif