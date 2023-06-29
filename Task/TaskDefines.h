#pragma once

#include <QSharedPointer>

class AbstractTask;
using TaskId = quint64;
using TaskPtr = QSharedPointer<AbstractTask>;
