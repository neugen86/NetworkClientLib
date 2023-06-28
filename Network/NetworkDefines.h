#pragma once

#include <QSharedPointer>

class NetworkTask;

using NetworkTaskPtr = QSharedPointer<NetworkTask>;
using NetworkTaskRef = QWeakPointer<NetworkTask>;
