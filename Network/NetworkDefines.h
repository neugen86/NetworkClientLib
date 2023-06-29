#pragma once

#include <QSharedPointer>

class AbstractNetworkTask;

using NetworkTaskPtr = QSharedPointer<AbstractNetworkTask>;
using NetworkTaskRef = QWeakPointer<AbstractNetworkTask>;
