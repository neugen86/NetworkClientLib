#pragma once

#include <QSharedPointer>

class NetworkTask;

using SharedNetworkTask = QSharedPointer<NetworkTask>;
using WeakNetworkTask = QWeakPointer<NetworkTask>;
